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

class Ray final {
private:
    // origin indicates where is this ray starts
    Point origin = BigO;
    // direction is a vector(starts at wherever) and has nothing to do with origin
    Vec direction = Vec(Point(2,2,2));
    // stopPoint is for when the ray intersects with a surface or is out of the box and we should stop the ray from goin any further
    double stopLength = STOP_LENGTH;
    // ancestor indicates where is this ray's ancestor(the one has the lowest scatter level) starts
    Point ancestor = BigO;
    int scatteredLevel = 0;
    std::array<double, spectralBands> intensity_p = {0.0};
    Point sourcePixelPosInGnd = BigO;
    void *sourcePixel = nullptr;

public:

    //std::shared_ptr<std::array<double, spectralBands>> spectrum_p = std::make_shared<std::array<double, spectralBands>>();

    Ray(const Point &origin, const Vec &direction) noexcept;
    explicit Ray(const Point& direction) noexcept;

    Ray(const Point &origin, const Vec &direction,
        std::array<double, spectralBands> intensity_p) noexcept;

    [[nodiscard]] void *getSourcePixel() const noexcept;

    Ray &setSourcePixel(void *p) noexcept;

    Ray() noexcept;

    [[nodiscard]] Vec crossVec(const Vec &other) const noexcept;

    [[nodiscard]] double dotVec(const Vec &other) const noexcept;

    [[nodiscard]] Point mollerTrumboreIntersection(const Triangle &tri) const;

    [[nodiscard]] bool intersectsWithBox(const Box &box) const;

    Ray(const Point &origin, const Vec &direction, const std::array<double, spectralBands> &intesity,
        int scatteredCount);

    [[nodiscard]] std::array<Ray, SCATTER_RAYS + 1> scatter(const Triangle &tri, const Point &intersection,
                                                            BRDF *itemBRDF, void *sourcePixel = nullptr) const;

    void setRayStopPoint(const Point &stopPoint_t) noexcept;

    [[nodiscard]] const Point &getOrigin() const noexcept;

    [[nodiscard]] const Point &getAncestor() const noexcept;

    [[nodiscard]] int getScatteredLevel() const noexcept;

    [[nodiscard]] const Vec &getDirection() const noexcept;

    [[nodiscard]] const std::array<double, spectralBands> &getIntensity_p() const noexcept;

    [[nodiscard]] std::array<double, spectralBands> &getMutIntensity_p() noexcept;

    Ray &setAncestor(const Point &ancestor_t) noexcept;

    [[nodiscard]] const Point &getSourcePixelPosInGnd() const noexcept;

    Ray &setSourcePixelPosInGnd(const Point &t) noexcept;

    Ray &setOrigin(const Point &origin_t) noexcept;

    Ray &setScatteredLevel(int t) noexcept;

    Ray &setDirection(const Vec &d_t) noexcept;

    Ray &setIntensity_p(const std::array<double, spectralBands> &t) noexcept;
};

void computeCoordinateSystem(const Vec &normal, Vec &tangent, Vec &bitangent);

Vec uniformHemisphereDirection(const Vec &normal);

Vec uniformHemisphereDirectionWithCenterOfMonteCarloSpace(const Vec &normal, const Vec &reflectionDirection);
#endif //VERTICES_RAY_H
