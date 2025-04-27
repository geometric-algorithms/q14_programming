// halfedge_structs.h
#ifndef HALFEDGE_STRUCTS_H
#define HALFEDGE_STRUCTS_H

struct Vertex;
struct HalfEdge;
struct Face;

const double EPS = 0.0001;

struct Point {
    double x, y;
    Point(double x = 0, double y = 0) : x(x), y(y) {}

    // Equality comparator
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }

};

struct Line {
    double a, b, c; // ax + by + c = 0
    Point pi, pj;
    Line(Point p1, Point p2) {
        a = p2.y - p1.y;
        b = p1.x - p2.x;
        c = p2.x*p1.y - p1.x*p2.y;

        pi = p1, pj = p2;
    }

    Line (double a_, double b_, double c_) : a(a_), b(b_), c(c_) {}
    Line() : a(0), b(0), c(0) {}

    bool intersection(const Line& other, Point& result) const {
        const double det = a * other.b - other.a * b;
        if (det == 0) return false; // No intersection
        
        result.x = (b * other.c - other.b * c) / det;
        result.y = (other.a * c - a * other.c) / det;
        return true;
    }

};


// DCEL components
struct Vertex {
    Point pt;
    HalfEdge* inc_edge;
    Vertex(Point p) : pt(p), inc_edge(nullptr) {}
};


struct Face {
    HalfEdge* outer;
    Face() : outer(nullptr) {}
    Face(HalfEdge* outer_) : outer(outer_) {}
};

struct HalfEdge {
    Vertex* origin;
    HalfEdge* twin;
    HalfEdge* next;
    HalfEdge* prev;
    Face* face;
    HalfEdge(Vertex* v = nullptr) : origin(v), twin(nullptr), 
                                   next(nullptr), prev(nullptr),
                                   face(nullptr) {}
};


bool isEqual(double a, double b) {
    return std::abs(a - b) <= EPS;
}

bool isGeq(double a, double b) {
    return std::abs(a - b) <= EPS || a > b;
}

bool isLeq(double a, double b) {
    return std::abs(a - b) <= EPS || a < b;
}

#endif