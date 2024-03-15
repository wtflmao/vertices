//
// Created by root on 2024/3/13.
//

#include "point.h"

// Constructor
Point::Point(double x, double y, double z) {
    this->x = x;
    this->y = y;
    this->z = z;
}

Point::Point() = default;

// Member functions
[[maybe_unused]] double Point::distance(const Point &p) const {
    return sqrt(pow(this->x - p.x, 2) + pow(this->y - p.y, 2) + pow(this->z - p.z, 2));
}

void Point::updatePoint(double xx, double yy, double zz) {
    this->x = xx;
    this->y = yy;
    this->z = zz;
}




