//
// Created by root on 2024/3/13.
//

/* Copyright 2024 wtflmao. All Rights Reserved.
 *
 * Distributed under MIT license.
 * See file LICENSE/LICENSE.MIT.md or copy at https://opensource.org/license/mit
 */

#ifndef VERTICES_POINT_H
#define VERTICES_POINT_H

#include <cfloat>
#include <ostream>
#include <valarray>
#include <array>

class Point {
class Point final {
public:
    Point() noexcept;

    double x = 0.0l;
    double y = 0.0l;
    double z = 0.0l;

    Point(double x, double y, double z) noexcept;
    [[maybe_unused]] [[nodiscard]] double distance(const Point &p) const noexcept;

    Point& operator-=(const Point& otherPoint) noexcept {
        x -= otherPoint.x;
        y -= otherPoint.y;
        z -= otherPoint.z;
        return *this;
    }

    Point operator+(const Point &otherPoint) const noexcept {
        Point ret = *this;
        ret.x = x + otherPoint.x;
        ret.y = y + otherPoint.y;
        ret.z = z + otherPoint.z;
        return ret;
    }

    Point& operator*=(const double multiplier) noexcept {
        x *= multiplier;
        y *= multiplier;
        z *= multiplier;
        return *this;
    }
    bool operator==(const Point& otherPoint) const noexcept {
        constexpr double epsilon = 1e-10;
        return std::abs(this->x - otherPoint.x) < epsilon && std::abs(this->y - otherPoint.y) < epsilon && std::abs(this->z - otherPoint.z) < epsilon;
    }
    Point& operator=(const Point& point) noexcept {
        if (&point == this)
            return *this;
        x = point.x;
        y = point.y;
        z = point.z;
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const Point& point) noexcept {
        os << "Point(" << point.x << ", " << point.y << ", " << point.z << ")";
        return os;
    }

    void updatePoint(double xx, double yy, double zz) noexcept;

    std::array<double, 3> getXYZ() const noexcept;
};

const Point BigO = Point(0.0l, 0.0l, 0.0l);


#endif //VERTICES_POINT_H
