/**
 *  Copyright 2018 Mike Reed
 */

#include "draw_curve.h"

class QuadShape : public CurveShape {
    GPoint fPts[3];
public:
    QuadShape() {
        fPts[0] = { 20, 100 };
        fPts[1] = { 100, 20 };
        fPts[2] = { 160, 120 };
        fColor = { 1, 0, 0, 1 };
    }

    void onDraw(GCanvas* canvas, const GPaint& paint) override {
        if (fShowPoints) {
            this->drawPoints(canvas, fPts, 3);
            return;
        }
        GPath path;
        path.moveTo(fPts[0]).quadTo(fPts[1], fPts[2]);
        canvas->drawPath(path, paint);
        if (fShowHalves) {
            GPoint dst[5];
            GPath::ChopQuadAt(fPts, dst, fSubT);
            if (false) {
            path.reset().moveTo(dst[0]).quadTo(dst[1], dst[2]);
            canvas->drawPath(path, GPaint({0.5, 0, 1, 0}));

            path.reset().moveTo(dst[2]).quadTo(dst[3], dst[4]);
            canvas->drawPath(path, GPaint({0.5, 0, 0, 1}));
            }

            path.reset();
            for (int i = 1; i <= 3; ++i) {
                path.addCircle(dst[i], 4);
            }
            canvas->drawPath(path, GPaint());
        }
    }

    void drawHilite(GCanvas* canvas) override {
        auto add_dashed_line = [](GPath* path, GPoint a, GPoint b) {
            GVector v = b - a;
            const float len = v.length();
            const float dist = 16;
            const GVector dv = v * (dist / len);
            a = a + dv * 0.5f;
            const float R = 1.4f;
            for (float d = dist * 0.5f; d < len; d += dist) {
                path->addRect(GRect::LTRB(a.fX - R, a.fY - R, a.fX + R, a.fY + R));
                a = a + dv;
            }
        };
        GPaint paint;
        GPath path;
        const int count = GARRAY_COUNT(fPts);
        for (int i = 0; i < count; ++i) {
            path.addCircle(fPts[i], 6);
            if (!fShowPoints && i < count - 1) {
                add_dashed_line(&path, fPts[i], fPts[i+1]);
            }
        }
        if (fShowHalves) {
            GPoint dst[5];
            GPath::ChopQuadAt(fPts, dst, fSubT);
            add_dashed_line(&path, dst[1], dst[3]);
        }
        canvas->drawPath(path, paint);

        this->Shape::drawShaderHilite(canvas);
    }

    GRect getRect() override {
        return GPath().moveTo(fPts[0]).quadTo(fPts[1], fPts[2]).bounds();
    }

    void setRect(const GRect& r) override {}
    GColor onGetColor() override { return fColor; }
    void onSetColor(const GColor& c) override { fColor = c; }

    GClick* findClick(GPoint p, GWindow* wind) override {
        if (GClick* click = Shape::findClick(p, wind)) {
            return click;
        }

        int index = -1;
        for (int i = 0; i < 3; ++i) {
            if (hit_test(p.x(), p.y(), fPts[i].x(), fPts[i].y())) {
                index = i;
            }
        }
        if (index >= 0) {
            return new GClick(p, [this, wind, index](GClick* click) {
                fPts[index] = click->curr();
                wind->requestDraw();
            });
        }
        return nullptr;
    }
private:
    GColor  fColor;
};

