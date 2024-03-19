//
// Created by root on 2024/3/13.
//

#ifndef VERTICES_RAY_H
#define VERTICES_RAY_H

#include "../common.h"
#include "point.h"
#include "vec.h"
#include "triangle.h"
#include "box.h"

//constexpr int SCATTER_RAYS = 8;

class Ray {
private:
    Point origin = BigO;
    // direction is a vector(starts at wherever) and has nothing to do with origin
    Vec direction = Vec(Point(2,2,2));

public:
    double intensity = 100.0;
    int scatteredLevel = 0;

    Ray(const Point &origin, const Vec &direction) noexcept;
    explicit Ray(const Point& direction) noexcept;

    Ray() noexcept;

    [[nodiscard]] Vec crossVec(const Vec& vec) const noexcept;
    [[nodiscard]] double dotVec(const Vec& vec) const noexcept;
    [[nodiscard]] const Point& getOrigin() const noexcept;
    [[nodiscard]] const Vec& getDirection() const noexcept;
    [[nodiscard]] Point mollerTrumboreIntersection(const Triangle &tri) const;

    bool intersectsWithBox(const Box &box);

    Ray(const Point &origin, const Vec &direction, double intesity, int scatteredCount);

    [[nodiscard]] std::array<Ray, SCATTER_RAYS> scatter(const Triangle &tri, const Point &intersection,
                                                        double reflectance) const;
};


#endif //VERTICES_RAY_H
