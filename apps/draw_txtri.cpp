/**
 *  Copyright 2021 Mike Reed
 */

#include "GPoint.h"
#include "GMatrix.h"
#include <vector>

extern bool gUseBilerpGShader;

class TxTriShape : public Shape {
    std::unique_ptr<GShader> fTexShader;
    GPoint                   fTex[3],
                             fPts[3];
    const GBitmap            fBM;
    int                      fTile = 0;

    void make_shader() {
        fTexShader = GCreateBitmapShader(fBM, GMatrix(), (GShader::TileMode)fTile);
    }

public:
    TxTriShape(const GBitmap& bm) : fBM(bm) {
        this->make_shader();
        fTex[0] = {20,   20};
        fTex[1] = {bm.width()/2,  60};
        fTex[2] = {30, bm.height()/2};

        GMatrix m = GMatrix::Translate(bm.width(), 50)
                  * GMatrix::Scale(2, 2);
        m.mapPoints(fPts, fTex, 3);
        printf("txtri\n");
    }

    bool handleSym(uint32_t sym) override {
        switch (sym) {
            case 't':
                fTile = (fTile + 1) % 3;
                this->make_shader();
                return true;
            case 'b':
                gUseBilerpGShader = !gUseBilerpGShader;
                return true;
            default:
                break;
        }
        return false;
    }

    void onDraw(GCanvas* canvas, const GPaint&) override {
        auto draw_tri = [](GCanvas* canvas, const GPoint pts[3], const GColor color) {
            GPaint paint(color);

            const float r = 3;
            for (int i = 0; i < 3; ++i) {
                canvas->drawRect(GRect::LTRB(pts[i].fX - r, pts[i].fY - r,
                                             pts[i].fX + r, pts[i].fY + r), paint);
                draw_line(canvas, pts[i], pts[(i + 1) % 3], color, 1.2f);
            }
        };

        GPaint paint(fTexShader.get());
        canvas->drawRect(GRect::WH(fBM.width(), fBM.height()), paint);

        const int indices[] = {0, 1, 2};
        canvas->drawMesh(fPts, nullptr, fTex, 1, indices, paint);

        draw_tri(canvas, fPts, {1,0,0,1});
        draw_tri(canvas, fTex, {0,0.75f,0,1});
    }

    GRect getRect() override {
        return GRect::WH(fBM.width(), fBM.height());
    }

    GColor onGetColor() override { return {0,0,0,1}; }
    void onSetColor(const GColor& c) override {}

    GClick* findClick(GPoint pt, GWindow* wind) override {
        auto hittest = [](GPoint hit, const GPoint pts[], int count) {
            const float tol = 4;
            for (int i = 0; i < count; ++i) {
                if ((pts[i] - hit).length() <= tol) {
                    return i;
                }
            }
            return -1;
        };

        int index = hittest(pt, fPts, 3);
        if (index >= 0) {
            return new GClick(pt, [this, wind, index](GClick* click) {
                fPts[index] = click->curr();
                wind->requestDraw();
            });
        }
        index = hittest(pt, fTex, 3);
        if (index >= 0) {
            return new GClick(pt, [this, wind, index](GClick* click) {
                fTex[index] = click->curr();
                wind->requestDraw();
            });
        }
        return nullptr;
    }
};
