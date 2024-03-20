//
// Created by root on 24-3-20.
//

#include "pixel.h"

void Pixel::overlap() {};

std::array<double, spectralBands> &Pixel::getPixelSpectralResp() noexcept {
    return pixelSpectralResp;
}

void Pixel::setPixelSpectralResp(const std::array<double, spectralBands> &arr) noexcept {
    pixelSpectralResp = arr;
}
