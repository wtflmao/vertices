//
// Created by root on 2024/3/13.
//

#ifndef VERTICES_VEC_H
#define VERTICES_VEC_H


#include "point.h"

class Vec {
private:
    double length = 0.0l;
    void updateLength() noexcept;
    void setLength(double len) noexcept;

protected:
    [[maybe_unused]] Vec(Point, double) noexcept;

public:
    Point head = Point(0.0l, 0.0l, 0.0l);
    Point tail = BigO;

    Vec(Point origin, Point direction) noexcept;
    explicit Vec(Point) noexcept;

    //explicit Vec(Ray &ray);
    [[nodiscard]] double getLength() const noexcept;
    [[maybe_unused]] [[nodiscard]] Vec cross(const Vec& other) const noexcept;
    [[nodiscard]] double dot(const Vec& other) const noexcept;
    [[maybe_unused]] [[nodiscard]] Vec getNormalized() const noexcept;

    Vec operator*(const double multiplier) const noexcept {
        Point t_direction = this->tail;
        t_direction *= multiplier;
        Point t_origin = this->head;
        t_origin *= multiplier;
        return {t_origin, t_direction};
    }
    Point operator+(const Point& point) const noexcept {
        Point result = this->tail;
        result.x += point.x;
        result.y += point.y;
        result.z += point.z;
        return result;
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
