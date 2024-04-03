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

std::array<double, spectralBands> &Pixel::getPixelSpectralResp() noexcept {
    return pixelSpectralResp;
}

void Pixel::setPixelSpectralResp(const std::array<double, spectralBands> &arr) noexcept {
    pixelSpectralResp = arr;
}

Pixel::Pixel(const Point &posInCam) noexcept: posInCam(posInCam) {

}

