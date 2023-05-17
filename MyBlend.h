#include "GPixel.h"
#include "GBlendMode.h"

typedef GPixel (*BlendFunc)(GPixel, GPixel);

GPixel clear(GPixel src, GPixel dst);
GPixel Src(GPixel src, GPixel dst);
GPixel Dst(GPixel src, GPixel dst);
GPixel Srcover(GPixel src, GPixel dst);
GPixel Dstover(GPixel src, GPixel dst);
GPixel Srcin(GPixel src, GPixel dst);
GPixel Dstin(GPixel src, GPixel dst);
GPixel Srcout(GPixel src, GPixel dst);
GPixel Dstout(GPixel src, GPixel dst);
GPixel Srcatop(GPixel src, GPixel dst);
GPixel Dstatop(GPixel src, GPixel dst);
GPixel Kxor(GPixel src, GPixel dst);

BlendFunc getBlendFunc(int Sa, GBlendMode mode) {
    switch (mode) {
        case GBlendMode::kClear: return clear;
        case GBlendMode::kSrc: return Src;
        case GBlendMode::kDst: return Dst;
        case GBlendMode::kSrcOver: 
        switch (Sa) {
            case (0): return Dst;
            case (255): return Src;
            default: return Srcover;
        }
        case GBlendMode::kDstOver: 
        switch (Sa) {
            case (0): return Dst;
            default: return Dstover;
        }
        case GBlendMode::kSrcIn: 
        switch (Sa) {
            case (0): return clear;
            default: return Srcin;
        }
        case GBlendMode::kDstIn: 
        switch(Sa) {
            case (0): return clear;
            case (255): return Dst;
            default: return Dstin;
        }
        case GBlendMode::kSrcOut: 
        switch (Sa) {
            case (0): return clear;
            default: return Srcout;
        }
        case GBlendMode::kDstOut:
        switch (Sa) {
            case (0): return Dst;
            case (255): return clear;
            default: return Dstout;
        }
        case GBlendMode::kSrcATop:
        switch (Sa) {
            case (0): return Dst;
            case (255): return Srcin;
            default: return Srcatop;
        }
        case GBlendMode::kDstATop:
        switch (Sa) {
            case (0): return clear;
            case (255): return Dstover;
            default: return Dstatop;
        }
        case GBlendMode::kXor:
        switch (Sa) {
            case (0): return Dst;
            case (255): return Srcout;
            default: return Kxor;
        }
        default: return Srcover;
    }
}

GPixel clear(GPixel src, GPixel dst) {
    return GPixel_PackARGB(0, 0, 0, 0);
}

GPixel Src(GPixel src, GPixel dst) {
    return src;
}

GPixel Dst(GPixel src, GPixel dst) {
    return dst;
}

GPixel Srcover(GPixel src, GPixel dst) {
    const int Sa = GPixel_GetA(src);
    // printf("src sa: %d, r: %d, g: %d, b: %d\n",  GPixel_GetA(src), GPixel_GetR(src), GPixel_GetG(src), GPixel_GetB(src));
    // printf("dst sa: %d, r: %d, g: %d, b: %d\n",  GPixel_GetA(dst), GPixel_GetR(dst), GPixel_GetG(dst), GPixel_GetB(dst));
    int a = GPixel_GetA(src) + divideBy255((255 - Sa) * GPixel_GetA(dst));
    int r = GPixel_GetR(src) + divideBy255((255 - Sa) * GPixel_GetR(dst));
    int g = GPixel_GetG(src) + divideBy255((255 - Sa) * GPixel_GetG(dst));
    int b = GPixel_GetB(src) + divideBy255((255 - Sa) * GPixel_GetB(dst));
    // printf("a: %d, r: %d, g: %d, b: %d\n", a, r, g, b);
    return GPixel_PackARGB(a, r, g, b);
}

GPixel Dstover(GPixel src, GPixel dst) {
    const int Da = GPixel_GetA(dst);

    int a = GPixel_GetA(dst) + divideBy255((255 - Da) * GPixel_GetA(src));
    int r = GPixel_GetR(dst) + divideBy255((255 - Da) * GPixel_GetR(src));
    int g = GPixel_GetG(dst) + divideBy255((255 - Da) * GPixel_GetG(src));
    int b = GPixel_GetB(dst) + divideBy255((255 - Da) * GPixel_GetB(src));

    return GPixel_PackARGB(a, r, g, b);
}

GPixel Srcin(GPixel src, GPixel dst) {
    const int Da = GPixel_GetA(dst);

    int a = divideBy255(GPixel_GetA(src) * Da);
    int r = divideBy255(GPixel_GetR(src) * Da);
    int g = divideBy255(GPixel_GetG(src) * Da);
    int b = divideBy255(GPixel_GetB(src) * Da);

    return GPixel_PackARGB(a, r, g, b);
}

GPixel Dstin(GPixel src, GPixel dst) {
    const int Sa = GPixel_GetA(src);

    int a = divideBy255(GPixel_GetA(dst) * Sa);
    int r = divideBy255(GPixel_GetR(dst) * Sa);
    int g = divideBy255(GPixel_GetG(dst) * Sa);
    int b = divideBy255(GPixel_GetB(dst) * Sa);

    return GPixel_PackARGB(a, r, g, b);
}

GPixel Srcout(GPixel src, GPixel dst) {
    const int Da = GPixel_GetA(dst);

    int a = divideBy255((255 - Da) * GPixel_GetA(src));
    int r = divideBy255((255 - Da) * GPixel_GetR(src));
    int g = divideBy255((255 - Da) * GPixel_GetG(src));
    int b = divideBy255((255 - Da) * GPixel_GetB(src));

    return GPixel_PackARGB(a, r, g, b);
}

GPixel Dstout(GPixel src, GPixel dst) {
    const int Sa = GPixel_GetA(src);

    int a = divideBy255((255 - Sa) * GPixel_GetA(dst));
    int r = divideBy255((255 - Sa) * GPixel_GetR(dst));
    int g = divideBy255((255 - Sa) * GPixel_GetG(dst));
    int b = divideBy255((255 - Sa) * GPixel_GetB(dst));

    return GPixel_PackARGB(a, r, g, b);
}

GPixel Srcatop(GPixel src, GPixel dst) {
    const int Sa = GPixel_GetA(src);
    const int Da = GPixel_GetA(dst);

    int a = divideBy255(GPixel_GetA(src) * Da) + divideBy255((255 - Sa) * GPixel_GetA(dst));
    int r = divideBy255(GPixel_GetR(src) * Da) + divideBy255((255 - Sa) * GPixel_GetR(dst));
    int g = divideBy255(GPixel_GetG(src) * Da) + divideBy255((255 - Sa) * GPixel_GetG(dst));
    int b = divideBy255(GPixel_GetB(src) * Da) + divideBy255((255 - Sa) * GPixel_GetB(dst));

    return GPixel_PackARGB(a, r, g, b);
}

GPixel Dstatop(GPixel src, GPixel dst) {
    const int Sa = GPixel_GetA(src);
    const int Da = GPixel_GetA(dst);

    int a = divideBy255(GPixel_GetA(dst) * Sa) + divideBy255((255 - Da) * GPixel_GetA(src));
    int r = divideBy255(GPixel_GetR(dst) * Sa) + divideBy255((255 - Da) * GPixel_GetR(src));
    int g = divideBy255(GPixel_GetG(dst) * Sa) + divideBy255((255 - Da) * GPixel_GetG(src));
    int b = divideBy255(GPixel_GetB(dst) * Sa) + divideBy255((255 - Da) * GPixel_GetB(src));

    return GPixel_PackARGB(a, r, g, b);
}

GPixel Kxor(GPixel src, GPixel dst) {
    const int Sa = GPixel_GetA(src);
    const int Da = GPixel_GetA(dst);

    int a = divideBy255((255 - Sa) * GPixel_GetA(dst)) + divideBy255((255 - Da) * GPixel_GetA(src));
    int r = divideBy255((255 - Sa) * GPixel_GetR(dst)) + divideBy255((255 - Da) * GPixel_GetR(src));
    int g = divideBy255((255 - Sa) * GPixel_GetG(dst)) + divideBy255((255 - Da) * GPixel_GetG(src));
    int b = divideBy255((255 - Sa) * GPixel_GetB(dst)) + divideBy255((255 - Da) * GPixel_GetB(src));

    return GPixel_PackARGB(a, r, g, b);
}