#include <stdio.h>
#include "GShader.h"
#include "GMatrix.h"
#include "GPixel.h"

class TriColorShader : public GShader {
public:
    TriColorShader(GPoint triPts[], GColor c0, GColor c1, GColor c2) {
        GPoint p0 = triPts[0];
        GPoint p1 = triPts[1];
        GPoint p2 = triPts[2];
        this->c0 = c0;
        this->c1 = c1;
        this->c2 = c2;
        GPoint pU = p1 - p0;
        GPoint pV = p2 - p0;
        
        this->fLM = GMatrix(pU.x(), pV.x(), p0.x(), pU.y(), pV.y(), p0.y());
    }

    bool isOpaque() override {
        return false;
    }

    bool setContext(const GMatrix& ctm) override {
        inv = ctm * fLM;
        return inv.invert(&inv);
    }

    void shadeRow(int x, int y, int count, GPixel row[]) override {
        for (int i = 0; i < count; i++) {
            GPoint dst = {i + x + .5f, y + .5f};
            inv.mapPoints(&dst, &dst, 1);
            GColor triColor = dst.x()*(c1 - c0) + dst.y()*(c2 - c0) + c0; // TODO precompute color differences to speed up
            row[i] = GColor2GPixel(triColor);
        }
    }
    
    GMatrix getfLM() {
        return fLM;
    }

    GMatrix getInv() {
        return inv;
    }

private:
    GColor c0;
    GColor c1;
    GColor c2;
    GMatrix fLM;
    GMatrix inv;
};


std::unique_ptr<GShader> GCreateTriColorShader(GPoint triPts[], GColor c0, GColor c1, GColor c2) {
    return std::unique_ptr<GShader>(new TriColorShader(triPts, c0, c1, c2));
}