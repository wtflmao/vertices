//
// Created by root on 24-3-20.
//
/* Copyright 2024 wtflmao. All Rights Reserved.
 *
 * Distributed under MIT license.
 * See file LICENSE/LICENSE.MIT.md or copy at https://opensource.org/license/mit
 */

#include "pixel.h"

// d for distance between two pixel's spacial distance
// d is in micron
[[deprecated]] inline double overlapFactor(const double d) {
    // sigma determines how big the overlap is as d grows
    // bigger sigma, more smooth
    // smaller sigma, steeper, goes to 0 quickly
    constexpr double sigma = 0.3;
    // A is a zoom factor
    constexpr double A = 0.99;
    return A * std::exp(-d * d) / (2 * sigma * sigma);
}

[[deprecated]] double Pixel::overlapRatio(int delta_i, int delta_j) {
    // r is in meter, means 单个像素再目标面上的投影半径
    // angle in std::tan() is half of theta, theta is the single pixel's FOV
    double r = CAMERA_HEIGHT * std::tan(pixelSize * 1e-6 / (2 * focalLength));
    // calculate the distance between two pixel, d is in meter
    double d = std::sqrt((delta_i * pixelSize * 1e-6 - 0) * (delta_i * pixelSize * 1e-6 - 0) +
                         (delta_j * pixelSize * 1e-6 - 0) * (delta_j * pixelSize * 1e-6 - 0));
    double overlapping =
            2 * r * r * std::acos((d) / (2 * r)) - 0.5 * std::sqrt((-d + 2 * r) * (-d + 2 * r) - 4 * r * r);
    if (overlapping < 0) {
        // the other pixel has no contribution to the current pixel
        return 0.0;
    } else {
        // 4.2069 is a magic number
        return overlapFactor(d * 4.2069 * 1e2);
    }
};

const std::array<double, spectralBands> &Pixel::getPixelSpectralResp() const noexcept {
    return pixelSpectralResp;
}

std::array<double, spectralBands> &Pixel::getMutPixelSpectralResp() noexcept {
    return pixelSpectralResp;
}

Pixel &Pixel::setPixelSpectralResp(const std::array<double, spectralBands> &arr) noexcept {
    pixelSpectralResp = arr;
    return *this;
}

Pixel &Pixel::setPosInCam(const Point &posInCam_t) noexcept {
    posInCam = posInCam_t;
    return *this;
}

const Point &Pixel::getPosInCam() const noexcept {
    return posInCam;
}

Pixel &Pixel::setPosInGnd(const Point &posInGnd_t) noexcept {
    posInGnd = posInGnd_t;
    return *this;
}

const Point &Pixel::getPosInGnd() const noexcept {
    return posInGnd;
}

Pixel &Pixel::setPosInImg(const Point &posInGnd_t) noexcept {
    posInImg = posInGnd_t;
    return *this;
}

const Point &Pixel::getPosInImg() const noexcept {
    return posInImg;
}

Pixel::Pixel(const Point &posInCam_t) noexcept {
    setPosInCam(posInCam_t);
    pixelSpectralResp = {0};
}

Pixel::Pixel() noexcept {
    pixelSpectralResp = {0};
}

// compute two Vecs that zhengjiao to teh normal vector
void computeCoordinateSystem(const Vec &normal, Vec &tangent, Vec &bitangent) {
    tangent = std::fabs(normal.getTail().getX()) > std::fabs(normal.getTail().getZ())
                  ? Vec(Point(-normal.getTail().getY(), normal.getTail().getX(), 0.0))
                  : Vec(Point(0.0, -normal.getTail().getZ(), normal.getTail().getY()));
    tangent = tangent.getNormalized();
    bitangent = normal.cross(tangent);
}

// 在上半球空间里随机生成一个向量
// FOVs are in degrees
Vec uniformHemisphereDirection(const Vec &normal) {
    // generate two random numbers stored as u and v
    const double u = rand01();
    const double v = rand01();

    // this is a random angle theta, from 0 to pi/2.
    //double theta = std::acos(std::sqrt(1.0 - v));
    constexpr double fovRatio_x = FOVx / 90.0;
    const double theta = std::acos(std::sqrt(1.0 - v * fovRatio_x));

    // this is a random angle phi, ranges from 0 to 2*pi
    //double phi = 2.0 * std::numbers::pi * u;
#if VERTICES_CONFIG_CXX_STANDARD >= 20
    const double phi = FOVy * u * std::numbers::pi / 180.0;
#elif VERTICES_CONFIG_CXX_STANDARD <= 17
    const double phi = FOVy * u * M_PI / 180.0;
#endif
    // get random xyz
    const double x = std::cos(phi) * std::sin(theta);
    const double y = std::sin(phi) * std::sin(theta);
    const double z = std::cos(theta);

    // construct a random direction
    const auto dir = Vec(Point(x, y, z));

    // rotate the normal vector to the upper hemisphere
    auto tangent = Vec(BigO), bitangent = Vec(BigO);
    computeCoordinateSystem(normal, tangent, bitangent);
    return tangent * dir.getTail().getX() + bitangent * dir.getTail().getY() + normal * dir.getTail().getZ();
}

Vec uniformHemisphereDirectionWithCenterOfMonteCarloSpace(const Vec &normal, const Vec &reflectionDirection) {
    while (true) {
        // generate two random numbers stored as u and v
        const double u = rand01();
        const double v = rand01();

        // this is a random angle theta, from 0 to pi/2.
        //double theta = std::acos(std::sqrt(1.0 - v));
        const double fovRatio_x = rand01() < 0.9 ? 25.2 / 90.0 : FOVx / 90.0;
        const double theta = std::acos(std::sqrt(1.0 - v * fovRatio_x));

        // this is a random angle phi, ranges from 0 to 2*pi
        //double phi = 2.0 * std::numbers::pi * u;
#if VERTICES_CONFIG_CXX_STANDARD >= 20
        const double phi = (rand01() > 0.9 ? 25.2 : FOVy) * u * std::numbers::pi / 180.0;
#elif VERTICES_CONFIG_CXX_STANDARD <= 17
        const double phi = (rand01() > 0.9 ? 25.2 : FOVy) * u * M_PI / 180.0;
#endif

        // get random xyz
        const double x = std::cos(phi) * std::sin(theta);
        const double y = std::sin(phi) * std::sin(theta);
        const double z = std::cos(theta);

        // construct a random direction
        const auto dir = Vec(Point(x, y, z));

        // rotate the normal vector to the upper hemisphere
        auto tangent = Vec(BigO), bitangent = Vec(BigO);
        computeCoordinateSystem(reflectionDirection, tangent, bitangent);
        // check if the generated direction is in the normal vec's upper hemisphere
        Vec a = tangent * dir.getTail().getX() + bitangent * dir.getTail().getY() + normal * dir.getTail().getZ();
        if (a.dot(normal) > 0)
            return a;
    }
}

Ray Pixel::shootRayFromPixel(const Vec &directionVec,
                             const std::array<double, spectralBands> &sunlightSpectrum) const noexcept {
    const auto posInGnd = coordTransform->camToGnd(posInCam);
    auto ray = Ray{};

    // build up the current ray
    ray.setOrigin(posInGnd)
            .setAncestor(posInGnd)
            .setIntensity_p(sunlightSpectrum)
            //.setDirection(uniformHemisphereDirection(directionVec));
            .setDirection(Vec{Point{0, -1.732, -1}});

    // RVO happens here, don't worry about value-returning
    return ray;
}

Ray Pixel::shootRayFromPixelFromImgPlate(const Vec &directionVec,
                                         const std::array<double, spectralBands> &sunlightSpectrum,
                                         Pixel *pixel_p) const noexcept {
    auto ray = Ray{};
    // posInGnd's unit is meter but is like xxx*10^-6, we need to multiply it with 10^6 (only for X and Y)
    const auto posInGndAmplified = Point(posInGnd.getX() * 1e6, posInGnd.getY() * 1e6,
                                         (posInGnd.getZ() - CAMERA_HEIGHT) * 1e6 + CAMERA_HEIGHT - CAM_IMG_DISTANCE);
    //const auto posInGndAmplified = Point(posInGnd.getX() * 1e6, posInGnd.getY() * 1e6, posInGnd.getZ());

    // build up the current ray
    ray.setOrigin(posInGndAmplified)
            .setAncestor(posInGndAmplified)
            .setIntensity_p(sunlightSpectrum)
            .setScatteredLevel(CAMERA_RAY_STARTER_SCATTER_LEVEL)
            .setSourcePixel(pixel_p)
            .setSourcePixelPosInGnd(pixel_p->getPosInGnd())
            .setDirection(directionVec);

    // RVO happens here, don't worry about value-returning
    return ray;
}

Pixel &Pixel::addRaySpectralResp(Ray &ray) noexcept {
    auto &resp = getMutPixelSpectralResp();
    for (int i = 0; i < resp.size(); i++)
        resp[i] += ray.getIntensity_p()[i];
    return *this;
}
