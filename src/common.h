//
// Created by root on 2024/3/15.
//

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


// VERTICES_NODE_H
constexpr std::size_t BVH_NODE_CHILDREN = 6;
constexpr std::size_t MAX_FACES_PER_BOX = 32;
constexpr std::size_t MAX_DEPTH = 12;
constexpr std::size_t SAMPLINGS = 64;


// VERTICES_PIXEL_H
// 1024 by 768 pixels
constexpr int resolutionX = 8;
constexpr int resolutionY = 6;
constexpr int spectralBands = 100;
// all in nanometer, please be in integer
constexpr int UPPER_WAVELENGTH = 380;
constexpr int LOWER_WAVELENGTH = 780;
constexpr int WAVELENGTH_STEP = (LOWER_WAVELENGTH - UPPER_WAVELENGTH) / spectralBands;


#endif //SRC_COMMON_H
