#include <stdio.h>

std::string TestDrawQuad(GCanvas* canvas, GISize dim) {
    GPoint verts[] = {{10, 10}, {100, 10}, {100, 100}, {10, 100}};
    canvas->drawQuad(verts, nullptr, nullptr, 8, GPaint({1,0,0,.6}));
    return "something";
}

std::string TestDrawTri(GCanvas* canvas, GISize dim) {
    GPoint pts[] = {
        { 10, 10 }, {100, 10}, {100, 100},
    };
    GPoint p0 = {10,10}; GPoint p1 = {100, 100}; GColor clrs[] = {{1, 1, 1, 1}, {0, 0, 0, 1}};
    auto sh = GCreateLinearGradient(p0, p1, clrs, 2, GShader::TileMode::kClamp);

    GColor colors[] = {{1, 0, 0, 1}, {0, 1, 0, 1}, {0, 0, 1, 1}};
    const int indices[] = {0, 1, 2};
    // TriColorShader shader = TriColorShader(pts, colors[0], colors[1], colors[2]);
    // printMatrix(shader.getfLM());
    // GPoint pTri = {250, 400};
    // shader.setContext(GMatrix());
    // pTri = shader.getInv() * pTri;
    // printPoints(&pTri, 1);

    canvas->drawMesh(pts, colors, pts, 1, indices, GPaint(sh.get()));
    return "something";
}

std::string TestDrawDonut(GCanvas* canvas, GISize dim) {
    GPath path;
    path.addCircle({dim.width()/2.f, dim.height()/2.f}, .8 * dim.width()/2.f);
    path.addCircle({dim.width()/2.f, dim.height()/2.f}, .3 * dim.width()/2.f, GPath::Direction::kCCW_Direction);
    canvas->drawPath(path, GPaint(GColor::RGBA(.75, .7, .65, 1)));
    
    GPath path1;
    path1.addCircle({dim.width()/2.f, dim.height()/2.f}, .75 * dim.width()/2.f);
    path1.addCircle({dim.width()/2.f, dim.height()/2.f}, .25 * dim.width()/2.f, GPath::Direction::kCCW_Direction);
    canvas->drawPath(path1, GPaint(GColor::RGBA(.7, .4, .7, .7)));
    return "something";
}

std::string TestBitMapModes(GCanvas* canvas, GISize dim) {
    GBitmap bm;
    bm.readFromFile("apps/spock.png");
    GMatrix oWs = GMatrix::Translate(80, 80) * GMatrix::Scale(.1, .1);
    std::unique_ptr<GShader> bm_shader = GCreateBitmapShader(bm,  oWs, GShader::kRepeat);
    canvas->drawPaint(GPaint(bm_shader.get()));
    return "something";
}

std::string TestGradientModes(GCanvas* canvas, GISize dim) {
    auto sh = GCreateLinearGradient({100, 100}, {120, 120}, {1, 0, 0, 1}, {0, 1, 0, 1}, GShader::kClamp);
    canvas->drawPaint(GPaint(sh.get()));
    return "something";
}

std::string SunsetDrawing(GCanvas* canvas, GISize dim) {
    canvas->scale(1, 1);
    canvas->translate(0,0);
    // canvas->rotate(.5);
    canvas->drawPaint(GPaint(GColor::RGBA(.88, .73, .85, 1)).setBlendMode(GBlendMode::kSrc)); // sky
    GColor sky[3] = {{.44, .08, .48, .8}, {.9, .2, .3, .7}, {.78, .43, .33, .6}};
    std::unique_ptr<GShader> sh = GCreateLinearGradient({dim.fWidth /2, dim.fHeight * .1}, {dim.fWidth / 2, dim.fHeight / 2}, sky, 3);
    canvas->drawPaint(GPaint(GColor::RGBA(.88, .73, .85, 1)).setShader(sh.get())); // sky_colors

    const float WATER_LEVEL = dim.height() * .6;

    // sun
    GRect sunBody = GRect::LTRB(dim.width() * .35, dim.height() * .45, dim.width() * .65, WATER_LEVEL);
    float sunRadius = (sunBody.right() - sunBody.left()) / 2.f;
    float sunCenter = (sunBody.left() + sunBody.right())/2.f;
    GColor sunColor = GColor::RGBA(.95,.6, .02,.6);
    GPath sunBodyPath;
    sunBodyPath.addCircle({sunCenter, WATER_LEVEL}, sunRadius);
    canvas->drawPath(sunBodyPath, sunColor);
    
    sunColor.a = .5;
    GRect sunCore = GRect::LTRB(sunBody.fLeft += (sunBody.right() - sunBody.left()) * .1,
                                sunBody.fTop += (sunBody.bottom() - sunBody.top()) * .15,
                                sunBody.fRight -= (sunBody.right() - sunBody.left()) * .1,
                                sunBody.fBottom);
    GPath sunCorePath;
    float sunCoreRadius = (sunCore.right() - sunCore.left()) / 2.f;
    sunCorePath.addCircle({sunCenter, WATER_LEVEL}, sunCoreRadius);
    // sunCorePath.addCircle({sunCenter, WATER_LEVEL}, sunRadius, GPath::Direction::kCCW_Direction);
    canvas->drawPath(sunCorePath, sunColor);

    sunColor.a = .3;
    GPath sunCorePath2;
    float sunCoreRadius2 = sunCoreRadius * 1.35;
    sunCorePath2.addCircle({sunCenter, WATER_LEVEL}, sunCoreRadius2);
    sunCorePath.addCircle({sunCenter, WATER_LEVEL}, sunCoreRadius, GPath::Direction::kCCW_Direction);

    canvas->drawPath(sunCorePath2, sunColor);

    // water
    GRect water_rect  = GRect::LTRB(-50, WATER_LEVEL, dim.width() + 50, dim.height() + 50);
    std::unique_ptr<GShader> water_grad = GCreateLinearGradient({dim.fWidth /2, WATER_LEVEL}, {dim.fWidth / 2, dim.height()}, GColor::RGBA(.9, .2, .3, 0), GColor::RGBA(.78, .33, .33, .7));
    canvas->drawRect(water_rect,  GColor::RGBA(0, .35, .6, .8)); // water
    canvas->drawRect(water_rect, GPaint(water_grad.get()));

    
    // reflections v2
    float rl = 1.0f;
    int rg = rl * 5;
    float rm = 0;
    int g = rg;

    for (int j = WATER_LEVEL * dim.width(); j < (dim.height() * dim.width() + (dim.width() * 50)); j += g) {
        int left = j % dim.width();
        int right = left + rl;
        int top = j / dim.width();
        int bottom = top + 1;
        int center_x = (left + right) / 2;

        int distance_x  = center_x - sunCenter;
        if (distance_x < 0) {distance_x = distance_x * -1;}
        float R = distance_x < 40 ? sunColor.r : 1;
        float G = distance_x < 40 ? sunColor.g : 1;
        float B = distance_x < 40 ? sunColor.b : 1;
        canvas->drawRect(GRect::LTRB(left,top,right,bottom), GColor::RGBA(R,G,B,(float) j / (dim.height() * dim.width()) - .3));
        rl += .02f;
        rg = 7 * rl;
        g = rand() % (int) rg;
        rm += .01;
    }

    // mountains
    GPoint mtn_pts[] = {{dim.width() * .75f, WATER_LEVEL}, {dim.width() * .8f + 20, WATER_LEVEL}, {dim.width() * .77f + 10, WATER_LEVEL - 10}};
    GPoint mtn_pts2[] = {{dim.width() * .75f + 20, WATER_LEVEL}, {dim.width() * .8f + 40, WATER_LEVEL}, {dim.width() * .78f + 30, WATER_LEVEL - 10}};
    GPoint mtn_pts3[] = {{dim.width() * .75f + 15, WATER_LEVEL}, {dim.width() * .8f + 70, WATER_LEVEL}, {dim.width() * .78f + 40, WATER_LEVEL - 35}};
    GPoint mtn_cap_pts[] = {{dim.width() * .78f + 26, WATER_LEVEL - 20}, {dim.width() * .78f + 53, WATER_LEVEL - 23}, {dim.width() * .78f + 40, WATER_LEVEL - 35}};
    canvas->drawConvexPolygon(mtn_pts, 3, GPaint(GColor::RGBA(.17, .15, .11, .8)));
    canvas->drawConvexPolygon(mtn_pts2, 3, GPaint(GColor::RGBA(.17, .15, .11, .6)));
    canvas->drawConvexPolygon(mtn_pts3, 3, GPaint(GColor::RGBA(.17, .15, .11, .4)));
    canvas->drawConvexPolygon(mtn_cap_pts, 3, GPaint(GColor::RGBA(.9, .9, .9, .2)));

    // GBitmap bm;
    // bm.readFromFile("apps/oldwell.png");
    // GRect oW_rect = GRect::XYWH(200, 125, 15, 20);
    // GMatrix oWs = GMatrix::Translate(oW_rect.left(), oW_rect.top()) * GMatrix::Scale(.1, .1);
    // std::unique_ptr<GShader> bm_shader = GCreateBitmapShader(bm,  oWs);
    // canvas->drawRect(oW_rect, GPaint(bm_shader.get()));

    GPath path;
    GPath path1;
    float ocean_lvl = water_rect.top();
    path1.moveTo(30, ocean_lvl - 5).lineTo(10 + 30, ocean_lvl - 18).lineTo(15 + 30, ocean_lvl - 4). lineTo(20 + 30, ocean_lvl - 12).lineTo(24 + 30, ocean_lvl - 7).
    lineTo(28 + 30, ocean_lvl - 10).lineTo(30 + 30, ocean_lvl - 6).lineTo(33 + 30, ocean_lvl - 3).lineTo(38 + 35, ocean_lvl - 6).lineTo(44 + 35, ocean_lvl).lineTo(30, ocean_lvl);
    path.moveTo(0, ocean_lvl - 10).lineTo(7, ocean_lvl - 14).lineTo(15, ocean_lvl - 4). lineTo(20, ocean_lvl - 12).lineTo(24, ocean_lvl - 7).
    lineTo(28, ocean_lvl - 13).lineTo(30, ocean_lvl - 11).lineTo(33, ocean_lvl - 4).lineTo(38, ocean_lvl - 6).lineTo(44, ocean_lvl).lineTo(0, ocean_lvl);
    // .lineTo({rp.x() + 5, rp.y() + 80}).lineTo({rp.x() + 3, rp.y() + 100}).lineTo({rp.x() - 5, rp.y() + 120}).lineTo({rp.x() - 8, (float) dim.fHeight});
    canvas->drawPath(path1, GPaint(GColor::RGBA(.17, .15, .11, .3)));
    canvas->drawPath(path, GPaint(GColor::RGBA(.17, .15, .11, .7)));
    return "Sunset of the Scintillating Sea";
}