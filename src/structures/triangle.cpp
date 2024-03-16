//
// Created by root on 2024/3/13.
//

#include "triangle.h"

// using member initialization list
Triangle::Triangle(const Point& v0, const Point& v1, const Point& v2) noexcept
    : v0(v0), v1(v1), v2(v2) {
        updateNormalVec();
}

Triangle::Triangle(const Point& v0, const Point& v1, const Point& v2, Vec &n) noexcept
    : v0(v0), v1(v1), v2(v2) {
        setNormalVec(n);
}

Triangle::Triangle() noexcept = default;

void Triangle::setNormalVec(Vec& n) noexcept {
    normal = n;
}

void Triangle::updateNormalVec() noexcept {
    normal = Vec(v0, v1).cross(Vec(v0, v2));
    if (normal.dot(Vec(v0, v1)) < 0) {
        normal *= -1;
    }
}

const Vec & Triangle::getNormal() const noexcept {
    return normal;
}

// Function to check if a point is inside a triangle
[[maybe_unused]] bool Triangle::is_inside_triangle(const Point& p) const {
    // Compute the barycentric coordinates of the point
    double alpha = Vec(v0, p).dot(Vec(v0, v1)) / (Vec(v0, v1).dot(Vec(v0, v1)));
    double beta = Vec(v1, p).dot(Vec(v1, v2)) / (Vec(v1, v2).dot(Vec(v1, v2)));
    double gamma = 1 - alpha - beta;

    // Check if the point is inside the triangle
    return (alpha >= -1e-6 && beta >= -1e-6 && gamma >= -1e-6);
}