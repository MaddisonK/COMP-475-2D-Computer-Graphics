/*
 *  Copyright 2016 Mike Reed
 */

#ifndef GPaint_DEFINED
#define GPaint_DEFINED

#include "GColor.h"
#include "GBlendMode.h"

class GShader;

class GPaint {
public:
    GPaint() {}
    GPaint(const GColor& c) : fColor(c) {}
    GPaint(GShader* s) : fShader(s) {}

    const GColor& getColor() const { return fColor; }
    GPaint& setColor(GColor c) { fColor = c; return *this; }
    GPaint& setRGBA(float r, float g, float b, float a) {
        return this->setColor(GColor::RGBA(r, g, b, a));
    }

    float   getAlpha() const { return fColor.a; }
    GPaint& setAlpha(float alpha) {
        fColor.a = alpha;
        return *this;
    }

    GBlendMode getBlendMode() const { return fMode; }
    GPaint&    setBlendMode(GBlendMode m) { fMode = m; return *this; }

    GShader* getShader() const { return fShader; }
    GPaint&  setShader(GShader* s) { fShader = s; return *this; }

private:
    GColor      fColor = {0, 0, 0, 1};
    GShader*    fShader = nullptr;
    GBlendMode  fMode = GBlendMode::kSrcOver;
};

#endif
