//
// Created by root on 2024/3/13.
//

/* Copyright 2024 wtflmao. All Rights Reserved.
 *
 * Distributed under MIT license.
 * See file LICENSE/LICENSE.MIT.md or copy at https://opensource.org/license/mit
 */

#ifndef VERTICES_TRIANGLE_H
#define VERTICES_TRIANGLE_H


#include "point.h"
#include "vec.h"
#include "../basic/BRDF.h"
#include <array>
#include <memory>

class Triangle final {
private:
    Vec normal = Vec(BigO, BigO);
    Point v0 = BigO, v1 = BigO, v2 = BigO;

public:
    std::array<double, 3> centroid;
    // this is an index
    short faceBRDF = -1;

    bool isBorderWall = false;

    [[nodiscard]] const Vec & getNormal() const noexcept;

    Triangle &setNormalVec(const Vec &n) noexcept;

    Triangle &updateNormalVec() noexcept;
    Triangle(const Point &v0, const Point &v1, const Point &v2) noexcept;

    Triangle(const Point &v0, const Point &v1, const Point &v2, const Vec &n) noexcept;

    Triangle() noexcept;
    [[maybe_unused]] [[nodiscard]] bool is_inside_triangle(const Point &p) const;

    [[nodiscard]] std::array<std::reference_wrapper<Point>, 3> getVertices() noexcept;

    void computeCentroid() noexcept;

    [[nodiscard]] Point getCentroid() const noexcept;

    [[nodiscard]] const Point &getV0() const noexcept;

    [[nodiscard]] const Point &getV1() const noexcept;

    [[nodiscard]] const Point &getV2() const noexcept;

    [[nodiscard]] Point &getMutV0() noexcept;

    [[nodiscard]] Point &getMutV1() noexcept;

    [[nodiscard]] Point &getMutV2() noexcept;

    Triangle &setV0(const Point &newV0) noexcept;

    Triangle &setV1(const Point &newV1) noexcept;

    Triangle &setV2(const Point &newV2) noexcept;
};


#endif //VERTICES_TRIANGLE_H
