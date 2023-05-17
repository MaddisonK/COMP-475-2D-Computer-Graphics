/**
 *  Copyright 2015 Mike Reed
 */

#include "GCanvas.h"
#include "GBitmap.h"
#include "GColor.h"
#include "GPoint.h"
#include "GRect.h"
#include "tests.h"

class GSurface {
public:
    GSurface(int width, int height) {
        fBitmap.alloc(width, height);
        fCanvas = GCreateCanvas(fBitmap);
    }
    ~GSurface() {
        free(fBitmap.pixels());
    }

    GCanvas* canvas() const { return fCanvas.get(); }
    const GBitmap& bitmap() const { return fBitmap; }

private:
    std::unique_ptr<GCanvas>    fCanvas;
    GBitmap                     fBitmap;
};

///////////////////////////////////////////////////////////////////////////////////////////////////

#include "tests_pa3.cpp"
#include "tests_pa4.cpp"
#include "tests_pa5.cpp"

const GTestRec gTestRecs[] = {
    { test_matrix,      "matrix_setters"    },
    { test_matrix_inv,  "matrix_inv"        },
    { test_matrix_map,  "matrix_map"        },

    { test_path,        "path",             },
    { test_path_rect,   "path_rect",        },
    { test_path_poly,   "test_path_poly",   },
    { test_path_transform, "path_transform" },

    { test_edger_quads, "test_edger_quads"  },
    { test_path_circle, "test_path_circle"  },
    { test_path_transform2, "test_path_transform2" },
    { test_path_chop_quad,   "path_chop_quad"    },
    { test_path_chop_cubic,   "path_chop_cubic"    },

    { nullptr, nullptr },
};

bool gTestSuite_Verbose;
bool gTestSuite_CrashOnFailure;
