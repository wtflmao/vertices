//
// Created by root on 2024/3/13.
//

/* Copyright 2024 wtflmao. All Rights Reserved.
 *
 * Distributed under MIT license.
 * See file LICENSE/LICENSE.MIT.md or copy at https://opensource.org/license/mit
 */

#include "point.h"

// Constructor
Point::Point(double x, double y, double z) noexcept {
    this->x = x;
    this->y = y;
    this->z = z;
}

Point::Point() noexcept = default;

// Member functions
double Point::distance(const Point &p) const noexcept {
    return sqrt(pow(this->x - p.x, 2) + pow(this->y - p.y, 2) + pow(this->z - p.z, 2));
}

[[deprecated]] void Point::updatePoint(double xx, double yy, double zz) noexcept {
    x = xx;
    y = yy;
    z = zz;
}

std::array<double, 3> Point::getXYZ() const noexcept {
    return std::array<double, 3>{x, y, z};
}

Point &Point::setX(double t) noexcept {
    x = t;
    return *this;
}

Point &Point::setY(double t) noexcept {
    y = t;
    return *this;
}

Point &Point::setZ(double t) noexcept {
    z = t;
    return *this;
}

Point &Point::setXYZ(std::array<double, 3> &t) noexcept {
    x = t.at(0);
    y = t.at(1);
    z = t.at(2);
    return *this;
}

double Point::getX() const noexcept {
    return x;
}

double Point::getY() const noexcept {
    return y;
}

double Point::getZ() const noexcept {
    return z;
}


