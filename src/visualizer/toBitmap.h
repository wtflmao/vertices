//
// Created by root on 24-4-19.
//

#ifndef VERTICES_TOBITMAP_H
#define VERTICES_TOBITMAP_H

#include "../../lib/BitmapPlusPlus/BitmapPlusPlus.hpp"
#include "../common.h"
#include <string>
#include <chrono>

static bmp::Pixel random_color() {
    // using 8-bit-color per channel in RGB mode
    return bmp::Pixel{static_cast<unsigned char>(randab(0, 255)), static_cast<unsigned char>(randab(0, 255)), static_cast<unsigned char>(randab(0, 255))};
}

class ToBitmap {
private:
    bmp::Bitmap* image = nullptr;
    int resX;
    int resY;

public:
    ToBitmap(int resolutionX, int resolutionY) noexcept;

    ToBitmap(std::size_t resolutionX, std::size_t resolutionY) noexcept;

    [[nodiscard]] int getResolutionX() const noexcept;

    [[nodiscard]] int getResolutionY() const noexcept;

    ToBitmap& setResolutionX(int x) noexcept;

    ToBitmap& setResolutionY(int y) noexcept;

    void setPixel(int x, int y, std::uint8_t color) noexcept;

    ToBitmap& fillWithRandom() noexcept;

    ToBitmap& fillWithZebra() noexcept;

    ToBitmap& fillWithAColor(std::uint8_t color) noexcept;

    [[nodiscard]] std::string saveToTmpDir(const std::string &timeStr, const std::string &additionalInfo) const noexcept;

    [[nodiscard]] const bmp::Bitmap &getImage() const noexcept;

    [[nodiscard]] bmp::Bitmap &getMutImage() const noexcept;
};



#endif //VERTICES_TOBITMAP_H
