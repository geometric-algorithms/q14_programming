#include <iostream>
#include <algorithm>
#include <limits>
#include <fstream>
#include "v2_arrangement.cpp"
using namespace std;

double INF = 1e3;


void write_svg(const std::vector<Face*>& , const std::vector<Vertex*>& , const std::string&);
void write_svg_query(const std::vector<Face*>& faces, 
                     const std::vector<Vertex*>& vertices, 
                     const Line& l,
                     const std::vector<Face*>& intersects,
                     const std::string& filename);


int main() {         // USE THIS FOR TESTING

    Arrangement arr(-INF, -INF, +INF, +INF);

    Point p0(0, 0), p1(1, 0.99), p2(2, 1), p3(0, -50), p4(150, 55), p5(300, 0), p6(300, 300), p7(800, 0), p8(800, 200),
          p9(600, -1000), p10(200, 0),
          z1(1000, -600), z2(200, 300);
    Line l0(p0, p1), l1(p0, p2), l2(p3, p4), l3(p5, p6), l4(p7, p8), l5(p9, p10),
         test(z1, z2);

    arr.insertLine(l0);
    arr.insertLine(l1);
    arr.insertLine(l2);
    arr.insertLine(l3);
    arr.insertLine(l4);
    arr.insertLine(l5);

    vector<Face*> faces = arr.getFaces();

    for (auto& face: faces) {
        HalfEdge *curr = face->outer;
        do {
            cout << "(" << (curr->origin->pt).x << ", " << (curr->origin->pt).y << ")" << "\t";
            curr = curr->next;
        } while (curr != face->outer);

        cout << endl;
    }


    vector<Vertex*> vertices = arr.getVertices();

    string filename = "output.svg";
    // write_svg(faces, vertices, filename);



    // ######### TESTING ZONE'S QUERY ###########
    vector<Face*> query_cells = arr.zoneQuery(test);

    string filename2 = "query.svg";
    write_svg_query(faces, vertices, test, query_cells, filename2);
}



void boundingSizes(vector<Line>& lines, double& sz1, double& sz2, double& sz3, double& sz4) {

    if (lines.size() == 1)
        return;

    sz1 = sz2 = numeric_limits<double>::max(), sz3 = sz4 = numeric_limits<double>::min();
    sort(lines.begin(), lines.end(), [] (const Line& l1, const Line& l2) {
        bool l1_vert = (l1.b == 0), l2_vert = (l2.b == 0);
        if (l1_vert && l2_vert) return false; // both vertical, treat as equal
        if (l1_vert) return false;            // l1 is vertical -> greater
        if (l2_vert) return true;             // l2 is vertical -> l1 < l2
        return (-l1.a / l1.b) < (-l2.a / l2.b); // compare slopes
    });
    

    Point intersect;
    for (int i = 0, j; i<lines.size() - 1; i++) {
        j = i + 1;
        if (lines[i].intersection(lines[j], intersect)) {
            sz1 = min(sz1, intersect.x);
            sz3 = max(sz3, intersect.x);
            sz2 = min(sz2, intersect.y);
            sz4 = max(sz4, intersect.y);
        }
    }
}


// ---------- SVG Writer ----------
void write_svg(const std::vector<Face*>& faces, const std::vector<Vertex*>& vertices, const std::string& filename = "output.svg") {
    const int width = 2000;
    const int height = 2000;

    auto svg_x = [](double x) { return x + 1000; };
    auto svg_y = [](double y) { return 2000 - (y + 1000); };

    std::vector<std::string> colors = {
        "#FFCCCC", "#CCFFCC", "#CCCCFF",
        "#FFFFCC", "#FFCCFF", "#CCFFFF", "#E6CCFF"
    };

    std::ofstream svg(filename);
    svg << "<svg xmlns='http://www.w3.org/2000/svg' width='" << width << "' height='" << height << "'>\n";
    svg << "<rect width='100%' height='100%' fill='white'/>\n";

    // Draw faces with colors
    int color_index = 0;
    for (auto face : faces) {
        if (!face || !face->outer) continue;

        std::string color = colors[color_index % colors.size()];
        color_index++;

        svg << "<path d='M ";

        HalfEdge* start = face->outer;
        HalfEdge* he = start;
        do {
            double x = svg_x(he->origin->pt.x);
            double y = svg_y(he->origin->pt.y);
            svg << x << " " << y << " L ";
            he = he->next;
        } while (he && he != start);

        svg << "Z' fill='" << color << "' stroke='black' stroke-width='1'/>\n";
    }

    // Draw vertices
    for (auto v : vertices) {
        double x = svg_x(v->pt.x);
        double y = svg_y(v->pt.y);
        svg << "<circle cx='" << x << "' cy='" << y << "' r='2' fill='red'/>\n";
    }

    svg << "</svg>\n";
    svg.close();

    std::cout << "SVG written to " << filename << "\n";
}




void write_svg_query(const std::vector<Face*>& faces, 
                     const std::vector<Vertex*>& vertices, 
                     const Line& l,
                     const std::vector<Face*>& intersects,
                     const std::string& filename = "query.svg") 
{
    const int width = 2000;
    const int height = 2000;

    auto svg_x = [](double x) { return x + 1000; };
    auto svg_y = [](double y) { return 2000 - (y + 1000); };

    std::vector<std::string> colors = {
        "#FFCCCC", "#CCFFCC", "#CCCCFF",
        "#FFFFCC", "#FFCCFF", "#CCFFFF", "#E6CCFF"
    };

    std::ofstream svg(filename);
    svg << "<svg xmlns='http://www.w3.org/2000/svg' width='" << width << "' height='" << height << "'>\n";
    svg << "<rect width='100%' height='100%' fill='white'/>\n";

    // Color only the intersected faces
    int color_index = 0;
    for (auto face : faces) {
        if (!face || !face->outer) continue;

        bool is_intersected = std::find(intersects.begin(), intersects.end(), face) != intersects.end();
        std::string fill_color = is_intersected ? colors[color_index++ % colors.size()] : "none";

        svg << "<path d='M ";
        HalfEdge* start = face->outer;
        HalfEdge* he = start;
        do {
            double x = svg_x(he->origin->pt.x);
            double y = svg_y(he->origin->pt.y);
            svg << x << " " << y << " L ";
            he = he->next;
        } while (he && he != start);
        svg << "Z' fill='" << fill_color << "' stroke='black' stroke-width='1'/>\n";
    }

    // Draw vertices
    for (auto v : vertices) {
        double x = svg_x(v->pt.x);
        double y = svg_y(v->pt.y);
        svg << "<circle cx='" << x << "' cy='" << y << "' r='2' fill='red'/>\n";
    }

    // Draw query line
    // Extend query line to fit the full bounding box
    Point p1, p2;
    if (std::abs(l.b) > 1e-9) {
        // Two points from x = -1000 and x = 1000
        double y1 = -(l.a * -1000 + l.c) / l.b;
        double y2 = -(l.a * 1000 + l.c) / l.b;
        p1 = Point(-1000, y1);
        p2 = Point(1000, y2);
    } else {
        // Vertical line: x = -c/a
        double x_val = -l.c / l.a;
        p1 = Point(x_val, -1000);
        p2 = Point(x_val, 1000);
    }

    svg << "<line x1='" << svg_x(p1.x) << "' y1='" << svg_y(p1.y) << "' "
        << "x2='" << svg_x(p2.x) << "' y2='" << svg_y(p2.y) << "' "
        << "stroke='blue' stroke-width='2' stroke-dasharray='10,5'/>\n";

    svg << "</svg>\n";
    svg.close();

    std::cout << "Query SVG written to " << filename << "\n";
}
