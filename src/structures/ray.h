//
// Created by root on 2024/3/13.
//

#ifndef VERTICES_RAY_H
#define VERTICES_RAY_H


#include "point.h"
#include "vec.h"
#include "triangle.h"


class Ray {
private:
    Point origin = BigO;
    // direction is a vector(starts at wherever) and has nothing to do with origin
    Vec direction = Vec(Point(2,2,2));

public:
    Ray(const Point &origin, const Vec &direction);
    explicit Ray(const Point& direction);

    [[nodiscard]] Vec crossVec(const Vec& vec) const;
    [[nodiscard]] double dotVec(const Vec& vec) const;
    [[nodiscard]] const Point& getOrigin() const;
    [[nodiscard]] const Vec& getDirection() const;
    [[nodiscard]] Point mollerTrumboreIntersection(const Triangle &tri) const;
};


#endif //VERTICES_RAY_H
