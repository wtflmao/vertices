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

#include <random>
inline std::random_device rd;
inline std::mt19937 generator(rd());
inline std::uniform_real_distribution<double> distribution(0.0, 1.0);

inline double rand01() {
    return distribution(generator);
}


// VERTICES_RAY_H
constexpr int SCATTER_RAYS = 1;
constexpr double STOP_LENGTH = 9999;

// VERTICES_NODE_H
constexpr std::size_t BVH_NODE_CHILDREN = 6;
constexpr std::size_t MAX_FACES_PER_BOX = 32;
constexpr std::size_t MAX_DEPTH = 12;
constexpr std::size_t SAMPLINGS = 64;


// VERTICES_PIXEL_H
// 1024 by 768 pixels
constexpr int resolutionX = 4;
constexpr int resolutionY = 3;
constexpr int spectralBands = 100;
// all in nanometer, please be in integer
constexpr int UPPER_WAVELENGTH = 380;
constexpr int LOWER_WAVELENGTH = 780;
constexpr int WAVELENGTH_STEP = (LOWER_WAVELENGTH - UPPER_WAVELENGTH) / spectralBands;


// VERTICES_CAMERA_H
// this eight means the first pixel's height
constexpr double CAMERA_HEIGHT = 1.2;
// pixel distance in micron 6969
constexpr double pixelSize = 6.9;
// GSD in meter
constexpr double gsd = 0.12;
// focalLength in millimeter
constexpr double focalLength = 20;
constexpr int CAMERA_RAY_STARTER_SCATTER_LEVEL = 1;
// FOVx
constexpr double FOVx = 15.714381;//2 * std::atan(resolutionX * pixelSize * 1e-6 / (2 * focalLength * 1e-3));
// FOVy
constexpr double FOVy = 11.818146;//2 * std::atan(resolutionY * pixelSize * 1e-6 / (2 * focalLength * 1e-3));
#include <array>

constexpr std::array<double, 3> CENTER_OF_CAMERA_IN_GND = {0, 0, CAMERA_HEIGHT};

// VERTICES_COORDTRANSFORM_H
// camera @ image
// distance in meters
constexpr double CAM_IMG_DISTANCE = CAMERA_HEIGHT / 60.0;
const double picElemX = 2.0 * CAM_IMG_DISTANCE * std::tan((FOVx * std::numbers::pi / 180.0) / 2.0);
const double picElemY = 2.0 * CAM_IMG_DISTANCE * std::tan((FOVy * std::numbers::pi / 180.0) / 2.0);
constexpr double IMG_ZOOM_FACTOR =
        2e+9 / CAMERA_HEIGHT / static_cast<double>(resolutionX) / static_cast<double>(resolutionY);
#endif //SRC_COMMON_H
