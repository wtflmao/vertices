//
// Created by root on 2024/3/13.
//

#ifndef VERTICES_TRIANGLE_H
#define VERTICES_TRIANGLE_H


#include "point.h"
#include "vec.h"
#include <array>

class Triangle {
private:
    Vec normal = Vec(BigO, BigO);

public:
    Point v0 = BigO, v1 = BigO, v2 = BigO;
    std::array<double, 3> centroid;

    [[nodiscard]] const Vec & getNormal() const noexcept;

    void setNormalVec(const Vec &n) noexcept;

    void updateNormalVec() noexcept;
    Triangle(const Point &v0, const Point &v1, const Point &v2) noexcept;

    Triangle(const Point &v0, const Point &v1, const Point &v2, const Vec &n) noexcept;

    Triangle() noexcept;
    [[maybe_unused]] [[nodiscard]] bool is_inside_triangle(const Point &p) const;

    [[nodiscard]] std::array<std::reference_wrapper<Point>, 3> getVertices() noexcept;

    void computeCentroid() noexcept;
};


#endif //VERTICES_TRIANGLE_H
