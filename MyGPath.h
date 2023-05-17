#include "GPath.h"
#include "GMatrix.h"
#include "GPoint.h"
#include "GRect.h"

const float UCUBE = 0.551915f;

GPath& GPath::addRect(const GRect& rect, Direction dir) {
    this->moveTo({rect.fLeft, rect.fTop});
    if (dir == Direction::kCW_Direction) {
        this->lineTo({rect.fRight, rect.fTop});
        this->lineTo({rect.fRight, rect.fBottom});
        this->lineTo({rect.fLeft, rect.fBottom});
    } else 
    if (dir == Direction::kCCW_Direction) {
        this->lineTo({rect.fLeft, rect.fBottom});
        this->lineTo({rect.fRight, rect.fBottom});
        this->lineTo({rect.fRight, rect.fTop});
    }
    return *this;
}

GPath& GPath::addPolygon(const GPoint pts[], int count) {
    this->moveTo({pts[0].x(), pts[0].y()});
    for (int i = 1; i < count; i++) {
        this->lineTo({pts[i].x(), pts[i].y()});
    }
    return *this;
}

GRect GPath::bounds() const {
    if (fPts.size() < 1) {return GRect::LTRB(0,0,0,0);}
    float L,T,R,B;
    L = R = fPts[0].x();
    T = B = fPts[0].y();
    for (int i = 1; i<fPts.size(); i++) {
        L = std::min(L, fPts[i].x());
        R = std::max(R, fPts[i].x());
        T = std::min(T, fPts[i].y());
        B = std::max(B, fPts[i].y());
    }
    return GRect::LTRB(L,T,R,B);
}

void GPath::transform(const GMatrix& m) {
    for (int i = 0; i < fPts.size(); i++) {
        m.mapPoints(&fPts[i], 1);
    }
}

GPoint lerpPoints(const GPoint& p0, const GPoint& p1, float t) {
    return (1-t) * p0 + t * p1;
}

void GPath::ChopQuadAt(const GPoint src[3], GPoint dst[5], float t) {
    dst[0] = src[0];
    dst[1] = lerpPoints(src[0], src[1], t);
    dst[3] = lerpPoints(src[1], src[2], t);
    dst[2] = lerpPoints(dst[1], dst[3], t);
    dst[4] = src[2];
}

void GPath::ChopCubicAt(const GPoint src[4], GPoint dst[7], float t) {
    dst[0] = src[0];
    dst[1] = lerpPoints(src[0], src[1], t);
    GPoint bc = lerpPoints(src[1], src[2], t);
    dst[2] = lerpPoints(dst[1], bc, t);
    dst[5] = lerpPoints(src[2], src[3], t);
    dst[4] = lerpPoints(bc, dst[5], t);
    dst[3] = lerpPoints(dst[2], dst[4], t);
    dst[6] = src[3];
}

GPath& GPath::addCircle(GPoint center, float radius, GPath::Direction dir) {
    GMatrix mx = GMatrix::Translate(center.x(), center.y()) * GMatrix::Scale(radius, radius);
    if (dir == Direction::kCW_Direction) {
        this->moveTo(mx*GPoint{0, -1});
        this->cubicTo(mx*GPoint{UCUBE, -1}, mx*GPoint{1, -UCUBE}, mx*GPoint{1, 0});
        this->cubicTo(mx*GPoint{1, UCUBE}, mx*GPoint{UCUBE, 1}, mx*GPoint{0, 1});
        this->cubicTo(mx*GPoint{-UCUBE, 1}, mx*GPoint{-1, UCUBE}, mx*GPoint{-1, 0});
        this->cubicTo(mx*GPoint{-1, -UCUBE}, mx*GPoint{-UCUBE, -1}, mx*GPoint{0, -1});
    } else {
        this->moveTo(mx*GPoint{0, -1});
        this->cubicTo(mx*GPoint{-UCUBE, -1}, mx*GPoint{-1, -UCUBE}, mx*GPoint{-1, 0});
        this->cubicTo(mx*GPoint{-1, UCUBE}, mx*GPoint{-UCUBE, 1}, mx*GPoint{0, 1});
        this->cubicTo(mx*GPoint{UCUBE, 1}, mx*GPoint{1, UCUBE}, mx*GPoint{1, 0});
        this->cubicTo(mx*GPoint{1, -UCUBE}, mx*GPoint{UCUBE, -1}, mx*GPoint{0, -1});
    }
    return *this;
}
