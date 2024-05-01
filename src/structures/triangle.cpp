//
// Created by root on 2024/3/13.
//

#include "triangle.h"

// using member initialization list
Triangle::Triangle(const Point& v0, const Point& v1, const Point& v2) noexcept
    : v0(v0), v1(v1), v2(v2) {
        updateNormalVec();
        computeCentroid();
}

Triangle::Triangle(const Point &v0, const Point &v1, const Point &v2, const Vec &n) noexcept
    : v0(v0), v1(v1), v2(v2) {
        setNormalVec(n);
        computeCentroid();
}

Triangle::Triangle() noexcept {
    computeCentroid();
};

Triangle::Triangle(void* item_p) noexcept {
    pointerToItemMTLDataset = item_p;
    computeCentroid();
};

Triangle &Triangle::setNormalVec(const Vec &n) noexcept {
    normal = n;
    return *this;
}

Triangle &Triangle::updateNormalVec() noexcept {
    normal = Vec(v0, v1).cross(Vec(v0, v2));
    return *this;
}

const Vec & Triangle::getNormal() const noexcept {
    return normal;
}

std::array<std::reference_wrapper<Point>, 3> Triangle::getVertices() noexcept {
    return {std::ref(v0), std::ref(v1), std::ref(v2)};
}

// Function to check if a point is inside a triangle
[[maybe_unused]] bool Triangle::is_inside_triangle(const Point& p) const {
    // Compute the barycentric coordinates of the point
    double alpha = Vec(v0, p).dot(Vec(v0, v1)) / (Vec(v0, v1).dot(Vec(v0, v1)));
    double beta = Vec(v1, p).dot(Vec(v1, v2)) / (Vec(v1, v2).dot(Vec(v1, v2)));
    double gamma = 1 - alpha - beta;

    // Check if the point is inside the triangle
    return (alpha >= -1e-10 && beta >= -1e-10 && gamma >= -1e-10);
}

// calculate centroid
void Triangle::computeCentroid() noexcept {
    double sumX = 0, sumY = 0, sumZ = 0;
    for (const auto &v: getVertices()) {
        sumX += v.get().getX();
        sumY += v.get().getY();
        sumZ += v.get().getZ();
    }
    centroid = {sumX / getVertices().size(), sumY / getVertices().size(), sumZ / getVertices().size()};
}

Point Triangle::getCentroid() const noexcept {
    return {centroid[0], centroid[1], centroid[2]};
}

const Point &Triangle::getV0() const noexcept {
    return v0;
}

const Point &Triangle::getV1() const noexcept {
    return v1;
}

const Point &Triangle::getV2() const noexcept {
    return v2;
}

Point &Triangle::getMutV0() noexcept {
    return v0;
}

Point &Triangle::getMutV1() noexcept {
    return v1;
}

Point &Triangle::getMutV2() noexcept {
    return v2;
}

Triangle &Triangle::setV0(const Point &newV0) noexcept {
    v0 = newV0;
    return *this;
}

Triangle &Triangle::setV1(const Point &newV1) noexcept {
    v1 = newV1;
    return *this;
}

Triangle &Triangle::setV2(const Point &newV2) noexcept {
    v2 = newV2;
    return *this;
}