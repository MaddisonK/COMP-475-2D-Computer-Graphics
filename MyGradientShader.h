#include <stdio.h>
#include "GShader.h"
#include "GMatrix.h"
#include "GBitmap.h"
#include "GPixel.h"

class GradientShader : public GShader {
public:
    GradientShader(GPoint p0, GPoint p1, const GColor colors[], int count, GShader::TileMode tM) {
        for (int i = 0; i < count; i++) {
            this->colors.push_back(colors[i]);
        }
        this->n = count;
        GPoint pU = p1 - p0;
        this->fLM = GMatrix(pU.x(), -pU.y(), p0.x(), pU.y(), pU.x(), p0.y());
        
        // pre-compute diffs
        for (int i = 0; i < count - 1; i++) {
            diffs.push_back(colors[i + 1] - colors[i]);
        }
        this->tileMode = tM;
    }

    bool isOpaque() override {
        for (int i = 0; i < n - 1; i++) {
            if (colors[i].a != 1) {
                return false;
            }
        }
        return true;
    }

    bool setContext(const GMatrix& ctm) override {
        inv = ctm * fLM;
        return inv.invert(&inv);
    }

    void shadeRow(int x, int y, int count, GPixel row[]) override {
        for (int i = 0; i < count; i++) {
            GPoint dst = {i + x + .5f, y + .5f};
            inv.mapPoints(&dst, &dst, 1);
            float fX = dst.x();
            if (n == 1) {row[i] = GColor2GPixel(colors.front());continue;}
            if (tileMode == GShader::kClamp)
            {
                if (dst.x() <= 0) {row[i] = GColor2GPixel(colors.front());continue;}
                if (dst.x() >= 1) {row[i] = GColor2GPixel(colors.back());continue;}
            }
            else if (tileMode == GShader::kRepeat)
            {
                fX = dst.x() - floor(dst.x());
                if (fX < 0) {fX+=1;}
            }
            else if (tileMode == GShader::kMirror)
            {
                fX = dst.x() * .5;
                fX = fX - floor(fX);
                if (fX > .5) {fX = 1 - fX;}
                fX *=2;
            }
            float c_x = fX * (n-1);
            int c_i = GFloorToInt(c_x);
            float w = c_x - c_i;
            GColor cI = lerpColors(c_i, w);
            row[i] = GColor2GPixel(cI);
        }
    }
    
    GMatrix getfLM() {
        return fLM;
    }

private:
    std::vector<GColor> colors;
    std::vector<GColor> diffs;
    int n;
    GMatrix fLM; // local Matrix
    GMatrix inv;
    GShader::TileMode tileMode;

    GColor lerpColors(int c_i, float w) {
        // c0 + w(c1 - c0); or c0 + w * deltas[index] (pre-compute the differences and store in class field)
        float r = colors[c_i].r + w * (diffs[c_i].r);
        float g = colors[c_i].g + w * (diffs[c_i].g);
        float b = colors[c_i].b + w * (diffs[c_i].b);
        float a = colors[c_i].a + w * (diffs[c_i].a);
        return GColor::RGBA(r, g, b, a);
    }
};


std::unique_ptr<GShader> GCreateLinearGradient(GPoint p0, GPoint p1, const GColor colors[], int count,
                                               GShader::TileMode tileMode) {
    return std::unique_ptr<GShader>(new GradientShader(p0, p1, colors, count, tileMode));
}