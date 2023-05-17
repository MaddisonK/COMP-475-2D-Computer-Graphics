/**
 *  Copyright 2018 Mike Reed
 */

#include "GPath.h"

class OvalShape : public Shape {
    GRect fRect;
    GColor  fColor;
public:
    OvalShape() {
        fRect = GRect::LTRB(40, 70, 200, 200);
        fColor = { 1, 0, 0, 1 };
    }

    void onDraw(GCanvas* canvas, const GPaint& paint) override {
        GPath path;
        path.addCircle({0.5f, 0.5f}, 0.5f);
        path.transform(GMatrix::Translate(fRect.fLeft, fRect.fTop)
                     * GMatrix::Scale(fRect.width(), fRect.height()));
        canvas->drawPath(path, paint);
    }

    GRect getRect() override { return fRect; }

    void setRect(const GRect& r) override { fRect = r; }
    GColor onGetColor() override { return fColor; }
    void onSetColor(const GColor& c) override { fColor = c; }
};

