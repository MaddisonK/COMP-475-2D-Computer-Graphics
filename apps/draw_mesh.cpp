/**
 *  Copyright 2017 Mike Reed
 */

#include "GPoint.h"
#include "GMatrix.h"
#include "../mike_mesh.h"
#include <vector>

static bool mesh_hit_test(float x0, float y0, float x1, float y1) {
    const float dx = fabs(x1 - x0);
    const float dy = fabs(y1 - y0);
    return std::max(dx, dy) <= 9;
}


static float   fExp = 1;

enum GridLines {
    kNone,
    kQuads,
    kTriangles,
    
    kLast = kTriangles,
};

constexpr GColor gColors[] = {
    {1,0,0,1}, {0,1,0,1}, {0,0,1,1}, {0.75,0.5,0.25,1},
    {1,1,0,1}, {0,1,1,1}, {1,0,1,1}, {0.25,0.5,0.75,1},
};
constexpr int NUM_COLOR_SETS = sizeof(gColors) / (sizeof(GColor) * 4);

class MeshShape : public Shape {
    GPoint  fPts[4];
    GColor  fColors[4 * NUM_COLOR_SETS];
    GBitmap fBitmap, fBitmap1;
    GPoint  fOffCurve[8];
    
    // Options
    //
    // levels: '-' '='
    // Auto_levels: 1/0
    //
    // Textures: cycle
    // Colors: cycle
    //
    // show Diagonal: 1/0

    int     fLevel = 1;
    float   fTexScale = 1;
    GPoint  fTexTrans = { 0, 0 };
    int     fCornerIndex = 0;
    bool    fShowDiag = false;
    bool    fOffDiag = false;

    int     fShowColors = 0;
    int     fShowTexs = 0;

    bool    fUseCProc = false;
    bool    fUseEProc = false;
    bool    fAutoLevels = false;

    static GPoint eval_cubic(GPoint p0, GPoint p1, GPoint p2, GPoint p3, float t0) {
        float t1 = 1 - t0;
        return t1*t1*t1*p0 + 3*t1*t1*t0*p1 + 3*t1*t0*t0*p2 + t0*t0*t0*p3;
    }

    GPoint edge_proc(MeshEdge edge, float t) {
        GPoint p0, p3;
        switch (edge) {
            case MeshEdge::kTop:
                p0 = fPts[0];
                p3 = fPts[1];
                break;
            case MeshEdge::kRight:
                p0 = fPts[1];
                p3 = fPts[2];
                break;
            case MeshEdge::kBottom:
                p0 = fPts[3];
                p3 = fPts[2];
                break;
            case MeshEdge::kLeft:
                p0 = fPts[0];
                p3 = fPts[3];
                break;
        }
        return eval_cubic(p0, fOffCurve[edge*2+0], fOffCurve[edge*2+1], p3, t);

    }

    static void draw_line(GCanvas* canvas, GPoint a, GPoint b, const GPaint& paint) {
        GVector v = b - a;
        float scale = 1.0f / v.length();
        GPath path;
        v = { v.fY * scale, -v.fX * scale };

        path.moveTo(a + v).lineTo(b + v).lineTo(b - v).lineTo(a - v);
        canvas->drawPath(path, paint);
    }
    static void draw_poly_line(GCanvas* canvas, const GPoint pts[], int count, const GPaint& paint) {
        for (int i = 0; i < count - 1; ++i) {
            draw_line(canvas, pts[i], pts[i+1], paint);
        }
    }

    static float lerp(float a, float b, float t) {
        return (1 - t)*a + t*b;
    }
    static GPoint lerp(GPoint a, GPoint b, float t) {
        return { lerp(a.fX, b.fX, t), lerp(a.fY, b.fY, t) };
    }
    void show_diagonal(GCanvas* canvas, int level) {
        std::vector<GPoint> diag0, diag1;

        const float dt = 1.0 / level;
        GPaint paint;
        paint.setColor({ .75, .75, .75, 1 });
        float t = 0;
        for (int i = 0; i <= level; ++i) {
            GPoint u0 = lerp(fPts[0], fPts[1], t);
            GPoint u1 = lerp(fPts[3], fPts[2], t);
            diag0.push_back(lerp(u0, u1, t));
            diag1.push_back(lerp(u0, u1, 1 - t));
            t += dt;
            t = std::min(t, 1.0f);
        }

        if (fShowDiag) {
            paint.setColor({1, 0, 0, 1 });
            draw_poly_line(canvas, diag0.data(), diag0.size(), paint);
            paint.setColor({0, 0, 1, 1 });
            draw_poly_line(canvas, diag1.data(), diag1.size(), paint);
        }
    }

    static void init_off(GPoint* pts, GPoint a, GPoint b) {
        pts[0] = lerp(a, b, 1.0f / 3);
        pts[1] = lerp(a, b, 2.0f / 3);
    }

public:
    MeshShape() {
        fPts[0] = {20,   20};
        fPts[1] = {400,  20};
        fPts[2] = {400, 400};
        fPts[3] = {20,  400};

        fColors[0] = { 1, 0, 0, 1 };
        fColors[1] = { 0, 1, 0, 1 };
        fColors[2] = { 0, 0, 1, 1 };
        fColors[3] = { 0.5,0.5,0.5, 1 };

        fBitmap.readFromFile("apps/spock.png");
        fBitmap1.readFromFile("apps/oldwell.png");
    }

    static GColor color_proc(const GColor[], float u, float v) {
        float uu = sin(u * M_PI);
        float vv = sin(v * M_PI);
        float scale = powf(uu * vv, fExp);
        return { scale, scale, scale, 1 };
    }

    static int compute_level(const GPoint p[4], float tolerance) {
        GVector err = (p[0] - p[1]) + (p[2] - p[3]);
        int level = GRoundToInt(sqrtf(err.length() / tolerance));
        level = std::max(level, 1);
        return level;
    }
    
    enum { NUM_SHADERS = 4 };

    std::unique_ptr<GShader> bmShader(const GBitmap& bm) const {
        GMatrix mx = GMatrix::Scale(fTexScale / bm.width(), fTexScale / bm.height())
                   * GMatrix::Translate(fTexTrans.x(), fTexTrans.y());
        return GCreateBitmapShader(bm, mx, GShader::kRepeat);
    }
    
    std::unique_ptr<GShader> makeShader() const {
        switch (fShowTexs) {
            case 1: return bmShader(fBitmap);
            case 2: return bmShader(fBitmap1);
            case 3: {
                GPoint tx = fTexTrans * (1.0f / 256);
                GPoint pts[] = { tx + GPoint{0, 0}, tx + GPoint{1, 0} };
                return GCreateLinearGradient(pts[0], pts[1], gColors, GARRAY_COUNT(gColors),
                                             GShader::TileMode::kMirror);
            } break;
        }
        return nullptr;
    }

    void onDraw(GCanvas* canvas, const GPaint&) override {
        std::unique_ptr<GShader> shader = makeShader();
        GPaint paint(shader.get());

        const GColor* colors = nullptr;
        if (fShowColors > 0) {
            colors = &gColors[(fShowColors - 1) * 4];
        }

        std::function<GPoint(MeshEdge, float)> eproc = [this](MeshEdge e, float t) {
            return edge_proc(e, t);
        };
        const int level = fAutoLevels ? compute_level(fPts, 2) : fLevel;

        mike_mesh(canvas, fPts, colors, paint, level, fOffDiag,
                  fUseCProc ? color_proc : nullptr,
                  fUseEProc ? eproc : nullptr);

       // if (!fShowTriangles) {
       //     show_quad_lines(canvas, fPts, level);
        //}

        if (fShowDiag) {
            this->show_diagonal(canvas, fLevel);
        }

        if (fUseEProc) {
            GPaint pn0, pn1({1,1,1,1});
            for (GPoint p : fOffCurve) {
                canvas->drawRect(GRect::XYWH(p.fX - 4, p.fY - 4, 8, 8), pn0);
                canvas->drawRect(GRect::XYWH(p.fX - 2, p.fY - 2, 4, 4), pn1);
            }
        }
    }

    GRect getRect() override {
        return GRect::WH(200, 200);
    }

    GColor onGetColor() override { return fColors[fCornerIndex]; }
    void onSetColor(const GColor& c) override { fColors[fCornerIndex] = c; }

    GClick* findClick(GPoint pt, GWindow* wind) override {
        for (int i = 0; i < 4; ++i) {
            if (mesh_hit_test(fPts[i].fX, fPts[i].fY, pt.fX, pt.fY)) {
                fCornerIndex = i;
                return new GClick(pt, [this, wind, i](GClick* click) {
                    fPts[i] = click->curr();
                    wind->requestDraw();
                });
            }
        }
        if (fUseEProc) {
            for (GPoint& oc : fOffCurve) {
                if (mesh_hit_test(oc.fX, oc.fY, pt.fX, pt.fY)) {
                    GPoint* ocPtr = &oc;
                    return new GClick(pt, [wind, ocPtr](GClick* click) {
                        *ocPtr = click->curr();
                        wind->requestDraw();
                    });
                }
            }
        }
        return new GClick(pt, [this, wind](GClick* click) {
            fTexTrans += click->curr() - click->prev();
            wind->requestDraw();
        });
    }

    void drawHilite(GCanvas*) override {  }

    void dumpCommands() const {
        const char* str = "Levels '-' and '=' -- [A]uto\n"
                          "[C]olors [T]extures\n"
                          "[D]iagonals  [O]ff_diagonals\n"
                          "Up/Down texture scaling\n";
        printf("%s\n", str);
    }

    bool handleSym(uint32_t sym) override {
        switch (sym) {
            case '/': this->dumpCommands(); return true;
            case '-': fLevel = std::max(fLevel - 1, 1); return true;
            case '=': fLevel = std::min(fLevel + 1, 64); return true;
            case 'c': fShowColors = (fShowColors + 1) % (NUM_COLOR_SETS + 1); return true;
            case 'd': fShowDiag = !fShowDiag; return true;
            case 'o': fOffDiag = !fOffDiag; return true;
            case 't': fShowTexs = (fShowTexs + 1) % NUM_SHADERS; return true;
            case SDLK_UP: fTexScale *= 1.25f; return true;
            case SDLK_DOWN: fTexScale *= 0.8f; return true;
            case '[': fExp *= .9f; return true;
            case ']': fExp *= 1.1f; return true;
            case 'a': fAutoLevels = !fAutoLevels; return true;
            case 'p': fUseCProc = !fUseCProc; return true;
            case 'e':
                fUseEProc = !fUseEProc;
                if (fUseEProc) {
                    init_off(&fOffCurve[0], fPts[0], fPts[1]);
                    init_off(&fOffCurve[2], fPts[1], fPts[2]);
                    init_off(&fOffCurve[4], fPts[3], fPts[2]);
                    init_off(&fOffCurve[6], fPts[0], fPts[3]);
                }
                return true;
            default:
                break;
        }
        return false;
    }
};

Shape* MeshShape_Factory() { return new MeshShape; }
