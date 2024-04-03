//
// Created by root on 2024/3/13.
//

/* Copyright 2024 wtflmao. All Rights Reserved.
 *
 * Distributed under MIT license.
 * See file LICENSE/LICENSE.MIT.md or copy at https://opensource.org/license/mit
 */

#include "vec.h"

// Constructor
[[maybe_unused]] Vec::Vec(const Point &direction, double len) noexcept {
    tail = direction;
    setLength(len);
}
// Constructor, but make head at (0, 0, 0)
Vec::Vec(const Point &origin, const Point &direction) noexcept {
    const double v[3] = {direction.getX() - origin.getX(), direction.getY() - origin.getY(),
                         direction.getZ() - origin.getZ()};
    tail = Point(v[0], v[1], v[2]);
    updateLength();
}

// Constructor
Vec::Vec(const Point &direction) noexcept {
    tail = direction;
    updateLength();
}

Vec &Vec::updateLength() noexcept {
    const double v[3] = {tail.getX() - head.getX(), tail.getY() - head.getY(), tail.getZ() - head.getZ()};
    setLength(sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]));
    return *this;
}

Vec &Vec::setLength(double len) noexcept {
    length = len;
    return *this;
}

double Vec::getLength() const noexcept {
    return length;
}

[[maybe_unused]] Vec Vec::getNormalized() const noexcept {
    return Vec(Point((tail.getX() - head.getX()) / getLength(), (tail.getY() - head.getY()) / getLength(),
                     (tail.getZ() - head.getZ()) / getLength()));
}

// Cross product: self x other
[[maybe_unused]] Vec Vec::cross(const Vec &other) const noexcept {
    const double v[3] = {this->tail.getY() * other.tail.getZ() - this->tail.getZ() * other.tail.getY(),
                         this->tail.getZ() * other.tail.getX() - this->tail.getX() * other.tail.getZ(),
                         this->tail.getX() * other.tail.getY() - this->tail.getY() * other.tail.getX()};
    return Vec(Point(v[0], v[1], v[2]));
}

// Dot product: self . other
double Vec::dot(const Vec &other) const noexcept {
    return this->tail.getX() * other.tail.getX() + this->tail.getY() * other.tail.getY() +
           this->tail.getZ() * other.tail.getZ();
}

Vec &Vec::setHead(const Point &newHead) noexcept {
    head = newHead;
    return *this;
}

Vec &Vec::setTail(const Point &newTail) noexcept {
    head = newTail;
    return *this;
}

const Point &Vec::getHead() const noexcept {
    return head;
}

const Point &Vec::getTail() const noexcept {
    return tail;
}