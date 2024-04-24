//
// Created by root on 24-4-19.
//

/* Copyright 2024 wtflmao. All Rights Reserved.
 *
 * Distributed under MIT license.
 * See file LICENSE/LICENSE.MIT.md or copy at https://opensource.org/license/mit
 */

#ifndef VERTICES_TOBITMAP_H
#define VERTICES_TOBITMAP_H

#include "../../lib/BitmapPlusPlus/BitmapPlusPlus.hpp"
#include "../io/infoAppender.h"
#include "../common.h"
#include <string>
#include <chrono>

class ToBitmap {
private:
    bmp::Bitmap *image = nullptr;
    int resX;
    int resY;

public:
    InfoAppender* infoAppender;

    ToBitmap(int resolutionX, int resolutionY) noexcept;

    ToBitmap(std::size_t resolutionX, std::size_t resolutionY) noexcept;

    [[nodiscard]] int getResolutionX() const noexcept;

    [[nodiscard]] int getResolutionY() const noexcept;

    ToBitmap &setResolutionX(int x) noexcept;

    ToBitmap &setResolutionY(int y) noexcept;

    void setPixel(int x, int y, std::int32_t color) const noexcept;

    void setPixelByChannel(int x, int y, std::uint8_t r, std::uint8_t g, std::uint8_t b) const noexcept;

    ToBitmap &fillWithRandom() noexcept;

    ToBitmap &fillWithZebra() noexcept;

    ToBitmap &fillWithGray() noexcept;

    ToBitmap &fillWithAColor(std::int32_t color) noexcept;

    [[nodiscard]] std::string saveToTmpDir(const std::string &timeStr,
                                           const std::string &additionalInfo) noexcept;

    [[nodiscard]] const bmp::Bitmap &getImage() const noexcept;

    [[nodiscard]] bmp::Bitmap &getMutImage() const noexcept;
};


#endif //VERTICES_TOBITMAP_H
