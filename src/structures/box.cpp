//
// Created by root on 2024/3/16.
//

#include "box.h"

Box::Box(const Point &min, const Point &max) noexcept
    : bounds({min, max}) {
}

Box::Box() noexcept = default;

bool Box::isEmpty() const noexcept {
    return bounds[0] == bounds[1];
}

void Box::setBounds(const Point &min, const Point &max) noexcept {
    bounds[0] = min;
    bounds[1] = max;
}

void Box::setBounds(const std::array<Point, 2> &bounds_t) noexcept {
    bounds[0] = bounds_t[0];
    bounds[1] = bounds_t[1];
}

const std::array<Point, 2>& Box::getBounds() const noexcept {
    return bounds;
}





