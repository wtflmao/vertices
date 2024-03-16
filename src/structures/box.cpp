//
// Created by root on 2024/3/16.
//

#include "box.h"

Box::Box(const Point &min, const Point &max) noexcept
    : bounds({min, max}) {
    for (auto &child : children) {
        child = nullptr;
    }
}

Box::Box() noexcept {
    for (auto &child : children) {
        child = nullptr;
    }
}

bool Box::isLeaf() const noexcept {
    return children[0] == nullptr;
}

bool Box::isEmpty() const noexcept {
    return isLeaf();
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

const std::array<std::unique_ptr<Box>, BVH_NODE_CHILDREN>& Box::getChildren() const noexcept {
    return children;
}

void Box::build(std::vector<Item>& objects) {


    // TODO: Implement
}