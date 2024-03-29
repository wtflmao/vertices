//
// Created by root on 2024/3/13.
//

/* Copyright 2024 wtflmao. All Rights Reserved.
 *
 * Distributed under MIT license.
 * See file LICENSE/LICENSE.MIT.md or copy at https://opensource.org/license/mit
 */

#ifndef VERTICES_RAY_H
#define VERTICES_RAY_H

#include "../common.h"
#include "point.h"
#include "vec.h"
#include "triangle.h"
#include "box.h"
#include "../basic/BRDF.h"
#include <numbers>

//constexpr int SCATTER_RAYS = 8;

class Ray {
private:
    // origin indicates where is this ray starts
    Point origin = BigO;
    // direction is a vector(starts at wherever) and has nothing to do with origin
    Vec direction = Vec(Point(2,2,2));
    // stopPoint is for when the ray intersects with a surface or is out of the box and we should stop the ray from goin any further
    double stopLength = STOP_LENGTH;

public:
    // ancestor indicates where is this ray's ancestor(the one has the lowest scatter level) starts
    Point ancestor = BigO;

    std::array<double, spectralBands> intensity_p = {};
    int scatteredLevel = 0;
    //std::shared_ptr<std::array<double, spectralBands>> spectrum_p = std::make_shared<std::array<double, spectralBands>>();

    Ray(const Point &origin, const Vec &direction) noexcept;
    explicit Ray(const Point& direction) noexcept;

    Ray(const Point &origin, const Vec &direction,
        std::array<double, spectralBands> intensity_p) noexcept;

    Ray() noexcept;

    [[nodiscard]] Vec crossVec(const Vec &other) const noexcept;

    [[nodiscard]] double dotVec(const Vec &other) const noexcept;

    [[nodiscard]] const Point& getOrigin() const noexcept;
    [[nodiscard]] const Vec& getDirection() const noexcept;
    [[nodiscard]] Point mollerTrumboreIntersection(const Triangle &tri) const;

    [[nodiscard]] bool intersectsWithBox(const Box &box) const;

    Ray(const Point &origin, const Vec &direction, const std::array<double, spectralBands> &intesity,
        int scatteredCount);

    [[nodiscard]] std::array<Ray, SCATTER_RAYS + 1> scatter(const Triangle &tri, const Point &intersection,
                                                            const std::shared_ptr<BRDF *> &itemBRDF) const;

    void setRayStopPoint(const Point &stopPoint_t) noexcept;

};

void computeCoordinateSystem(const Vec &normal, Vec &tangent, Vec &bitangent);

Vec uniformHemisphereDirection(const Vec &normal);

#endif //VERTICES_RAY_H
