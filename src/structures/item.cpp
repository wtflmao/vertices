//
// Created by root on 2024/3/15.
//

#include "item.h"

const std::vector<Triangle>& Item::getFaces() const {
    return faces;
}

const std::vector<Point>& Item::getVertices() const {
    return vertices;
}

std::vector<Triangle>& Item::getMutFaces() {
    return faces;
}

std::vector<Point>& Item::getMutVertices() {
    return vertices;
}

Item::Item() = default;