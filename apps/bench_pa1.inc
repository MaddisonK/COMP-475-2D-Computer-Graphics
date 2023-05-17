/**
*  Copyright 2015 Mike Reed
*/

static GRect rand_rect(GRandom& rand, const GRect& bounds) {
    const float x = bounds.width();
    const float y = bounds.height();
    float tmp[4] {
        rand.nextF() * x - bounds.left(), rand.nextF() * y - bounds.top(),
        rand.nextF() * x - bounds.left(), rand.nextF() * y - bounds.top()
    };
    return GRect::XYWH(std::min(tmp[0], tmp[2]), std::min(tmp[1], tmp[3]),
                       std::max(tmp[0], tmp[2]), std::max(tmp[1], tmp[3]));
}

class RectsBench : public GBenchmark {
    enum { W = 200, H = 200 };
    const bool fForceOpaque;
public:
    RectsBench(bool forceOpaque) : fForceOpaque(forceOpaque) {}

    const char* name() const override { return fForceOpaque ? "rects_opaque" : "rects_blend"; }
    GISize size() const override { return { W, H }; }
    void draw(GCanvas* canvas) override {
        const int N = 500;
        const GRect bounds = GRect::LTRB(-10, -10, W + 10, H + 10);
        GRandom rand;
        for (int i = 0; i < N; ++i) {
            GColor color = rand_color(rand, fForceOpaque);
            GRect rect = rand_rect(rand, bounds);
            canvas->fillRect(rect, color);
        }
    }
};

class SingleRectBench : public GBenchmark {
    const GISize    fSize;
    const GRect     fRect;
    const char*     fName;
public:
    SingleRectBench(GISize size, GRect r, const char* name) : fSize(size), fRect(r), fName(name) {}

    const char* name() const override { return fName; }
    GISize size() const override { return fSize; }
    void draw(GCanvas* canvas) override {
        const int N = 10000;
        GRandom rand;
        for (int i = 0; i < N; ++i) {
            GColor color = rand_color(rand);
            canvas->fillRect(fRect, color);
        }
    }
};
