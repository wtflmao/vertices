//
// Created by root on 2024/3/13.
//

#ifndef VERTICES_POINT_H
#define VERTICES_POINT_H

#include <cfloat>
#include <ostream>
#include <valarray>

class Point {
public:
    Point();

    double x = 0.0l;
    double y = 0.0l;
    double z = 0.0l;

    Point(double x, double y, double z);
    [[maybe_unused]] [[nodiscard]] double distance(const Point &p) const;

    Point& operator-=(const Point& otherPoint) {
        x -= otherPoint.x;
        y -= otherPoint.y;
        z -= otherPoint.z;
        return *this;
    }
    Point& operator*=(const double multiplier) {
        x *= multiplier;
        y *= multiplier;
        z *= multiplier;
        return *this;
    }
    bool operator==(const Point& otherPoint) const {
        double epsilon = 1e-6;
        return std::abs(this->x - otherPoint.x) < epsilon && std::abs(this->y - otherPoint.y) < epsilon && std::abs(this->z - otherPoint.z) < epsilon;
    }
    Point& operator=(const Point& point) {
        if (&point == this)
            return *this;
        x = point.x;
        y = point.y;
        z = point.z;
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const Point& point) {
        os << "Point(" << point.x << ", " << point.y << ", " << point.z << ")";
        return os;
    }

    void updatePoint(double xx, double yy, double zz);
};

const Point BigO = Point(0.0l, 0.0l, 0.0l);


#endif //VERTICES_POINT_H
