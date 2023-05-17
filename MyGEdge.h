struct GEdge { // x = ym+b
    float m;
    float cX;
    int top, bottom;
    int w;

    static GEdge FromPoints(GPoint p1, GPoint p2) {
        int w;
        if (p1.y() > p2.y()) {
            std::swap(p1, p2);
            w = -1;
        } else {w = 1;}
        int top = GRoundToInt(p1.y());
        int bottom = GRoundToInt(p2.y());
        float m;
        if (top != bottom) {m = (p1.x() - p2.x()) / (p1.y() - p2.y());}
        float cX = p1.x() + m*(top - p1.y() + 0.5f); // computes x value at center of top pixel
        return {m, cX, top, bottom, w};
    }

    float eval(int y) const {
        return cX + m * (y - top);
    }

    bool active(int y) const {
        return (y >= top && y < bottom);
    }
};

std::vector<GEdge> GPoints2GEdges(const GPoint points[], int n) {
    std::vector<GEdge> edges(n);
    for (int i = 0; i < n - 1; i++) {
        edges[i] = GEdge::FromPoints(points[i], points[i+1]);
    }
    edges[n-1] = GEdge::FromPoints(points[0], points[n-1]);
    return edges;
}

std::vector<GEdge> GPoints2GEdges(std::vector<GPoint> points) {
    int n = points.size();
    std::vector<GEdge> edges(n);
    for (int i = 0; i < n - 1; i++) {
        edges[i] = GEdge::FromPoints(points[i], points[i+1]);
    }
    edges[n-1] = GEdge::FromPoints(points[0], points[n-1]);
    return edges;
}

bool cmpGEdges(const GEdge &a, const GEdge &b) {
    if (a.top != b.top) {
        return a.top < b.top;
    }
    if (a.cX != b.cX) {
        return a.cX < b.cX;
    }
    return a.m < b.m;
}

void printEdges(std::vector<GEdge> edges) {
    printf("Edges: (");
    for (GEdge e: edges) {
        if (e.top == e.bottom) {
            printf("{%d __ %d}), ", e.top, e.bottom);
        } else if (e.w == -1) {
            printf("{%f, %d} -> {%f, %d}), ", e.eval(e.top), e.top, e.eval(e.bottom), e.bottom);
        } else {
            printf("{%f, %d} -> {%f, %d}), ", e.eval(e.bottom), e.bottom, e.eval(e.top), e.top);
        }
        
    }
    printf("}\n\n");
}