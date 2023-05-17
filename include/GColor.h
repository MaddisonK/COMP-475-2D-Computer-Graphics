/**
 *  Copyright 2015 Mike Reed
 */

#ifndef GColor_DEFINED
#define GColor_DEFINED

#include "GMath.h"

/**
 *  Defines an un-premultiplied color, where alpha, red, green, blue are all independent of
 *  each other. Legal values for each component are [0.0 .. 1.0].
*/
struct GColor {
    float r, g, b, a;

    static GColor RGBA(float r, float g, float b, float a) {
        return {r, g, b, a};
    }

    static GColor RGB(float r, float g, float b) {
        return {r, g, b, 1};
    }

    GColor pinToUnit() const {
        return RGBA(GPinToUnit(r), GPinToUnit(g), GPinToUnit(b), GPinToUnit(a));
    }

    bool operator==(const GColor& c) const {
        return r == c.r &&
               g == c.g &&
               b == c.b &&
               a == c.a;
    }
    bool operator!=(const GColor& c) const { return !(*this == c); }

    GColor operator-() const { return {-r, -g, -b, -a}; }
    GColor operator+(const GColor& c) const { return { r + c.r, g + c.g, b + c.b, a + c.a }; }
    GColor operator-(const GColor& c) const { return { r - c.r, g - c.g, b - c.b, a - c.a }; }
    GColor operator*(const GColor& c) const { return { r * c.r, g * c.g, b * c.b, a * c.a }; }

    friend GColor operator*(const GColor& c, float s) { return { c.r*s, c.g*s, c.b*s, c.a*s }; }
    friend GColor operator*(float s, const GColor& c) { return c*s; }

    GColor& operator+=(const GColor& c) { *this = *this + c; return *this; }
    GColor& operator-=(const GColor& c) { *this = *this - c; return *this; }
    GColor& operator*=(const GColor& c) { *this = *this * c; return *this; }
};

#endif
