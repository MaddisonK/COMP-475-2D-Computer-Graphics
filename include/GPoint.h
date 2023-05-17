/**
 *  Copyright 2015 Mike Reed
 */

#ifndef GPoint_DEFINED
#define GPoint_DEFINED

#include "GMath.h"

struct GPoint {
    float fX;
    float fY;
    
    float x() const { return fX; }
    float y() const { return fY; }
    float length() const { return sqrtf(fX*fX + fY*fY); }
    
    bool operator==(GPoint p) const {
        return fX == p.fX && fY == p.fY;
    }
    bool operator!=(GPoint p) const { return !(*this == p); }
    
    GPoint operator+(GPoint v) const {
        return { fX + v.fX, fY + v.fY };
    }
    GPoint operator-(GPoint v) const {
        return { fX - v.fX, fY - v.fY };
    }
    
    friend GPoint operator*(GPoint v, float s) {
        return { v.fX * s, v.fY * s };
    }
    friend GPoint operator*(float s, GPoint v) {
        return { v.fX * s, v.fY * s };
    }

    GPoint& operator+=(GPoint o) {
        *this = *this + o;
        return *this;
    }
};

using GVector = GPoint;

template <typename T> class GSize {
public:
    T fWidth, fHeight;
    
    T width() { return fWidth; }
    T height() { return fHeight; }
};
typedef GSize<int> GISize;

#endif
