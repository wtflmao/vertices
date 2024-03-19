//
// Created by root on 2024/3/13.
//

#ifndef VERTICES_RAY_H
#define VERTICES_RAY_H


#include "point.h"
#include "vec.h"
#include "triangle.h"
#include "box.h"


class Ray {
private:
    Point origin = BigO;
    // direction is a vector(starts at wherever) and has nothing to do with origin
    Vec direction = Vec(Point(2,2,2));

public:
    Ray(const Point &origin, const Vec &direction) noexcept;
    explicit Ray(const Point& direction) noexcept;

    [[nodiscard]] Vec crossVec(const Vec& vec) const noexcept;
    [[nodiscard]] double dotVec(const Vec& vec) const noexcept;
    [[nodiscard]] const Point& getOrigin() const noexcept;
    [[nodiscard]] const Vec& getDirection() const noexcept;
    [[nodiscard]] Point mollerTrumboreIntersection(const Triangle &tri) const;

    bool intersectsWithBox(const Box &box);
};


#endif //VERTICES_RAY_H
