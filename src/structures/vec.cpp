//
// Created by root on 2024/3/13.
//

#include <valarray>
#include "vec.h"

// Constructor
[[maybe_unused]] Vec::Vec(Point direction, double len) {
    this->tail = direction;
    setLength(len);
}
// Constructor, but make head at (0, 0, 0)
Vec::Vec(Point origin, Point direction) {
    const double v[3] = {direction.x - origin.x, direction.y - origin.y, direction.z - origin.z};
    this->tail = Point(v[0], v[1], v[2]);
    this->updateLength();
}

// Constructor
Vec::Vec(Point direction) {
    this->tail = direction;
    this->updateLength();
}
//Constructor
/*[[maybe_unused]] Vec::Vec(Ray& ray) {
    Vec tmp = Vec(ray.getOrigin(), ray.getDirection());
    this->tail = tmp.tail;
    setLength(tmp.getLength());
}*/

void Vec::updateLength() {
    const double v[3] = {tail.x - head.x, tail.y - head.y, tail.z - head.z};
    setLength(sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]));
}

void Vec::setLength(double len) {
    this->length = len;
}

double Vec::getLength() const {
    return this->length;
}



[[maybe_unused]] Vec Vec::getNormalized() const {
    return Vec(Point(tail.x / getLength(), tail.y / getLength(), tail.z / getLength()));
}

// Cross product: self x other
[[maybe_unused]] Vec Vec::cross(const Vec &other) const {
    const double v[3] = {this->tail.y * other.tail.z - this->tail.z * other.tail.y,
                         this->tail.z * other.tail.x - this->tail.x * other.tail.z,
                         this->tail.x * other.tail.y - this->tail.y * other.tail.x};
    return Vec(Point(v[0], v[1], v[2]));
}

// Dot product: self . other
double Vec::dot(const Vec &other) const {
    return this->tail.x * other.tail.x + this->tail.y * other.tail.y + this->tail.z * other.tail.z;
}

/*Point Vec::intersects(const Triangle& tri) const {
    // Implement the Möller-Trumbore intersection algorithm here
    // Return true if the ray intersects the triangle, false otherwise


    // Compute the normal vector of the triangle
    // Compute the distance from the head to the plane of the triangle
    double dist = tri.getNormal().dot(Vec(origin, tri.v0));

    // Check if the ray intersects the plane
    if (dist <= 1e-6) {
        return Point{DBL_MAX, DBL_MAX, DBL_MAX}; // No intersection
    }

    // Compute the intersection point with the plane
    Point intersection = Vec(direction) * dist + origin;

    // Check if the intersection point is inside the triangle
    if (!tri.is_inside_triangle(intersection)) {
        return Point{DBL_MAX, DBL_MAX, DBL_MAX}; // No intersection
    }

    return intersection;
}*/