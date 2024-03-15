//
// Created by root on 2024/3/13.
//

#include <valarray>
#include "ray.h"
#include <cfloat>

// Constructor
Ray::Ray(const Point& origin, const Vec& direction)
    : origin(origin), direction(direction) {

}
Ray::Ray(Point direction)
    : origin(Point(0.0l, 0.0l, 0.0l)),      // Explaining Variable
        direction(Vec(direction)) {

}

const Point& Ray::getOrigin() const {
    return this->origin;
}

const Vec& Ray::getDirection() const {
    return this->direction;
}

// Cross product: self x other
[[maybe_unused]] [[maybe_unused]] Vec Ray::crossVec(const Vec &other) const {
    const double v[3] = {this->direction.tail.y*other.tail.z - this->direction.tail.z * other.tail.y,
                         this->direction.tail.z*other.tail.x - this->direction.tail.x * other.tail.z,
                         this->direction.tail.x*other.tail.y - this->direction.tail.y * other.tail.x};
    return Vec(Point(v[0], v[1], v[2]));
}

// Dot product: self . other
double Ray::dotVec(const Vec &other) const {
    return this->direction.tail.x*other.tail.x + this->direction.tail.y * other.tail.y + this->direction.tail.z * other.tail.z;
}

// Member function
Point Ray::mollerTrumboreIntersection(const Triangle& tri) const {
    double det, inv_det, u, v, t;
    Point intersection = Point{DBL_MAX, DBL_MAX, DBL_MAX};


    //dge1 = tri.v1 - tri.v0;
    Vec edge1 = Vec(tri.v0, tri.v1);
    //edge2 = tri.v2 - tri.v0;
    Vec edge2 = Vec(tri.v0, tri.v2);
    Vec pvec = this->crossVec(edge2);
    det = edge1.dot(pvec);

    if (det > -1e-6 && det < 1e-6) {
        return intersection; // Ray is parallel to the triangle, no intersection
    }

    inv_det = 1.0 / det;

    Vec tvec = Vec(tri.v0, this->origin);
    u = tvec.dot(pvec) * inv_det;

    if (u < 0.0 || u > 1.0){
        return intersection; // The intersection lies outside of the triangle DBL_MAX
    }

    auto qvec = tvec.cross(edge1);
    v = this->dotVec(qvec) * inv_det;

    if (v < 0.0 || u + v > 1.0) {
        return intersection; // The intersection lies outside of the triangle DBL_MAX
    }

    t = edge2.dot(qvec) * inv_det;

    if (t > 1e-6) {
        intersection = this->direction * t + this->origin;
        return intersection; // Ray intersect the triangle
    }

    return intersection;
}