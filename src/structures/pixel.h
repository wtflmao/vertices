//
// Created by root on 24-3-20.
//

#ifndef VERTICES_PIXEL_H
#define VERTICES_PIXEL_H

#include "../common.h"
#include <array>

class Pixel {
public:

    // spectral response for a single pixel
    // let's say it's ranges from 380 - 780 nm, and we have 100 bands
    std::array<double, spectralBands> pixelSpectralResp;

    // pixel overlap control
    void overlap();

    std::array<double, spectralBands> &getPixelSpectralResp() noexcept;

    void setPixelSpectralResp(const std::array<double, spectralBands> &arr) noexcept;
};


#endif //VERTICES_PIXEL_H
