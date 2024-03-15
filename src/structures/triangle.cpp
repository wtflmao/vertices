//
// Created by root on 2024/3/13.
//

#include "triangle.h"

// using member initialization list
Triangle::Triangle(Point v0, Point v1, Point v2): v0(v0), v1(v1), v2(v2) {
    updateNormalVec();
}

void Triangle::setNormalVec(Vec n) {
    normal.tail = n.tail;
}

void Triangle::updateNormalVec() {
    setNormalVec(Vec(v0, v1).cross(Vec(v0, v2)));
}

Vec Triangle::getNormal() const {
    return normal;
}

// Function to check if a point is inside a triangle
bool Triangle::is_inside_triangle(const Point& p) const {
    // Compute the barycentric coordinates of the point
    //double alpha = Vec(p - v0).dot(Vec(v1 - v0)) / (Vec(v1 - v0).dot(Vec( v1 - v0)));
    double alpha = Vec(v0, p).dot(Vec(v0, v1)) / (Vec(v0, v1).dot(Vec(v0, v1)));
    //double beta = Vec(p - v1).dot(Vec(v2 - v1)) / (Vec(v2 - v1).dot(Vec(v2 - v1)));
    double beta = Vec(v1, p).dot(Vec(v1, v2)) / (Vec(v1, v2).dot(Vec(v1, v2)));
    double gamma = 1 - alpha - beta;

    // Check if the point is inside the triangle
    return (alpha >= -1e-6 && beta >= -1e-6 && gamma >= -1e-6);
}