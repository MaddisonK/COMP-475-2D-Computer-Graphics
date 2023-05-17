#include "GPixel.h"
#include "GColor.h"
#include "GPoint.h"
#include <stdio.h>
#include <algorithm>
#include <iostream>
#include "MyGEdge.h"

GPixel GColor2GPixel(const GColor& color) {
    GColor pin = color.pinToUnit();
    unsigned int Pa = pin.a * 255 + 0.5;
    unsigned int Pr = pin.a * pin.r * 255 + 0.5;
    unsigned int Pg = pin.a * pin.g * 255 + 0.5;
    unsigned int Pb = pin.a * pin.b * 255 + 0.5;
    return GPixel_PackARGB(Pa, Pr, Pg, Pb);
}

inline uint8_t divideBy255(unsigned n) {
    return (n + 128) * 257 >> 16;
}

GRect findPolygonBounds(const GPoint points[], int n) {
    float L,T,R,B;
    L = R = points[0].x();
    T = B = points[0].y();
    for (int i = 1; i < n; i++) {
        L = std::min(L, points[i].x());
        R = std::max(R, points[i].x());
        T = std::min(T, points[i].y());
        B = std::max(B, points[i].y());
    }
    return GRect::LTRB(L,T,R,B);
}

/* Caller must allocate pts to be length 4 */
void findRectanglePoints(const GRect& rect, GPoint pts[]) {
    pts[0] = {rect.left(), rect.top()};
    pts[1] = {rect.right(), rect.top()};
    pts[2] = {rect.right(), rect.bottom()};
    pts[3] = {rect.left(), rect.bottom()};
}

bool isRectOutOfBounds(GRect rect, GBitmap bitmap) {
    if (GRoundToInt(rect.bottom()) > bitmap.height()) {
        return true;
    } else if (GRoundToInt(rect.top()) < 0) {
        return true;
    } else if (GRoundToInt(rect.right() > bitmap.width())) {
        return true;
    } else if (GRoundToInt(rect.left()) < 0) {
        return true;
    } else {
        return false;
    }
}

bool clipPointsVert(GPoint &p0, GPoint &p1, int y0, int y1); // y0 is top, y1 is bot, y0 < y1

void clipPointsHor(GPoint &p0, GPoint &p1, int x0, int x1, std::vector<GEdge> &clipped_edges); // x0 = 0 

std::vector<GEdge> clipPoints(GPoint p0, GPoint p1, int height, int width) {
    std::vector<GEdge> clipped_edges;
    if (clipPointsVert(p0, p1, 0, height)) {
        clipPointsHor(p0, p1, 0, width, clipped_edges);
    };
    return clipped_edges;
}

std::vector<GEdge> clipPolygon(const GPoint points[], int n, int height, int width) {
    std::vector<GEdge> clipped_edges;
    for (int i = 0; i < n; i++) {
        GPoint p0; GPoint p1;
        if (i == n - 1) {
            p0 = points[n-1]; p1 = points[0];
        } else {
            p0 = points[i]; p1 = points[i+1];
        }
        if (clipPointsVert(p0, p1, 0, height)) {
            clipPointsHor(p0, p1, 0, width, clipped_edges);
        };
    }    
    return clipped_edges;
}


void clipY(GPoint &p0, GPoint &p1, int y) { // p0 is the point being clipped
    p0.fX = p0.x() + (p0.x() - p1.x()) * (y - p0.y()) / (p0.y() - p1.y());
    p0.fY = y;
}

void clipX(GPoint &p0, GPoint &p1, int x) { // p0 is the point being clipped
    p0.fY = p0.y() - (p0.x() - x) * (p0.y() - p1.y()) / (p0.x() - p1.x());
    p0.fX = x;
}

void clipPointsHor(GPoint &p0, GPoint &p1, int x0, int x1, std::vector<GEdge> &clipped_edges) {
    if ((p0.x() < x0 && p1.x() < x0)) { // both points are to the left of x0
        p0.fX = x0;
        p1.fX = x0;
        clipped_edges.push_back(GEdge::FromPoints(p0, p1));
        return;
    }
    if ((p0.x() > x1 && p1.x() > x1)) { // both points are to the right of x1
        p0.fX = x1;
        p1.fX = x1;
        clipped_edges.push_back(GEdge::FromPoints(p0, p1));
        return;
    }
    if (p1.x() < p0.x()) {std::swap(p1, p0);} // to ensure p0 will be to the left
    if (p0.x() < x0) {
        GPoint q0 = {x0, p0.y()};
        clipX(p0, p1, x0);
        clipped_edges.push_back(GEdge::FromPoints(q0, p0));
    }
    if (p1.x() > x1) {
        GPoint q1 = {x1, p1.y()};
        clipX(p1, p0, x1);
        clipped_edges.push_back(GEdge::FromPoints(q1, p1));
    }
    clipped_edges.push_back(GEdge::FromPoints(p0, p1));
}

bool clipPointsVert(GPoint &p0, GPoint &p1, int y0, int y1) {
    if (GRoundToInt(p0.y()) == GRoundToInt(p1.y())) { // edge is horizontal -> discard
        return false;}
    if ((p0.y() < y0 && p1.y() < y0) || (p0.y() > y1 && p1.y() > y1)) { // both points are outside one side of the bounds -> discard
        return false;}
    
    if (p1.y() < p0.y()) {std::swap(p1, p0);} // to ensure p0 will be on top
    if (p0.y() < y0) {
        clipY(p0, p1, y0);
    }
    if (p1.y() > y1) {
        clipY(p1, p0, y1);
    }
    return true;
}

bool isRect(const GPoint points[], int n) {
    if (n != 4) {return false;}
    for (int i = 0; i < n; i++) {
        GPoint p0 = points[i];
        GPoint p1 = points[(i+1) % n];
        if (p0.x() == p1.x() || p0.y() == p1.y()) {
            continue;
        } else {return false;}
    }
    return true;
}

float roundf(float f) {
    return f;
}

void printPoints(const GPoint pts[], int count) {
    printf("points: [");
    for (int i = 0; i < count; i++) {
        printf("%d: (%f, %f), ", i, roundf(pts[i].x()), roundf(pts[i].y()));
    }
    printf("]\n");
}

GPoint solveQuad(GPoint& A, GPoint& B, GPoint& C, float t) {
    float t_inv = 1 - t;
    return t_inv * t_inv * A + 2 * B * t * t_inv + C * t * t;
}

std::vector<GEdge> generateEdgesFromQuad(GPoint& A, GPoint& B, GPoint& C) {
    std::vector<GEdge> edges;
    GPoint abc = (A - B - B + C) * .25;
    float d = abc.length();
    int l = std::max(1, GCeilToInt(sqrtf(d * 4)));
    // printf("d: %f, l: %d \n", d, l);
    float spacing = 1.0f / l;
    // printf("spacing: %f\n", spacing);
    
    float t = 0;
    GPoint p0;
    GPoint p1;
    for (int i = 0; i < l; i++) {
        p0 = solveQuad(A, B, C, t);
        t+=spacing;
        p1 = solveQuad(A, B, C, t);
        GEdge edge = GEdge::FromPoints(p0, p1);
        if (edge.top == edge.bottom) {continue;}
        edges.push_back(GEdge::FromPoints(p0, p1));
    }
    return edges;
}

GPoint solveCubic(GPoint& A, GPoint& B, GPoint& C, GPoint& D, float t) {
    GPoint a = (D - A) + 3.0 * (B - C);
    GPoint b = 3.0 * ((C - B) + (A - B));
    return ((a*t + b)*t + 3.0 * (B - A))*t + A;
}

std::vector<GEdge> generateEdgesFromCubic(GPoint& A, GPoint& B, GPoint& C, GPoint& D) {
    std::vector<GEdge> edges;
    GPoint abc = A - B - B + C;
    GPoint bcd = B - C - C + D;

    float dx = std::max(std::abs(abc.x()), std::abs(bcd.x()));
    float dy = std::max(std::abs(abc.y()), std::abs(bcd.y()));
    float d = sqrtf((GPoint) {dx, dy}.length() * 3);
    int l = std::max(1, GCeilToInt(d));
    // printf("l: %d \n", l);
    float spacing = 1.0f / l;
    // printf("spacing: %f\n", spacing);

    float t = 0;
    GPoint p0;
    GPoint p1;
    for (int i = 0; i < l; i++) {
        p0 = solveCubic(A, B, C, D, t);
        t+=spacing;
        p1 = solveCubic(A, B, C, D, t);
        GEdge edge = GEdge::FromPoints(p0, p1);
        if (edge.top == edge.bottom) {continue;}
        edges.push_back(edge);
    }
    return edges;
}