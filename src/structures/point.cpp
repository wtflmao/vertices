//
// Created by root on 2024/3/13.
//

#include <valarray>
#include "point.h"

// Constructor
Point::Point(double x, double y, double z) {
    this->x = x;
    this->y = y;
    this->z = z;
}

// Member functions
[[maybe_unused]] double Point::distance(Point *p) const {
    return sqrt(pow(this->x - p->x, 2) + pow(this->y - p->y, 2) + pow(this->z - p->z, 2));
}





