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

class Point final {
private:
    double x = 0.0l;
    double y = 0.0l;
    double z = 0.0l;

public:
    Point() noexcept;
    Point(double x, double y, double z) noexcept;
    [[maybe_unused]] [[nodiscard]] double distance(const Point &p) const noexcept;

    std::array<double, 3> getXYZ() const noexcept;

    Point &setX(double t) noexcept;

    Point &setY(double t) noexcept;

    Point &setZ(double t) noexcept;

    Point &setXYZ(std::array<double, 3> &t) noexcept;

    [[nodiscard]] double getX() const noexcept;

    [[nodiscard]] double getY() const noexcept;

    [[nodiscard]] double getZ() const noexcept;

    Point& operator-=(const Point& otherPoint) noexcept {
        setX(getX() - otherPoint.getX());
        setY(getY() - otherPoint.getY());
        setZ(getZ() - otherPoint.getZ());
        return *this;
    }

    Point operator+(const Point &otherPoint) const noexcept {
        Point ret(x + otherPoint.x, y + otherPoint.y, z + otherPoint.z);
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

    Point &operator=(const Point &point) {
        if (&point == this)
            return *this;
        if (this == nullptr)
            throw std::runtime_error("Attempted to assign to null Point object");
        if (&point == nullptr)
            throw std::runtime_error("Attempted to assign from null Point object");
        x = point.getX();
        y = point.getY();
        z = point.getZ();
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const Point& point) noexcept {
        os << "Point(" << point.x << ", " << point.y << ", " << point.z << ")";
        return os;
    }

    [[deprecated]] void updatePoint(double xx, double yy, double zz) noexcept;
};

const Point BigO = Point(0.0l, 0.0l, 0.0l);


#endif //VERTICES_POINT_H
