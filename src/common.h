//
// Created by root on 2024/3/15.
//

/* Copyright 2024 wtflmao. All Rights Reserved.
 *
 * Distributed under MIT license.
 * See file LICENSE/LICENSE.MIT.md or copy at https://opensource.org/license/mit
 */

#ifndef SRC_COMMON_H
#define SRC_COMMON_H

#include "compatibility.h"
#include "logger/coutLogger.h"
#include "logger/stdoutLogger.h"
#include "logger/filesystemLogger.h"
#include <random>
#include <array>
#include <memory>
#include <cstdint>
#include <thread>

inline std::random_device rd;
inline std::mt19937 generator(rd());
inline std::uniform_real_distribution<double> distribution(0.0, 1.0);

inline double rand01() {
    return distribution(generator);
}

inline int randab(const int a, const int b) {
    return std::uniform_int_distribution<std::int32_t>(a, b)(generator);
}

const Logger coutLogger = std::make_shared<CoutLogger>();
const Logger stdoutLogger = std::make_shared<StdoutLogger>();
const Logger fsLogger = std::make_shared<FilesystemLogger>();


// VERTICES_RAY_H
constexpr int SCATTER_RAYS = 3;
constexpr double STOP_LENGTH = 9999;


// VERTICES_NODE_H
constexpr std::size_t BVH_NODE_CHILDREN = 8;
constexpr std::size_t MAX_FACES_PER_BOX = 8;
constexpr std::size_t MAX_DEPTH = 12;
constexpr std::size_t SAMPLINGS = 32;


// VERTICES_PIXEL_H
// 1024 by 768 pixels
inline int resolutionX = 1;
inline int resolutionY = 1;
constexpr int spectralBands = 100;
// all in nanometer, please be in integer
constexpr int UPPER_WAVELENGTH = 380;
constexpr int LOWER_WAVELENGTH = 780;
constexpr int WAVELENGTH_STEP = (LOWER_WAVELENGTH - UPPER_WAVELENGTH) / spectralBands;


// VERTICES_CAMERA_H
// this eight means the first pixel's height
constexpr double CAMERA_HEIGHT = 30;
// pixel distance in micron 6969
constexpr double pixelSize = 6.9;
// GSD in meter
constexpr double gsd = 0.12;
// focalLength in millimeter
constexpr double focalLength = 20;
constexpr int CAMERA_RAY_STARTER_SCATTER_LEVEL = 0;
// FOVx
constexpr double FOVx = 1 * 15.714381; //2 * std::atan(resolutionX * pixelSize * 1e-6 / (2 * focalLength * 1e-3));
// FOVy
constexpr double FOVy = FOVx; //11.818146;//2 * std::atan(resolutionY * pixelSize * 1e-6 / (2 * focalLength * 1e-3));
constexpr std::array<double, 3> CENTER_OF_CAMERA_IN_GND = {0, 0, CAMERA_HEIGHT};


// VERTICES_COORDTRANSFORM_H
// camera @ image
// distance in meters
constexpr double CAM_IMG_DISTANCE = CAMERA_HEIGHT / 60.0;
#if VERTICES_CONFIG_CXX_STANDARD >= 20
const double picElemX = 2.0 * CAM_IMG_DISTANCE * std::tan((FOVx * std::numbers::pi / 180.0) / 2.0);
const double picElemY = 2.0 * CAM_IMG_DISTANCE * std::tan((FOVy * std::numbers::pi / 180.0) / 2.0);
#elif VERTICES_CONFIG_CXX_STANDARD <= 17
const double picElemX = 2.0 * CAM_IMG_DISTANCE * std::tan((FOVx * M_PI / 180.0) / 2.0);
const double picElemY = 2.0 * CAM_IMG_DISTANCE * std::tan((FOVy * M_PI / 180.0) / 2.0);
#endif
const double IMG_ZOOM_FACTOR =
        2e+9 / CAMERA_HEIGHT / static_cast<double>(resolutionX) / static_cast<double>(resolutionY);


// VERTICES_FIELD_H
// in meters
constexpr double FIELD_LENGTH_X = 60.0;
constexpr double FIELD_LENGTH_Y = 60.0;

// VERTICES_TOBITMAP_H
// according to sRGB IEC61966-2.1 standard, returns an integer in range of [0, 255]
inline std::uint8_t RGBToGrayscale(const std::uint8_t r, const std::uint8_t g, const std::uint8_t b) {
    return static_cast<std::uint8_t>(0.2126 * r + 0.7152 * g + 0.0722 * b) & static_cast<std::uint8_t>(0xff);
}

// accepts an integer in range [0, 255], returns an integer in range of [0x0, 0xffffff]
inline auto grayscaleToRGB_int(const std::uint8_t gray) {
    return static_cast<std::int32_t>(static_cast<std::int32_t>(gray) << 16 | static_cast<std::int32_t>(gray) << 8 |
                                     static_cast<std::int32_t>(gray));
}

// accepts an integer in range [0, 255], returns an array of integer that its elements are in range of [0, 255]
inline auto grayscaleToRGB_3array(const std::uint8_t gray) {
    return std::array{gray, gray, gray};
}


// VERTICES_MAIN_H
#ifndef VERTICES_CONFIG_
#define VERTICES_CONFIG_
//#define VERTICES_CONFIG_SINGLE_THREAD_FOR_CAMRAYS
#define VERTICES_CONFIG_MULTI_THREAD_FOR_CAMRAYS_WORKAROUND
const unsigned int HARDWARE_CONCURRENCY_RAW = std::max(std::thread::hardware_concurrency(), 1u);
const unsigned int HARDWARE_CONCURRENCY = HARDWARE_CONCURRENCY_RAW;
#endif

constexpr double mixRatio = 0.04;
constexpr double mixRatioL = mixRatio;
constexpr double mixRatioR = mixRatio;
constexpr double mixRatioU = mixRatio;
constexpr double mixRatioD = mixRatio;


#endif //SRC_COMMON_H
