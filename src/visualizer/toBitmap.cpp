//
// Created by root on 24-4-19.
//

#include "toBitmap.h"

int ToBitmap::getResolutionX() const noexcept {
    return resX;
}

int ToBitmap::getResolutionY() const noexcept {
    return resY;
}

ToBitmap &ToBitmap::setResolutionX(const int x) noexcept {
    resX = x;
    return *this;
}

ToBitmap &ToBitmap::setResolutionY(const int y) noexcept {
    resY = y;
    return *this;
}

ToBitmap::ToBitmap(const int resolutionX, const int resolutionY) noexcept {
    resX = resolutionX;
    resY = resolutionY;
    image = new bmp::Bitmap(resolutionX, resolutionY);
}

ToBitmap::ToBitmap(const std::size_t resolutionX, const std::size_t resolutionY) noexcept {
    resX = static_cast<int>(resolutionX);
    resY = static_cast<int>(resolutionY);
    image = new bmp::Bitmap(resX, resY);
}

void ToBitmap::setPixel(const int x, const int y, const std::int32_t color) const noexcept {
    image->set(x, y, bmp::Pixel{color});
}
void ToBitmap::setPixelByChannel(const int x, const int y, const std::uint8_t r, const std::uint8_t g, const std::uint8_t b) const noexcept {
    image->set(x, y, bmp::Pixel{r, g, b});
}

ToBitmap &ToBitmap::fillWithRandom() noexcept {
    for (auto &pixel: *image) {
        pixel = bmp::Pixel{
            static_cast<unsigned char>(randab(0, 255)), static_cast<unsigned char>(randab(0, 255)),
            static_cast<unsigned char>(randab(0, 255))
        };
    }
    return *this;
}

ToBitmap &ToBitmap::fillWithZebra() noexcept {
    for (auto &pixel: *image) {
        const auto isBlack = static_cast<std::uint8_t>(randab(0, 1) * 255);
        pixel = bmp::Pixel{isBlack, isBlack, isBlack};
    }
    return *this;
}

ToBitmap &ToBitmap::fillWithGray() noexcept {
    for (auto &pixel: *image) {
        auto grayArr = grayscaleToRGB_3array(randab(0, 0xff));
        pixel = bmp::Pixel{grayArr[0], grayArr[1], grayArr[2]};
    }
    return *this;
}

// color is a 24-bit rgb value, e.g. 0x5865F2 is blurple, 0x23272a is not quite black, 0xffffff is white
ToBitmap &ToBitmap::fillWithAColor(const std::int32_t color) noexcept {
    image->clear(bmp::Pixel{color});
    return *this;
}

auto getFormattedTimeWithoutSecond() {
    const auto now = std::chrono::system_clock::now();
    const auto itt = std::chrono::system_clock::to_time_t(now);
    return std::put_time(std::localtime(&itt), "%Y%m%d%H%M");
}

std::string getAPathToNewTempFile2(const std::string &suffixWithDot, const std::string &timeStr = "auto-gen",
                                   const std::string &additionalInfo = "") {
    std::string pathPrefix;
#ifdef _WIN32
    if (const char *temp = std::getenv("TEMP"); temp != nullptr) {
        pathPrefix = temp;
        pathPrefix += R"(\)";
    } else {
        pathPrefix = R"(C:\Windows\temp\)";
    }
#else
    pathPrefix = "/tmp/";
#endif
    std::ostringstream timeBuf;
    if (timeStr == "auto-gen")
        timeBuf << getFormattedTimeWithoutSecond();
    else
        timeBuf << timeStr;
    pathPrefix += ("vertices_output_" + timeBuf.str() + (additionalInfo.empty() ? "" : "_" + additionalInfo) +
                   suffixWithDot);
    return pathPrefix;
}

std::string ToBitmap::saveToTmpDir(const std::string &timeStr, const std::string &additionalInfo = "") const noexcept {
    try {
        auto filepath = getAPathToNewTempFile2(".bmp", timeStr.empty() ? "auto-gen" : timeStr, additionalInfo);
        image->save(filepath);
        return filepath;
    } catch (const bmp::Exception &e) {
        coutLogger->writeErrorEntry(e.what(), 1);
    }
    return "";
}

const bmp::Bitmap &ToBitmap::getImage() const noexcept {
    return *image;
}

bmp::Bitmap &ToBitmap::getMutImage() const noexcept {
    return *image;
}
