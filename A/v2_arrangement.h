// arrangement.h
#ifndef ARRANGEMENT_H
#define ARRANGEMENT_H


#include <vector>
#include <set>
#include "v2_halfedge_structs.h"

// struct BoundingBox {
//     Point b1, b2, b3, b4;
//     Line l1, l2, l3, l4;

//     // make sure the all the lines intersect the left bounding line. No vertical lines in the inputs are allowed
//     BoundingBox(double inf) : b1(-inf, inf), b2(inf, inf), b3(inf, -inf), b4(-inf, -inf), 
//                               l1(1, 0, inf), l2(0, 1, -inf), l3(1, 0, -inf), l4(0, 1, inf) {}
// };


struct cmp1 {
    bool operator() (const HalfEdge* a, const HalfEdge* b) const {
        return (a->origin->pt).y < (b->origin->pt).y;
    }
};

struct cmp2 {
    bool operator() (const HalfEdge* a, const HalfEdge* b) const {
        return -(a->origin->pt).x < -(b->origin->pt).x;
    }
};

struct cmp3 {
    bool operator() (const HalfEdge* a, const HalfEdge* b) const {
        return -(a->origin->pt).y < -(b->origin->pt).y;
    }
};

struct cmp4 {
    bool operator() (const HalfEdge* a, const HalfEdge* b) const {
        return (a->origin->pt).x < (b->origin->pt).x;
    }
};

class Arrangement {
    std::vector<Vertex*> vertices;
    std::vector<HalfEdge*> edges;
    std::vector<Face*> faces;
    double bound_size;

    double bsz[5];
    Line boundary[4];

    std::set<HalfEdge*, cmp1> leftmost;
    std::set<HalfEdge*, cmp2> downmost;
    std::set<HalfEdge*, cmp3> rightmost;
    std::set<HalfEdge*, cmp4> upmost;

  public:
    
    Arrangement(double bsz1, double bsz2, double bsz3, double bsz4);

    bool getBoundaryIntersection(Line& l, Point& p);
    bool findIntersection(Line& l, HalfEdge* e, Point& p);
    HalfEdge* getLeftMostEdge(Line &l);
    HalfEdge* findExit(Line& l, HalfEdge* e);
    // void updateDCEL(Point& p, HalfEdge* e);         // p should lie on e
    void updateDCEL(Face* f, Vertex* v1, Vertex* v2, HalfEdge* e1, HalfEdge* e2, HalfEdge* pass1, HalfEdge* pass2);
    void insertLine(Line& l);       // call this function for all the lines in L to complete the incremental algorithm


    std::set<HalfEdge*, cmp1> get() {return leftmost;}
    std::vector<HalfEdge*> getEdges() {return edges;}
    std::vector<Face*> getFaces() {return faces;}
    std::vector<Vertex*> getVertices() {return vertices;}


    HalfEdge* getBoundaryEdge(Line &l);
    std::vector<Face*> zoneQuery(Line &l);

};


double abs(double x) {
    return x < 0 ? -x : x;
}

#endif  // ARRANGEMENT_H