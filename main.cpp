// Using C++23 standard
//
// Created by root on 2024/3/13.
//

#include <iostream>
#include <valarray>
#include <vector>
#include <cfloat>
#include "src/structures/ray.h"
#include "src/structures/triangle.h"


int main() {
    const Point NO_INTERSECT = Point(DBL_MAX, DBL_MAX, DBL_MAX);
    std::vector<Point> vertices = {Point(0.0, 0.0, 0.0), Point(1.0, 0.0, 0.0), Point(0.0, 1.0, 2.0)};
    Triangle tri = Triangle(vertices[0], vertices[1], vertices[2]);
    Ray r = Ray(Point(0.1, 0.2, 4.22), Vec(Point(0,0.1,-1)));
    Ray r2 = Ray(Point(3, 2, -6), Vec(Point(1,1,1)));

    if (NO_INTERSECT != r.mollerTrumboreIntersection(tri)) {
        std::cout << "The ray intersects the triangle at " << r.mollerTrumboreIntersection(tri) << std::endl;
    } else {
        std::cout << "The ray does not intersect the triangle." << std::endl;
    }

    return 0;
}