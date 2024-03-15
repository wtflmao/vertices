//
// Created by root on 2024/3/13.
//

#ifndef VERTICES_TRIANGLE_H
#define VERTICES_TRIANGLE_H


#include "point.h"
#include "vec.h"

class Triangle {
private:
    Vec normal = Vec(BigO, BigO);

public:
    Point v0 = BigO, v1 = BigO, v2 = BigO;

    Triangle(Point, Point, Point);
    [[nodiscard]] Vec getNormal() const;

    void setNormalVec(Vec n);

    void updateNormalVec();

    [[nodiscard]] bool is_inside_triangle(const Point &p) const;
};


#endif //VERTICES_TRIANGLE_H
