/**
*  Copyright 2015 Mike Reed
*/

static void to_quad(const GRect& r, GPoint quad[4]) {
    quad[0] = {r.left(),  r.top()};
    quad[1] = {r.right(), r.top()};
    quad[2] = {r.right(), r.bottom()};
    quad[3] = {r.left(),  r.bottom()};
}

class PolyRectsBench : public GBenchmark {
    enum { W = 200, H = 200 };
    const bool fForceOpaque;
public:
    PolyRectsBench(bool forceOpaque) : fForceOpaque(forceOpaque) {}

    const char* name() const override { return fForceOpaque ? "quads_opaque" : "quads_blend"; }
    GISize size() const override { return { W, H }; }
    void draw(GCanvas* canvas) override {
        const int N = 500;
        const GRect bounds = GRect::LTRB(-10, -10, W + 10, H + 10);
        GRandom rand;
        for (int i = 0; i < N; ++i) {
            GColor color = rand_color(rand, fForceOpaque);
            GPoint quad[4];
            to_quad(rand_rect(rand, bounds), quad);
            canvas->drawConvexPolygon(quad, 4, GPaint(color));
        }
    }
};

static void tesselate_circle(GPoint pts[], int count, float cx, float cy, float rad) {
    assert(count >= 3);
    for (int i = 0; i < count; ++i) {
        float angle = i * M_PI * 2 / count;
        pts[i] = {cos(angle) * rad + cx, sin(angle) * rad + cy};
    }
}

class CirclesBench : public GBenchmark {
    enum { W = 200, H = 200 };
    const bool fTiny;
public:
    CirclesBench(bool tiny) : fTiny(tiny) {}

    const char* name() const override { return fTiny ? "circles_tiny" : "circles_large"; }
    GISize size() const override { return { W, H }; }
    void draw(GCanvas* canvas) override {
        GPoint circle[100];
        tesselate_circle(circle, 100, 100, 100, fTiny ? 5 : 90);

        const int N = 500;
        GRandom rand;
        for (int i = 0; i < N; ++i) {
            canvas->drawConvexPolygon(circle, 100, GPaint(rand_color(rand, true)));
        }
    }
};

class ModesBench : public GBenchmark {
    enum { W = 200, H = 200 };
    const GColor fColor;
    const char* fName;
public:
    ModesBench(const GColor& c, const char* name) : fColor(c), fName(name) {}

    const char* name() const override { return fName; }
    GISize size() const override { return { W, H }; }
    void draw(GCanvas* canvas) override {
        const GRect r = GRect::WH(W, H);
        GPaint paint(fColor);
        GRandom rand;

        const bool vary = fColor.a > 0 && fColor.a < 1;
        const int N = 50;
        for (int m = 0; m < 12; ++m) {
            paint.setBlendMode(static_cast<GBlendMode>(m));
            for (int i = 0; i < N; ++i) {
                if (vary) {
                    paint.setAlpha(rand.nextF());
                }
                canvas->drawRect(r, paint);
            }
        }
    }
};

