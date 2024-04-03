//
// Created by root on 2024/3/15.
//

/* Copyright 2024 wtflmao. All Rights Reserved.
 *
 * Distributed under MIT license.
 * See file LICENSE/LICENSE.MIT.md or copy at https://opensource.org/license/mit
 */

#ifndef VERTICES_READVERTEX_H
#define VERTICES_READVERTEX_H

#include "readNewItem.h"
#include "../structures/item.h"

void processVertex(const char*, Item&);

// 1 2 3 for X Y Z axis, 4 5 6 for -X -Y -Z
// im using right-hand coordinate system
constexpr std::array<std::array<double, 4>, 4> MAT23to23 =
{
    {
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    }
};
constexpr std::array<std::array<double, 4>, 4> MAT62to23 =
{
    {
        {1, 0, 0, 0},
        {0, 0, 1, 0},
        {0, -1, 0, 0},
        {0, 0, 0, 1}
    }
};

using Vec4 = std::array<double, 4>;

inline std::array<double, 4> matrix4x4DotVector4(const std::array<std::array<double, 4>, 4> &mat, const Vec4 &v) {
    std::array<double, 4> ret{};
    for (std::size_t i = 0; i < 4; i++) {
        for (std::size_t j = 0; j < 4; j++) {
            ret[i] += mat[i][j] * v[j];
        }
    }
    return ret;
}

#endif //VERTICES_READVERTEX_H