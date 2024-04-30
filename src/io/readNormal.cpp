//
// Created by root on 24-4-29.
//

/* Copyright 2024 wtflmao. All Rights Reserved.
 *
 * Distributed under MIT license.
 * See file LICENSE/LICENSE.MIT.md or copy at https://opensource.org/license/mit
 */

#include "readNormal.h"

void processNormal(const char* line, Item& p) {
    /*
     * example:
     * vn  0.1888 0.7551 -0.6278
     *     x_axis y_axis  z_axis
     */

    auto& p_v = p.getMutNormalList();
    auto v_i = std::array<double, 3>{0.0};
#ifdef _WIN32
    sscanf_s(line, "vn %lf %lf %lf", &v_i.at(0), &v_i.at(1), &v_i.at(2));
#else
    sscanf(line, "vn %lf %lf %lf", &v_i.at(0), &v_i.at(1), &v_i.at(2));
#endif
    // since vectors start from (0, 0, 0), so we save them as Points() and there's no need to scale them
    p_v->emplace_back(v_i[0], v_i[1], v_i[2]);
}

