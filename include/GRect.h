/**
 *  Copyright 2015 Mike Reed
 */

#ifndef GRect_DEFINED
#define GRect_DEFINED

#include "GMath.h"

class GIRect {
public:
    int32_t fLeft, fTop, fRight, fBottom;

    int32_t left() const { return fLeft; }
    int32_t top() const { return fTop; }
    int32_t right() const { return fRight; }
    int32_t bottom() const { return fBottom; }

    int32_t x() const { return fLeft; }
    int32_t y() const { return fTop; }
    int32_t width() const { return fRight - fLeft; }
    int32_t height() const { return fBottom - fTop; }

    static GIRect LTRB(int32_t l, int32_t t, int32_t r, int32_t b) {
        return {l, t, r, b};
    }
    
    static GIRect XYWH(int32_t x, int32_t y, int32_t w, int32_t h) {
        return {x, y, x + w, y + h};
    }
    
    static GIRect WH(int32_t w, int32_t h) {
        return {0, 0, w, h};
    }

    GIRect makeOffset(int dx, int dy) const {
        return {fLeft + dx, fTop + dy, fRight + dx, fBottom + dy};
    }
    
    bool isEmpty() const { return fLeft >= fRight || fTop >= fBottom; }
    
    operator bool() const { return !this->isEmpty(); }
};

class GRect {
public:
    float fLeft, fTop, fRight, fBottom;

    float left() const { return fLeft; }
    float top() const { return fTop; }
    float right() const { return fRight; }
    float bottom() const { return fBottom; }

    float x() const { return fLeft; }
    float y() const { return fTop; }
    float width() const { return fRight - fLeft; }
    float height() const { return fBottom - fTop; }

    static GRect LTRB(float l, float t, float r, float b) {
        return {l, t, r, b};
    }
    
    static GRect XYWH(float x, float y, float w, float h) {
        return {x, y, x + w, y + h};
    }
    
    static GRect WH(float w, float h) {
        return {0, 0, w, h};
    }
    
    GRect makeOffset(float dx, float dy) const {
        return LTRB(fLeft + dx, fTop + dy, fRight + dx, fBottom + dy);
    }
    
    bool isEmpty() const { return fLeft >= fRight || fTop >= fBottom; }
    
    operator bool() const { return !this->isEmpty(); }

    GIRect round() const {
        return GIRect::LTRB(GRoundToInt(fLeft), GRoundToInt(fTop),
                            GRoundToInt(fRight), GRoundToInt(fBottom));
    }

    GIRect roundOut() const {
        return GIRect::LTRB(GFloorToInt(fLeft), GFloorToInt(fTop),
                            GCeilToInt(fRight), GCeilToInt(fBottom));
    }
    
    static GRect Make(const GIRect& r) {
        return {
            (float)r.fLeft, (float)r.fTop, (float)r.fRight, (float)r.fBottom
        };
    }
};

#endif
