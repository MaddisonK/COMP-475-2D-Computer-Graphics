/*
 *  Copyright 2022 Maddison Khire
 */

#include <cmath>
#include <stdio.h>
#include <iostream>
#include <vector>
#include "GCanvas.h"
#include "GRect.h"
#include "GColor.h"
#include "GBitmap.h"
#include "GPixel.h"
#include "GShader.h"
#include "GPath.h"
#include "GMath.h"
#include "MyUtils.h"
#include "MyBitmapShader.h"
#include "MyBlend.h"
#include "MyGMatrix.h"
#include "MyGradientShader.h"
#include "MyGPath.h"
#include "MyTriColorShader.h"
#include "MyTriProxyShader.h"
#include "MyComposeShader.h"
#include "MyDrawings.h"

class MyCanvas : public GCanvas {
public:
    MyCanvas(const GBitmap& device) : fDevice(device) {}
    
    void save() {transforms.push_back(ctm());}
    void restore() {transforms.pop_back();}
    void concat(const GMatrix& matrix) {transforms.back() = GMatrix::Concat(ctm(), matrix);}

    void drawPaint(const GPaint& paint) override {
        // shading setup
        GPixel src = GColor2GPixel(paint.getColor());
        GShader* shader = paint.getShader();
        BlendFunc blendFunc = (shader) ? (shader->isOpaque()) ? (getBlendFunc(255, paint.getBlendMode())) : (getBlendFunc(10, paint.getBlendMode())) 
            : getBlendFunc(GPixel_GetA(src), paint.getBlendMode());
        if (shader) {shader->setContext(ctm());}
        GPixel row[fDevice.width()]; std::fill_n(row, fDevice.width(), src);
        
        for (int j = 0; j < fDevice.height(); j++) {
            if (shader) {
                shader->shadeRow(0, j, fDevice.width(), row);
            }
            blit(0, fDevice.width(), j, blendFunc, row, shader);
        }
    }
    
    void drawRect(const GRect& rect, const GPaint& paint) override {
        if (!rect) {return;}
        
        // transforms
        GPoint og_rect_pts[4];
        findRectanglePoints(rect, og_rect_pts);
        GPoint t_rect_pts[4];
        ctm().mapPoints(t_rect_pts, og_rect_pts, 4);
        if (!isRect(t_rect_pts, 4)) {
            this->drawConvexPolygon(og_rect_pts, 4, paint);
            return;
        }
        GRect t_rect = findPolygonBounds(t_rect_pts, 4);

        // shading setup
        GPixel src = GColor2GPixel(paint.getColor());
        GShader* shader = paint.getShader();
        BlendFunc blendFunc = (shader) ? (shader->isOpaque()) ? (getBlendFunc(255, paint.getBlendMode())) : (getBlendFunc(10, paint.getBlendMode())) 
            : getBlendFunc(GPixel_GetA(src), paint.getBlendMode());
        if (shader) {shader->setContext(ctm());}
        GPixel row[fDevice.width()]; std::fill_n(row, fDevice.width(), src);
       
        int i_min = std::min(fDevice.width(), std::max(0, GRoundToInt(t_rect.left())));
        int i_max = std::min(fDevice.width(), std::max(0, GRoundToInt(t_rect.right())));
        int j_min = std::min(fDevice.height(), std::max(0, GRoundToInt(t_rect.top())));
        int j_max = std::min(fDevice.height(), std::max(0, GRoundToInt(t_rect.bottom())));
        for (int j = j_min; j < j_max; j++) {
            if (shader) {
                shader->shadeRow(i_min, j, i_max - i_min, row);
                for (int i = i_min; i < i_max; i++) {
                    src = row[i - i_min];
                    GPixel* p = fDevice.getAddr(i, j);
                    *p = blendFunc(src, *p);
                }
            } else {
                for (int i = i_min; i < i_max; i++) {
                    GPixel* p = fDevice.getAddr(i, j);
                    *p = blendFunc(src, *p);
                }
            }
        }
    }

    void drawConvexPolygon(const GPoint points[], int count, const GPaint& paint) override {
        if (count <= 2) {return;}
        
        // transform
        GPoint t_points[count];
        ctm().mapPoints(t_points, points, count);

        if (isRect(t_points, count)) {
            GRect rect = findPolygonBounds(points, count);
            this->drawRect(rect, paint);
            return;
        }

        GPixel src = GColor2GPixel(paint.getColor());
        GShader* shader = paint.getShader();
        BlendFunc blendFunc = (shader) ? (shader->isOpaque()) ? (getBlendFunc(255, paint.getBlendMode())) : (getBlendFunc(10, paint.getBlendMode())) 
            : getBlendFunc(GPixel_GetA(src), paint.getBlendMode());
        if (shader) {shader->setContext(ctm());}
        GPixel row[fDevice.width()]; std::fill_n(row, fDevice.width(), src);

        std::vector<GEdge> edges;
        edges = clipPolygon(t_points, count, fDevice.height(), fDevice.width()); 
        if (edges.size() < 2) {return;}
        std::sort(edges.begin(), edges.end(), cmpGEdges);

        int top = edges[0].top;
        int bot = edges[edges.size()-1].bottom;
        int l = 0; int r = 1;
        GEdge LE = edges[l]; GEdge RE = edges[r];
        float lx = LE.cX; float rx = RE.cX;
        for (int y = top; y < bot; y++) {
            if (y >= LE.bottom) {
                l = std::max(l, r) + 1;
                if (l >= edges.size()) {return;}
                LE = edges[l];
                lx = LE.cX;
            }
            if (y >= RE.bottom) {
                r = std::max(l, r) + 1;
                if (r >= edges.size()) {return;}
                RE = edges[r];
                rx = RE.cX;
            }
            int Lx = GRoundToInt(lx);
            int Rx = GRoundToInt(rx);
            if (shader) {
                shader->shadeRow(Lx, y, Rx - Lx, row);
                for (int x = std::max(Lx,0); x < std::min(Rx,fDevice.width()); x++) {
                    src = row[x - Lx];
                    GPixel* p = fDevice.getAddr(x, y);
                    *p = blendFunc(src, *p);
                }
            } else {
                for (int x = std::max(Lx,0); x < std::min(Rx,fDevice.width()); x++) {
                    GPixel* p = fDevice.getAddr(x, y);
                    *p = blendFunc(src, *p);
                }
            }
            lx += LE.m;
            rx += RE.m;
        }
    }
    
    void drawPath(const GPath& path, const GPaint& paint) {
        // shading setup
        GPixel src = GColor2GPixel(paint.getColor());
        GShader* shader = paint.getShader();
        BlendFunc blendFunc = (shader) ? (shader->isOpaque()) ? (getBlendFunc(255, paint.getBlendMode())) : (getBlendFunc(10, paint.getBlendMode())) 
            : getBlendFunc(GPixel_GetA(src), paint.getBlendMode());
        if (shader) {shader->setContext(ctm());}
        GPixel row[fDevice.width()]; row[0] = src;
        
        // retrieve edges
        std::vector<GEdge> edges; 
        GPoint pts[4]; 
        GPath::Edger edger(path); 
        GPath::Verb v;
        while ((v = edger.next(pts)) != GPath::kDone) {
            if (v == GPath::kLine) {
                pts[0] = ctm() * pts[0]; pts[1] = ctm() * pts[1];
                GEdge E = GEdge::FromPoints(pts[0], pts[1]);
                if (E.top == E.bottom) {continue;}
                edges.push_back(E);
            } else if (v == GPath::kQuad) {
                pts[0] = ctm() * pts[0]; pts[1] = ctm() * pts[1]; pts[2] = ctm() * pts[2];
                std::vector<GEdge> quad_edges = generateEdgesFromQuad(pts[0], pts[1], pts[2]);
                edges.insert(edges.end(), quad_edges.begin(), quad_edges.end());
            } else if (v == GPath::kCubic) {
                pts[0] = ctm() * pts[0]; pts[1] = ctm() * pts[1]; pts[2] = ctm() * pts[2]; pts[3] = ctm() * pts[3];
                std::vector<GEdge> cubic_edges = generateEdgesFromCubic(pts[0], pts[1], pts[2], pts[3]);
                edges.insert(edges.end(), cubic_edges.begin(), cubic_edges.end());  
            }
        }
        if (edges.size() < 2) {return;}
        std::sort(edges.begin(), edges.end(), cmpGEdges);

        // blit loop
        for (int y = edges[0].top; y < std::min(edges.back().bottom, fDevice.height()); y++) {
            int i = 0; 
            int w = 0;
            float lx, rx;
            while (edges[i].active(y) && (i < edges.size())) {
                if (w == 0) { // only zero when it is a new left edge
                    lx = edges[i].eval(y);
                }
                w+=edges[i].w;
                if (w == 0) {
                    rx = edges[i].eval(y);
                    int Lx = GRoundToInt(lx);
                    int Rx = GRoundToInt(rx);
                    if (shader) {shader->shadeRow(Lx, y, Rx - Lx, row);}
                    blit(Lx, Rx, y, blendFunc, row, shader);
                }
                if (!edges[i].active(y+1)) {
                    edges.erase(edges.begin()+i);
                    if (edges.size() <= 0) {return;}
                } else { // elements shift down if element is removed anyways
                    i++;
                }
            }
            int I = 0;
            while (edges[I].active(y+1) && (I < edges.size())) {
                I++;
            }
            std::sort(edges.begin(), edges.begin()+I, [y](const GEdge &a, const GEdge &b) {return a.eval(y+1) < b.eval(y+1);});
        }
    }

    void drawMesh(const GPoint verts[], const GColor colors[], const GPoint texs[], int count, const int indices[], const GPaint& paint) override {
        int n = 0;
        for (int i = 0; i < count; ++i) {
            GPoint p0 = verts[indices[n+0]];
            GPoint p1 = verts[indices[n+1]];
            GPoint p2 = verts[indices[n+2]];
            GPoint triPts[] = {p0, p1, p2};
            if (colors && texs) {
                GColor c0 = colors[indices[n+0]];
                GColor c1 = colors[indices[n+1]];
                GColor c2 = colors[indices[n+2]];
                auto triColorSh = GCreateTriColorShader(triPts, c0, c1, c2);
                GPoint uv0 = texs[indices[n+0]];
                GPoint uv1 = texs[indices[n+1]];
                GPoint uv2 = texs[indices[n+2]];
                GPoint uvPts[] = {uv0, uv1, uv2};
                auto triProxySh = GCreateTriProxyShader(triPts, uvPts, paint.getShader());
                auto composeSh = GCreateComposeShader(triColorSh.get(), triProxySh.get());
                this->drawConvexPolygon(triPts, 3, GPaint(composeSh.get()));
            } else if (colors) { // only colors
                GColor c0 = colors[indices[n+0]];
                GColor c1 = colors[indices[n+1]];
                GColor c2 = colors[indices[n+2]];
                auto triColorSh = GCreateTriColorShader(triPts, c0, c1, c2);
                this->drawConvexPolygon(triPts, 3, GPaint(triColorSh.get()));
            } else if (texs) {
                GPoint uv0 = texs[indices[n+0]];
                GPoint uv1 = texs[indices[n+1]];
                GPoint uv2 = texs[indices[n+2]];
                GPoint uvPts[] = {uv0, uv1, uv2};
                auto triProxySh = GCreateTriProxyShader(triPts, uvPts, paint.getShader());
                this->drawConvexPolygon(triPts, 3, GPaint(triProxySh.get()));
            } else {
                // GPath path;
                // this->drawPath(path.moveTo(p0).lineTo(p1).lineTo(p2), paint);
                this->drawConvexPolygon(triPts, 3, paint);
            }
            n += 3;
        }
    }

    void drawQuad(const GPoint verts[], const GColor colors[], const GPoint texs[], int level, const GPaint& paint) override {
        int inds[] = {0, 1, 3, 1, 2, 3};
        float d = 1.0 / (level + 1);
        float u = 0; 
        GPoint gamma = verts[0];
        GPoint beta = verts[3];
        GPoint gamma_uv, beta_uv;
        GColor gamma_c, beta_c;
        for (int i = 0; i < (level+1); i++) {
            float v = 0;
            if (texs) {
                gamma_uv = (1-u)*texs[0] + u*texs[1];
                beta_uv = (1-u)*texs[3] + u*texs[2];
            }
            if (colors) {
                gamma_c = (1-u)*colors[0] + u*colors[1];
                beta_c = (1-u)*colors[3] + u*colors[2];
            }
            u+=d;
            GPoint pts[4]; GPoint uvs[4]; GColor clrs[4];
            GPoint gamma_tmp = (1-u)*verts[0] + u*verts[1];
            GPoint beta_tmp = (1-u)*verts[3] + u*verts[2];
            GPoint tmp_gamma_uv, tmp_beta_uv;
            GColor tmp_gamma_c, tmp_beta_c;
            pts[0] = gamma;
            if (texs) {
                tmp_gamma_uv = (1-u)*texs[0] + u*texs[1];
                tmp_beta_uv = (1-u)*texs[3] + u*texs[2];
            }
            if (colors) {
                tmp_gamma_c = (1-u)*colors[0] + u*colors[1];
                tmp_beta_c = (1-u)*colors[3] + u*colors[2];
            }
            for (int j = 0; j < (level+1); j++) {
                pts[1] = (1-v)*gamma_tmp + v*beta_tmp;
                v+=d;
                pts[2] = (1-v)*gamma_tmp + v*beta_tmp;
                pts[3] = (1-v)*gamma + v*beta;
                // printPoints(pts, 4);
                if (texs) {
                    uvs[0] = (1-(v-d))*gamma_uv + (v-d)*beta_uv;
                    uvs[1] = (1-(v-d))*tmp_gamma_uv + (v-d)*tmp_beta_uv;
                    uvs[2] = (1-v)*tmp_gamma_uv + (v)*tmp_beta_uv;
                    uvs[3] = (1-v)*gamma_uv + (v)*beta_uv;
                    this->drawMesh(pts, nullptr, uvs, 2, inds, paint);
                } else if (colors) {
                    clrs[0] = (1-(v-d))*gamma_c + (v-d)*beta_c;
                    clrs[1] = (1-(v-d))*tmp_gamma_c + (v-d)*tmp_beta_c;
                    clrs[2] = (1-v)*tmp_gamma_c + (v)*tmp_beta_c;
                    clrs[3] = (1-v)*gamma_c + (v)*beta_c;
                    this->drawMesh(pts, clrs, nullptr, 2, inds, paint);
                } else {
                    this->drawMesh(pts, nullptr, nullptr, 2, inds, paint);
                }
                pts[0] = pts[3];
            }
            gamma = gamma_tmp; beta = beta_tmp;
            gamma_uv = tmp_gamma_uv; beta_uv = tmp_beta_uv;
            gamma_c = tmp_gamma_c; beta_c = tmp_beta_c;
        }
    }
    // void drawQuad(const GPoint verts[], const GColor colors[], const GPoint texs[], int level, const GPaint& paint) override {
    //     std::vector<GPoint> pts;
    //     std::vector<GPoint> uv_pts;
    //     std::vector<GColor> clrs;
    //     float d = 1.0 / (level + 1);
    //     float u = 0; 
    //     GPoint gamma;
    //     GPoint beta;
    //     GPoint gamma_uv;
    //     GPoint beta_uv;
    //     GColor gamma_c;
    //     GColor beta_c;

    //     for (int i = 0; i <= (level+1); i++) {
    //         float v = 0;
    //         gamma = (1-u)*verts[0] + u*verts[1];
    //         beta = (1-u)*verts[3] + u*verts[2];
    //         if (colors) {
                
    //         }
    //         if (texs) {
    //             gamma_uv = (1-u)*texs[0] + u*texs[1];
    //             beta_uv = (1-u)*texs[3] + u*texs[2];
    //         }
    //         for (int j = 0; j <= (level+1); j++) {
    //             GPoint p = (1-v)*gamma + v*beta;
    //             printPoints(&p, 1);
    //             pts.push_back((1-v)*gamma + v*beta);
    //             if (texs) {
    //                 uv_pts.push_back((1-v)*gamma_uv + v*beta_uv);
    //             }
    //             if (colors) {
    //                 clrs.push_back(GColor());
    //             }
    //             v+=d;
    //         }
    //         u+=d;
    //     }
    //     for (int i=0; i<(level+1); i++) {
    //         for (int j = 0; j<(level+1); j++) {
    //             GPoint tVerts[4];
    //             int z = j*(level+2)+i;
    //             tVerts[0] = pts[z];
    //             tVerts[3] = pts[z+1];
    //             tVerts[1] = pts[z+level+2];
    //             tVerts[2] = pts[z+level+3];
    //             int tIs[] = {0, 3, 1, 2, 3, 1};
    //             if (texs) {
    //                 GPoint tUVs[4];
    //                 tUVs[0] = uv_pts[z];
    //                 tUVs[3] = uv_pts[z+1];
    //                 tUVs[1] = uv_pts[z+level+2];
    //                 tUVs[2] = uv_pts[z+level+3];
    //                 this->drawMesh(tVerts, nullptr, tUVs, 2, tIs, paint);
    //                 continue;
    //             } 
    //             if (colors) {
    //                 this->drawMesh(tVerts, nullptr, nullptr, 2, tIs, paint);
    //                 continue;
    //             }
    //             this->drawMesh(tVerts, nullptr, nullptr, 2, tIs, paint);
    //         }
    //     }
    // }

private:
    // Note: we store a copy of the bitmap
    const GBitmap fDevice;
    std::vector<GMatrix> transforms = {GMatrix()};

    GMatrix ctm() {
        return transforms.back();
    }

    void blit(int l, int r, int y, const BlendFunc blendFunc, const GPixel* row, bool hasShader) {
        if (y < 0) {return;}
        l = std::max(l,0);
        for (int x = l; x < std::min(r, fDevice.width()); x++) {
            GPixel *p = fDevice.getAddr(x, y);
            *p = (hasShader) ? blendFunc(row[x-l], *p) : blendFunc(row[0], *p);
            // *p = GColor2GPixel(GColor::RGBA(1, .1, 1, .5)); // DEBUG override
        }
    }
};

std::unique_ptr<GCanvas> GCreateCanvas(const GBitmap& device) {
    return std::unique_ptr<GCanvas>(new MyCanvas(device));
}

std::string GDrawSomething(GCanvas* canvas, GISize dim) {
    // return TestDrawQuad(canvas, dim);
    // return TestDrawTri(canvas, dim);
    return SunsetDrawing(canvas, dim);
}

