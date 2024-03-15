//
// Created by root on 2024/3/13.
//

#ifndef VERTICES_VEC_H
#define VERTICES_VEC_H


#include "point.h"

class Vec {
private:



    double length = 0.0l;
    void updateLength();
    void setLength(double len);

protected:
    [[maybe_unused]] Vec(Point, double);

public:
    Point head = Point(0.0l, 0.0l, 0.0l);
    Point tail = BigO;

    Vec(Point, Point);
    explicit Vec(Point);

    //explicit Vec(Ray &ray);
    [[nodiscard]] double getLength() const;
    [[maybe_unused]] [[nodiscard]] Vec cross(const Vec& other) const;
    [[nodiscard]] double dot(const Vec& other) const;
    [[maybe_unused]] [[nodiscard]] Vec getNormalized() const;

    Vec operator*(const double multiplier) const {
        Point result = this->tail;
        result.x *= multiplier;
        result.y *= multiplier;
        result.z *= multiplier;
        return Vec(result);
    }
    Point operator+(const Point& point) const {
        Point result = this->tail;
        result.x += point.x;
        result.y += point.y;
        result.z += point.z;
        return result;
    }
    Vec& operator=(const Vec& vec) {
        if (&vec == this)
            return *this;
        head = vec.head;
        tail = vec.tail;
        length = vec.length;
        return *this;
    }
};

[[maybe_unused]] const Vec BigVec = Vec(BigO, BigO);


#endif //VERTICES_VEC_H
