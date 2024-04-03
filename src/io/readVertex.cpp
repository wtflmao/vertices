//
// Created by root on 2024/3/15.
//

/* Copyright 2024 wtflmao. All Rights Reserved.
 *
 * Distributed under MIT license.
 * See file LICENSE/LICENSE.MIT.md or copy at https://opensource.org/license/mit
 */

#include "readVertex.h"

void processVertex(const char* line, Item& p) {
    /*
     * example:
     * v  -5.5886984 4.9444199 0.1230417
     *       x_axis   y_axis   z_axis
     */

    std::vector<Point>& p_v = p.getMutVertices();
    p_v.emplace_back();
    Point& p_vv = p_v.back();
    auto v_i = std::array<double, 3>({0.0});
    sscanf_s(line, "v %lf %lf %lf", &v_i.at(0), &v_i.at(1), &v_i.at(2));

    auto transformedVertices = std::array<double, 4>({0.0});
    if (p.forwardAxis == 2 && p.upAxis == 3) {
        transformedVertices = matrix4x4DotVector4(MAT23to23, Vec4{v_i.at(0), v_i.at(1), v_i.at(2), 1.0});
    } else if (p.forwardAxis == 6 && p.upAxis == 2) {
        transformedVertices = matrix4x4DotVector4(MAT62to23, Vec4{v_i.at(0), v_i.at(1), v_i.at(2), 1.0});
    } else {
        fprintf(stderr, "Invalid forward or up axis.\a\n");
        exit(9);
    }

    p_vv.setX(transformedVertices.at(0) * p.getScaleFactor().at(0) + p.getCenter().getX())
            .setY(transformedVertices.at(1) * p.getScaleFactor().at(1) + p.getCenter().getY())
            .setZ(transformedVertices.at(2) * p.getScaleFactor().at(2) + p.getCenter().getZ());
}
