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


#endif //SRC_COMMON_H