//
// Created by root on 2024/3/13.
//

#include "point.h"

// Constructor
Point::Point(double x, double y, double z) noexcept {
    this->x = x;
    this->y = y;
    this->z = z;
}

Point::Point() noexcept = default;

// Member functions
[[maybe_unused]] double Point::distance(const Point &p) const noexcept {
    return sqrt(pow(this->x - p.x, 2) + pow(this->y - p.y, 2) + pow(this->z - p.z, 2));
}

void Point::updatePoint(double xx, double yy, double zz) noexcept {
    this->x = xx;
    this->y = yy;
    this->z = zz;
}

std::array<double, 3> Point::getXYZ() const noexcept {
    return std::array<double, 3>{this->x, this->y, this->z};
}



