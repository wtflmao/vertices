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
#include <numbers>

//constexpr int SCATTER_RAYS = 8;

class Ray {
private:
    Point origin = BigO;
    // direction is a vector(starts at wherever) and has nothing to do with origin
    Vec direction = Vec(Point(2,2,2));
    // stopPoint is for when the ray intersects with a surface or is out of the box and we should stop the ray from goin any further
    size_t stopLength = 999;

public:
    std::shared_ptr<std::array<double, spectralBands> > intensity_p = std::make_shared<std::array<double,
        spectralBands> >();
    int scatteredLevel = 0;
    //std::shared_ptr<std::array<double, spectralBands>> spectrum_p = std::make_shared<std::array<double, spectralBands>>();

    Ray(const Point &origin, const Vec &direction) noexcept;
    explicit Ray(const Point& direction) noexcept;
    Ray(const Point &origin, const Vec &direction, std::shared_ptr<std::array<double, spectralBands>> intensity_p) noexcept;

    Ray() noexcept;

    [[nodiscard]] Vec crossVec(const Vec& vec) const noexcept;
    [[nodiscard]] double dotVec(const Vec& vec) const noexcept;
    [[nodiscard]] const Point& getOrigin() const noexcept;
    [[nodiscard]] const Vec& getDirection() const noexcept;
    [[nodiscard]] Point mollerTrumboreIntersection(const Triangle &tri) const;

    bool intersectsWithBox(const Box &box);

    Ray(const Point &origin, const Vec &direction, const std::array<double, spectralBands> &intesity,
        int scatteredCount);

    [[nodiscard]] std::array<Ray, SCATTER_RAYS + 1> scatter(const Triangle &tri, const Point &intersection,
                                                            double reflectance) const;

    void setRayStopPoint(const Point &stopPoint_t) noexcept;
};


#endif //VERTICES_RAY_H
