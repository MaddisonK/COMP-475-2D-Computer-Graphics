#include <stdio.h>
#include "GShader.h"
#include "GMatrix.h"
#include "GPixel.h"

class ComposeShader : public GShader {
public:
    ComposeShader(GShader* shader0, GShader* shader1) {
        this->shader0 = shader0;
        this->shader1 = shader1;
    }

    bool isOpaque() override {
        return false;
    }

    bool setContext(const GMatrix& ctm) override {
        return (shader0->setContext(ctm) && shader1->setContext(ctm));
    }

    void shadeRow(int x, int y, int count, GPixel row[]) override {
        GPixel row0[count];
        GPixel row1[count];
        shader0->shadeRow(x, y, count, row0);
        shader1->shadeRow(x, y, count, row1);
        for (int i = 0; i < count; i++) {
            row[i] = pixelMul(row1[i], row0[i]);
        }
    }

private:
    GShader* shader0;
    GShader* shader1;

    GPixel pixelMul(GPixel pix0, GPixel pix1) {
        unsigned int a = divideBy255(GPixel_GetA(pix0) * GPixel_GetA(pix1));
        unsigned int r = divideBy255(GPixel_GetR(pix0) * GPixel_GetR(pix1));
        unsigned int g = divideBy255(GPixel_GetG(pix0) * GPixel_GetG(pix1));
        unsigned int b = divideBy255(GPixel_GetB(pix0) * GPixel_GetB(pix1));
        return GPixel_PackARGB(a, r, g, b);
    }
};


std::unique_ptr<GShader> GCreateComposeShader(GShader* shader0, GShader* shader1) {
    return std::unique_ptr<GShader>(new ComposeShader(shader0, shader1));
}