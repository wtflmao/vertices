//
// Created by root on 2024/3/15.
//

#include "item.h"

const std::vector<Triangle>& Item::getFaces() const noexcept {
    return faces;
}

const std::vector<Point>& Item::getVertices() const noexcept {
    return vertices;
}

std::vector<Triangle>& Item::getMutFaces() noexcept {
    return faces;
}

std::vector<Point>& Item::getMutVertices() noexcept {
    return vertices;
}

void Item::setScaleFactor(std::array<double, 3>& factor) noexcept {
    scaleFactor = factor;
}

const std::array<double, 3>& Item::getScaleFactor() const noexcept {
    return scaleFactor;
}

void Item::setCenter(Point& pos) noexcept {
    center = pos;
}

const Point& Item::getCenter() const noexcept {
    return center;
}

Item::Item() = default;