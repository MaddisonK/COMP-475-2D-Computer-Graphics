/**
 *  Copyright 2018 Mike Reed
 */

#include "draw_curve.h"

template <typename DRAW> void spin(GCanvas* canvas, int N, DRAW draw) {
    for (int i = 0; i < N; ++i) {
        canvas->save();
        canvas->rotate(2 * M_PI * i / N);
        draw(canvas);
        canvas->restore();
    }
}

static void show_spiro(GCanvas* canvas, const GPoint pts[4]) {
    GRandom rand;
    GPath path;
    path.moveTo(pts[0]).cubicTo(pts[1], pts[2], pts[3]);

    canvas->save();
    canvas->scale(0.5f, 0.5f);
    canvas->translate(900, 700);
    spin(canvas, 9, [&path, &rand](GCanvas* canvas) {
        canvas->drawPath(path, GPaint({1, rand.nextF(), rand.nextF(), rand.nextF()}));
    });
    canvas->restore();
}

class CubicShape : public CurveShape {
    GPoint fPts[4];
public:
    CubicShape() {
        fPts[0] = { 20, 100 };
        fPts[1] = { 100, 20 };
        fPts[2] = { 160, 120 };
        fPts[3] = { 250, 50 };
        fColor = { 1, 0, 0, 1 };
    }

    void onDraw(GCanvas* canvas, const GPaint& paint) override {
        if (fShowPoints) {
            this->drawPoints(canvas, fPts, 4);
            return;
        }
        GPath path;
        path.moveTo(fPts[0]).cubicTo(fPts[1], fPts[2], fPts[3]);
        canvas->drawPath(path, paint);

        if (false) { show_spiro(canvas, fPts); }

        if (fShowHalves) {
            GPoint dst[7];
            GPath::ChopCubicAt(fPts, dst, fSubT);

            if (false) {
            path.reset().moveTo(dst[0]).cubicTo(dst[1], dst[2], dst[3]);
            canvas->drawPath(path, GPaint({0.5, 0, 1, 0}));

            path.reset().moveTo(dst[3]).cubicTo(dst[4], dst[5], dst[6]);
            canvas->drawPath(path, GPaint({0.5, 0, 0, 1}));
            }

            path.reset();
            for (int i = 1; i <= 5; ++i) {
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
            GPoint dst[7];
            GPath::ChopCubicAt(fPts, dst, fSubT);
            add_dashed_line(&path, dst[1], dst[2]);
            add_dashed_line(&path, dst[2], dst[4]);
            add_dashed_line(&path, dst[4], dst[5]);
        }
        canvas->drawPath(path, paint);

        this->Shape::drawShaderHilite(canvas);
    }

    GRect getRect() override {
        return GPath().moveTo(fPts[0]).cubicTo(fPts[1], fPts[2], fPts[3]).bounds();
    }

    void setRect(const GRect& r) override {}
    GColor onGetColor() override { return fColor; }
    void onSetColor(const GColor& c) override { fColor = c; }

    GClick* findClick(GPoint p, GWindow* wind) override {
        if (GClick* click = Shape::findClick(p, wind)) {
            return click;
        }

        int index = -1;
        for (int i = 0; i < 4; ++i) {
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

