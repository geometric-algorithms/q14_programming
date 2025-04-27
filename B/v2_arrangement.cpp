// arrangement.cpp
#include "v2_arrangement.h"




Arrangement::Arrangement(double sz1, double sz2, double sz3, double sz4) {

    // initialize the bound_size
    bound_size = abs(sz1);
    bsz[0] = bound_size, bsz[1] = sz1, bsz[2] = sz2, bsz[3] = sz3, bsz[4] = sz4;
    Point p1(sz1, sz4), p2(sz1, sz2), p3(sz3, sz2), p4(sz3, sz4);
    boundary[0] = Line(p1, p2), boundary[1] = Line(p2, p3), boundary[2] = Line(p3, p4), boundary[3] = Line(p4, p1);


    // create the bounding box
    Vertex      *b1 = new Vertex(Point(sz1, sz4)),
                *b2 = new Vertex(Point(sz1, sz2)),
                *b3 = new Vertex(Point(sz3, sz2)),
                *b4 = new Vertex(Point(sz3, sz4));

    HalfEdge    *e1 = new HalfEdge(b1),
                *e2 = new HalfEdge(b2),
                *e3 = new HalfEdge(b3),
                *e4 = new HalfEdge(b4);
    
    e1->twin = new HalfEdge(b2);
    e2->twin = new HalfEdge(b3);
    e3->twin = new HalfEdge(b4);
    e4->twin = new HalfEdge(b1);

    e1->twin->twin = e1;
    e2->twin->twin = e2;
    e3->twin->twin = e3;
    e4->twin->twin = e4;
    
    Face        *f = new Face();

    // fill the DCEL table with bounding box
    f->outer = e1;

    // fill the HalfEdge table
    e1->next = e2;
    e2->next = e3;
    e3->next = e4;
    e4->next = e1;

    e1->prev = e4;
    e2->prev = e3;
    e3->prev = e2;
    e4->prev = e3;

    e1->face = e2->face = e3->face = e4->face = f;

    e1->twin->next = e4->twin;
    e2->twin->next = e3->twin;
    e3->twin->next = e2->twin;
    e4->twin->next = e3->twin;

    e1->twin->prev = e2->twin->prev;
    e2->twin->prev = e3->twin->prev;
    e3->twin->prev = e4->twin->prev;
    e4->twin->prev = e1->twin->prev;        // ei->twin = nullptr


    vertices.push_back(b1); vertices.push_back(b2);
    vertices.push_back(b3); vertices.push_back(b4);

    edges.push_back(e1); edges.push_back(e2);
    edges.push_back(e3); edges.push_back(e4);
    edges.push_back(e1->twin); edges.push_back(e2->twin);
    edges.push_back(e3->twin); edges.push_back(e4->twin);

    faces.push_back(f);
    leftmost.insert(e1);
    downmost.insert(e2), rightmost.insert(e3);
    upmost.insert(e4);

};


bool Arrangement::getBoundaryIntersection(Line& l, Point& p) {      // only the lefmost

    // if (l.b == 0)
    //     return false;
    // p.x = -bound_size;
    // p.y = - (l.a * p.x + l.c) / (l.b);              // no line in the input should be vertical

    // if (p.y >= -bound_size && p.y <= bound_size)
    //     return true;

    // return false;


    if (l.b == 0) {
        p.x = -(l.c) / (l.a), p.y = bsz[2];
        return true;
    }

    for (int i=0; i<4; i++) {
        double bx1 = boundary[i].pi.x, bx2 = boundary[i].pj.x, by1 = boundary[i].pi.y, by2 = boundary[i].pj.y;
        if (boundary[i].intersection(l, p))
            if (isGeq(p.x, std::min(bx1, bx2)) && isLeq(p.x, std::max(bx1, bx2)) &&
                isGeq(p.y, std::min(by1, by2)) && isLeq(p.y, std::max(by1, by2)))
                    return true;
    }

    return false;


};


HalfEdge* Arrangement::getLeftMostEdge(Line& l) {

    Point p;
    if (getBoundaryIntersection(l, p)) {
        Vertex* vl = new Vertex(p); 
        HalfEdge* dummy = new HalfEdge(vl);

        auto it = leftmost.lower_bound(dummy);
        HalfEdge* foundEdge = *it;
        
        return foundEdge;
    }

    return nullptr;
};


bool Arrangement::findIntersection(Line& l, HalfEdge* e, Point& p) {

    Point p1 = e->origin->pt, p2 = e->next->origin->pt;
    Line el(p1, p2);

    if (el.intersection(l, p))
        if ((isGeq(p.x, std::min(p1.x, p2.x)) && isLeq(p.x, std::max(p1.x, p2.x))) &&
            (isGeq(p.y, std::min(p1.y, p2.y)) && isLeq(p.y, std::max(p1.y, p2.y))))
                return true;

    return false;
}; 

HalfEdge* Arrangement::findExit(Line& l, HalfEdge* e) {

    Point p;
    HalfEdge *curr = e->next;
    while (curr != e) {
        if (findIntersection(l, curr, p))
            return curr;
        curr = curr->next;
    }

    return nullptr;
};

void Arrangement::insertLine(Line& l) {
    
    HalfEdge *e0 = getBoundaryEdge(l), *pass1, *pass2, *exit;
    pass1 = pass2 = e0->twin;       // nullptr anyway

    Point p1, p2;
    getBoundaryIntersection(l, p1);
    Vertex *v1 = new Vertex(p1), *v2;

    Face *curr_face = e0->face;
    while (curr_face) {

        exit = findExit(l, e0); 
        findIntersection(l, exit, p2);
        v2 = new Vertex(p2);

        vertices.push_back(v1), vertices.push_back(v2);     // redundant vertices in the vector!!
        updateDCEL(curr_face, v1, v2, e0, exit, pass1, pass2);
        pass1 = exit->next->twin->next; pass2 = exit; 
        v1 = v2; 
        e0 = exit->twin;

        curr_face = e0 ? e0->face : nullptr;
    }

};


void Arrangement::updateDCEL(Face *f, Vertex *v1, Vertex *v2, HalfEdge *e1, HalfEdge *e2, HalfEdge *pass1, HalfEdge *pass2) {

    HalfEdge* new_edge = new HalfEdge(v2), *new_twin_edge = new HalfEdge(v1);
    new_edge->twin = new_twin_edge;
    new_twin_edge->twin = new_edge;

    new_twin_edge->face = e1->face;         // e1->face == f
    Face *new_face = new Face(new_edge);
    new_edge->face = new_face;

    new_twin_edge->prev = e1, new_edge->prev = e2;

    HalfEdge *e1_ = new HalfEdge(v1);
    e1_->next = e1->next, e1_->prev = new_edge;
    e1->next = new_twin_edge;
    e1_->face = new_face;

    HalfEdge *curr = e1_;
    while (curr != e2) {
        curr->face = new_face;
        curr = curr->next;
    }

    // ----- UNDER INSPECTION ------
    e1->twin = pass1, pass1->twin = e1, e1_->twin = pass2, pass2->twin = e1_;
    // -----------------------------

    HalfEdge *e2_ = new HalfEdge(v2);
    e2_->next = e2->next, e2_->prev = new_twin_edge, e2_->face = f;
    e2->next = new_edge, e2->face = new_face;

    new_edge->next = e1_, new_twin_edge->next = e2_;

    // vertices.push_back(v1), vertices.push_back(v2);
    edges.push_back(e1_), edges.push_back(e2_), edges.push_back(new_edge), edges.push_back(new_twin_edge);
    faces.push_back(new_face);

    f->outer = new_twin_edge;
    v1->inc_edge = new_twin_edge, v2->inc_edge = new_edge;


    e1_->next->prev = e1_, e2_->next->prev = e2_;       // NOTE: THIS UPDATE IS NOT INCLUDED IN THE BELOW DIAGRAM. 

    if ( isEqual((e1->origin->pt).x, bsz[1]) && isEqual((e1_->origin->pt).x, bsz[1]) )
        leftmost.insert(e1_);
    else if ( isEqual((e1->origin->pt).x, bsz[3]) && isEqual((e1_->origin->pt).x, bsz[3]) )
        rightmost.insert(e1_);
    else if ( isEqual((e1->origin->pt).y, bsz[2]) && isEqual((e1_->origin->pt).y, bsz[2]) )
        downmost.insert(e1_);
    else if ( isEqual((e1->origin->pt).y, bsz[4]) && isEqual((e1_->origin->pt).y, bsz[4]) )
        upmost.insert(e1_);

    if ( isEqual((e2->origin->pt).x, bsz[1]) && isEqual((e2_->origin->pt).x, bsz[1]) )
        leftmost.insert(e2_);
    else if ( isEqual((e2->origin->pt).x, bsz[3]) && isEqual((e2_->origin->pt).x, bsz[3]) )
        rightmost.insert(e2_);
    else if ( isEqual((e2->origin->pt).y, bsz[2]) && isEqual((e2_->origin->pt).y, bsz[2]) )
        downmost.insert(e2_);
    else if ( isEqual((e2->origin->pt).y, bsz[4]) && isEqual((e2_->origin->pt).y, bsz[4]) )
        upmost.insert(e2_);

    // pass1 = e2_, pass2 = e2;
};



/*
            name    origin      twin            next            prev        face
    ---------------------------------------------------------------------------------------
            e1      (old)       pass1      new_twin_edge        (old)       (old)
            e1_     v1          pass2      e1->next           new_edge      new_face
            e2      (old)         -        new_edge             (old)       new_face
            e2_     v2            -        e2->next        new_twin_edge    (old)
        new_edge    v2     new_twin_edge   e1_                   e2         new_face
    new_twin_edge   v1         new_edge    e2_                   e1         (old)      


    Besides, all the edges between e1(old) and e2(old) should update their faces to new_face.
    Also update lefmost if e1 is an edge of left boundary



            name    outer  
    ----------------------------
            f      new_twin_edge     
        new_face   new_edge        


            name    inc_edge  
    ----------------------------
            v1     new_twin_edge     
            v2     new_edge        


*/


HalfEdge* Arrangement::getBoundaryEdge(Line& l) {

    Point p;
    if (getBoundaryIntersection(l, p)) {

        Vertex* vb = new Vertex(p); 
        HalfEdge* dummy = new HalfEdge(vb);

        bool ybound = (p.y > bsz[2]) && (p.y < bsz[4]);
        bool xbound = (p.x > bsz[1]) && (p.x < bsz[3]);

        bool isLeft = isEqual(p.x, bsz[1]) && ybound;
        bool isBottom = isEqual(p.y, bsz[2]) && xbound;
        bool isRight = isEqual(p.x, bsz[3]) && ybound;
        bool isUp = isEqual(p.y, bsz[4]) && xbound;

        HalfEdge* foundEdge;
        if (isLeft) {
            auto it = leftmost.lower_bound(dummy);
            foundEdge = *it;
        } else if (isBottom) {
            auto it = downmost.lower_bound(dummy);
            foundEdge = *it;
        } else if (isRight) {
            auto it = rightmost.lower_bound(dummy);
            foundEdge = *it;
        } else if (isUp) {
            auto it = upmost.lower_bound(dummy);
            foundEdge = *it;
        }

        return foundEdge;
    }

    return nullptr;
}




std::vector<Face*> Arrangement::zoneQuery(Line &l) {

    HalfEdge* e0 = getBoundaryEdge(l), *exit;
    Face* curr_face = e0->face;

    std::vector<Face*> cells;

    while (curr_face) {
        cells.push_back(curr_face);
        exit = findExit(l, e0);
        e0 = exit->twin;
        curr_face = e0 ? e0->face : nullptr;
    }

    return cells;
};