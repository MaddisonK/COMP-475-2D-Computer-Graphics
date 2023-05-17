#include <stdio.h>
#include "GShader.h"
#include "GMatrix.h"
#include "GBitmap.h"
#include "GPixel.h"

class TriProxyShader : public GShader {
public:
    TriProxyShader(GPoint triPts[], GPoint uvPts[], GShader* shader) {
        GPoint p0 = triPts[0];
        GPoint p1 = triPts[1];
        GPoint p2 = triPts[2];
        GPoint pU = p1 - p0;
        GPoint pV = p2 - p0;
        GMatrix P = GMatrix(pU.x(), pV.x(), p0.x(), pU.y(), pV.y(), p0.y());

        GPoint uv0 = uvPts[0];
        GPoint uv1 = uvPts[1];
        GPoint uv2 = uvPts[2];
        GPoint UV1 = uv1 - uv0;
        GPoint UV2 = uv2 - uv0;
        GMatrix T = GMatrix(UV1.x(), UV2.x(), uv0.x(), UV1.y(), UV2.y(), uv0.y());
        T.invert(&T);
        M = P * T;
        this->shader = shader;
    }

    bool isOpaque() override {
        return false;
    }

    bool setContext(const GMatrix& ctm) override {
        return shader->setContext(ctm * M);
    }

    void shadeRow(int x, int y, int count, GPixel row[]) override {
        shader->shadeRow(x, y, count, row);
    }
    
    GMatrix getM() {
        return M;
    }

private:
    GColor c0;
    GColor c1;
    GColor c2;
    GMatrix M;
    GShader* shader;
};


std::unique_ptr<GShader> GCreateTriProxyShader(GPoint triPts[], GPoint uvPts[], GShader* sh) {
    return std::unique_ptr<GShader>(new TriProxyShader(triPts, uvPts, sh));
}