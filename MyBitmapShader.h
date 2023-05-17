#include <stdio.h>
#include "GShader.h"
#include "GMatrix.h"
#include "GBitmap.h"
#include "GPixel.h"

class BitmapShader : public GShader {
public:
    BitmapShader(const GBitmap& bitmap, const GMatrix& localMatrix, GShader::TileMode tM) : fDevice(bitmap), fLM(localMatrix), tileMode(tM) {}

    bool isOpaque() override {
        return fDevice.isOpaque();
    }

    bool setContext(const GMatrix& ctm) override {
        inv = ctm * fLM;
        return inv.invert(&inv);
    }

    void shadeRow(int x, int y, int count, GPixel row[]) override {
        for (int i = 0; i < count; i++) {
            GPoint dst = {i + x + .5f, y + .5f};
            inv.mapPoints(&dst, &dst, 1);
            int c_i;
            int c_j;

            if (tileMode == GShader::kClamp)
            {
                c_i = std::min(fDevice.width() - 1, std::max(0, GFloorToInt(dst.x())));
                c_j = std::min(fDevice.height() - 1, std::max(0, GFloorToInt(dst.y())));
            }
            else if (tileMode == GShader::kRepeat)
            {
                dst.fX *= 1.f/(fDevice.width());
                dst.fY *= 1.f/(fDevice.height());
                dst.fX = dst.x() - GFloorToInt(dst.x());
                dst.fY = dst.y() - GFloorToInt(dst.y());
                dst.fX *= fDevice.width();
                dst.fY *= fDevice.height();
                c_i = GFloorToInt(dst.x());
                c_j = GFloorToInt(dst.y());
            }
            else if (tileMode == GShader::kMirror)
            {
                float fX = dst.fX / fDevice.width();
                float fY = dst.fY / fDevice.height();
                fX *= .5; fY *= .5;
                fX = fX - floorf(fX);
                fY = fY - floorf(fY);

                if (fX > .5) {fX = 1 - fX;}
                if (fY > .5) {fY = 1 - fY;}

                fX *= fDevice.width(); fX *= 2;
                fY *= fDevice.height(); fY *= 2;
                c_i = GFloorToInt(fX);
                c_j = GFloorToInt(fY);
            }
            row[i] = *(fDevice.getAddr(c_i, c_j));
        }
    }

private:
    const GBitmap fDevice;
    GMatrix fLM; // local Matrix
    GMatrix inv;
    GShader::TileMode tileMode;
};


std::unique_ptr<GShader> GCreateBitmapShader(const GBitmap& bm, const GMatrix& localMatrix, GShader::TileMode tileMode) {
    return std::unique_ptr<GShader>(new BitmapShader(bm, localMatrix, tileMode));
}