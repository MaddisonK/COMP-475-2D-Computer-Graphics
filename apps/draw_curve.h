/**
 *  Copyright 2021 Mike Reed
 */

#ifndef GDrawCurveShape_DEFINED
#define GDrawCurveShape_DEFINED

#include "GPath.h"

class CurveShape : public Shape {
protected:
    float fSubT = 0.5f;
    bool  fShowHalves = false;
    bool  fShowPoints = false;

    bool handleSym(uint32_t sym) override {
        if (sym == 'h') {
            fShowHalves = !fShowHalves;
            return true;
        }
        if (sym == 'p') {
            fShowPoints = !fShowPoints;
            return true;
        }

        constexpr float DT = 1.0f/64;

        if (sym == '-') {
            fSubT = std::max(fSubT - DT, 0.f);
            return true;
        }
        if (sym == '=') {
            fSubT = std::min(fSubT + DT, 1.f);
            return true;
        }
        return this->Shape::handleSym(sym);
    }
    
    void drawPoints(GCanvas* canvas, const GPoint pts[], int N) const {
        const float dt = 1.0f / 32;
        const float r = 2.3f;
        GPaint paint({1,0,0,1});

        for (float t = 0; t <= 1; t += dt) {
            GPoint dst[7], p;

            if (t > 0.5f) {
                paint.setColor({0,0,1,1});
            }

            if (N == 3) {
                GPath::ChopQuadAt(pts, dst, t);
                p = dst[2];
            } else {
                GPath::ChopCubicAt(pts, dst, t);
                p = dst[3];
            }
            canvas->drawRect(GRect::LTRB(p.fX - r, p.fY - r, p.fX + r, p.fY + r), paint);
        }
    }
};

#endif
