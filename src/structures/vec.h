//
// Created by root on 2024/3/13.
//

/* Copyright 2024 wtflmao. All Rights Reserved.
 *
 * Distributed under MIT license.
 * See file LICENSE/LICENSE.MIT.md or copy at https://opensource.org/license/mit
 */

#ifndef VERTICES_VEC_H
#define VERTICES_VEC_H


#include "point.h"

class Vec final {
private:
    double length = 0.0l;

    Vec &updateLength() noexcept;

    Vec &setLength(double len) noexcept;
    Point head = Point(0.0l, 0.0l, 0.0l);
    Point tail = BigO;

public:
    [[maybe_unused]] Vec(const Point &, double) noexcept;
    Vec(const Point &origin, const Point &direction) noexcept;

    explicit Vec(const Point &) noexcept;

    //explicit Vec(Ray &ray);
    [[nodiscard]] double getLength() const noexcept;

    [[nodiscard]] Vec cross(const Vec &other) const noexcept;
    [[nodiscard]] double dot(const Vec& other) const noexcept;

    [[nodiscard]] Vec getNormalized() const noexcept;

    Vec &setHead(const Point &newHead) noexcept;

    Vec &setTail(const Point &newTail) noexcept;

    [[nodiscard]] const Point &getHead() const noexcept;

    [[nodiscard]] const Point &getTail() const noexcept;

    Vec operator*(const double multiplier) const noexcept {
        Point t_direction = this->tail;
        t_direction *= multiplier;
        Point t_origin = this->head;
        t_origin *= multiplier;
        return {t_origin, t_direction};
    }
    Point operator+(const Point& point) const noexcept {
        Point result = this->tail;
        result.setX(result.getX() + point.getX())
                .setY(result.getY() + point.getY())
                .setZ(result.getZ() + point.getZ());
        return result;
    }

    Vec operator+(const Vec &anotherVec) const noexcept {
        return {head + anotherVec.head, tail + anotherVec.tail};
    }

    Vec operator-(const Vec &anotherVec) const noexcept {
        Point new_head = head;
        Point new_tail = tail;
        new_head -= anotherVec.head;
        new_tail -= anotherVec.tail;
        return {new_head, new_tail};
    }
    Vec& operator=(const Vec& vec) noexcept {
        if (&vec == this)
            return *this;
        head = vec.head;
        tail = vec.tail;
        length = vec.length;
        return *this;
    }

    // invert the vector
    Vec& operator*=(const double multiplier) noexcept {
        this->head *= multiplier;
        this->tail *= multiplier;
        return *this;
    }
};

[[maybe_unused]] const Vec BigVec = Vec(BigO, BigO);


#endif //VERTICES_VEC_H
