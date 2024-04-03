//
// Created by root on 24-3-20.
//

/* Copyright 2024 wtflmao. All Rights Reserved.
 *
 * Distributed under MIT license.
 * See file LICENSE/LICENSE.MIT.md or copy at https://opensource.org/license/mit
 */

#ifndef VERTICES_PIXEL_H
#define VERTICES_PIXEL_H

#include "../common.h"
#include "point.h"
#include <array>

class Pixel {
public:
class Pixel final {
    // this pos indicates the location of this pixel in camera coord
    Point posInCam = BigO;

    // spectral response for a single pixel
    // let's say it's ranges from 380 - 780 nm, and we have 100 bands
    std::array<double, spectralBands> pixelSpectralResp;

    std::array<double, spectralBands> &getPixelSpectralResp() noexcept;

    void setPixelSpectralResp(const std::array<double, spectralBands> &arr) noexcept;

    double overlapRatio(int delta_i, int delta_j);

    Pixel(const Point &) noexcept;
};


#endif //VERTICES_PIXEL_H
