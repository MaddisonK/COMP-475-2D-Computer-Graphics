/**
 *  Copyright 2021 Mike Reed
 */

#include "GCanvas.h"
#include "GBitmap.h"
#include "GColor.h"
#include "GPoint.h"
#include "GRect.h"
#include "tests.h"

static void force_fill_pixels(GBitmap& bm, GPixel pixel) {
    visit_pixels(bm, [pixel](int x, int y, GPixel* p) {
        *p = pixel;
    });
}

static bool expect_pixels_value(const GBitmap& bm, GPixel pixel) {
    bool success = true;
    visit_pixels(bm, [&](int x, int y, GPixel* p) {
        if (*p != pixel) {
            success = false;
        }
    });
    return success;
}

static void test_clear(GTestStats* stats) {
    const struct {
        GColor color;
        GPixel pixel;
    } rec[] = {
        { {1,1,1,1}, GPixel_PackARGB(0xFF, 0xFF, 0xFF, 0xFF) },
        { {0,0,0,1}, GPixel_PackARGB(0xFF, 0, 0, 0) },
        { {1,0,0,1}, GPixel_PackARGB(0xFF, 0xFF, 0, 0) },
        { {0,1,0,1}, GPixel_PackARGB(0xFF, 0, 0xFF, 0) },
        { {0,0,1,1}, GPixel_PackARGB(0xFF, 0, 0, 0xFF) },
        { {0,0,0,0}, GPixel_PackARGB(0, 0, 0, 0) },
        { {1,1,1,0}, GPixel_PackARGB(0, 0, 0, 0) },
    };

    for (const auto& r : rec) {
        GBitmap bm;
        bm.alloc(3, 3);
        auto canvas = GCreateCanvas(bm);
        force_fill_pixels(bm, 0x12345678);  // init with garbage
        canvas->clear(r.color);
        EXPECT_TRUE(stats, expect_pixels_value(bm, r.pixel));
    }
}

static void test_rect_nodraw(GTestStats* stats) {
    const GRect rects[] = {
        {0, 0, 0, 0},
        // doesn't cross a pixel center
        {0.51f, 0, 1.49f, 3},
        {0, 0.51f, 3, 1.49f},
        // clipped out
        {-5, 0, 0, 3},
        {3, 0, 5, 3},
        {0, 3, 3, 5},
        {0 -3, 3, 0},
    };
    
    const GColor color = {1,1,1,1};
    for (const auto& r : rects) {
        GBitmap bm;
        bm.alloc(3, 3);
        auto canvas = GCreateCanvas(bm);
        canvas->fillRect(r, color);
        EXPECT_TRUE(stats, expect_pixels_value(bm, 0));
    }
}
