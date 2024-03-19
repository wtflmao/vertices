//
// Created by root on 2024/3/13.
//

#include "ray.h"

// Constructor
Ray::Ray(const Point& origin, const Vec& direction) noexcept
    : origin(origin), direction(direction) {

}
Ray::Ray(const Point& direction) noexcept
    : direction(Vec(direction)) {

}

const Point& Ray::getOrigin() const noexcept {
    return this->origin;
}

const Vec& Ray::getDirection() const noexcept {
    return this->direction;
}

// Cross product: self x other
[[maybe_unused]] [[maybe_unused]] Vec Ray::crossVec(const Vec &other) const noexcept {
    const double v[3] = {this->direction.tail.y*other.tail.z - this->direction.tail.z * other.tail.y,
                         this->direction.tail.z*other.tail.x - this->direction.tail.x * other.tail.z,
                         this->direction.tail.x*other.tail.y - this->direction.tail.y * other.tail.x};
    return Vec(Point(v[0], v[1], v[2]));
}

// Dot product: self . other
double Ray::dotVec(const Vec &other) const noexcept {
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

    // Check if ray is parallel to triangle
    if (det > -(1e-6) && det < 1e-6) {
        // Ray is parallel to the triangle, no intersection
        return intersection; }

    // Calculate distance from v0 to ray origin
    inv_det = 1.0 / det;
    Vec tvec = Vec(tri.v0, this->origin);
    u = tvec.dot(pvec) * inv_det;

    // Check if intersection point is outside the first edge
    if (u < 0.0 || u > 1.0) {
        // The intersection lies outside of the first edge
        return intersection;
    }

    // Prepare to test the second edge
    auto qvec = tvec.cross(edge1);
    v = this->dotVec(qvec) * inv_det;

    // Check if intersection point is outside the second edge
    if (v < 0 || u + v > 1.0) {
        // The intersection lies outside of the triangle 2nd egde
        return intersection;
    }

    // Compute the t value for the intersection point
    t = edge2.dot(qvec) * inv_det;

    // Check if the intersection point is behind the ray
    if (t < 0.0) {
        // The intersection lies behind the ray
        return intersection;
    }

    // Intersection point is t distance away from ray origin in direction of ray
    intersection = this->direction * t + this->origin;
    return intersection;
}

bool Ray::intersectsWithBox(const Box& box) {
    double tMin = -std::numeric_limits<double>::infinity();
    double tMax = std::numeric_limits<double>::infinity();

    const auto directionXYZ = direction.tail.getXYZ();
    const auto originXYZ = origin.getXYZ();
    const auto minBoundXYZ = box.getBounds()[0].getXYZ();
    const auto maxBoundXYZ = box.getBounds()[1].getXYZ();

    for (int i = 0; i < 3; i++) {
        double invDir = 1.0 / directionXYZ[i];
        double t1 = (minBoundXYZ[i] - originXYZ[i]) / directionXYZ[i];
        double t2 = (maxBoundXYZ[i] - originXYZ[i]) / directionXYZ[i];
        if (t1 > t2) {
            const double tmp = t1;
            t1 = t2;
            t2 = tmp;
        }
        if (t1 > tMin) {
            tMin = t1;
        }
        if (t2 < tMax) {
            tMax = t2;
        }
        if (tMin > tMax) {
            return false;
        }
    }
    return true;
}