//
// Created by root on 2024/3/15.
//

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
    double v_i[3] = {0.0, 0.0, 0.0};
    sscanf(line, "v %lf %lf %lf", &v_i[0], &v_i[1], &v_i[2]);

    p_vv.updatePoint(v_i[0] * p.getScaleFactor()[0] + p.getCenter().x,
                     v_i[1] * p.getScaleFactor()[1] + p.getCenter().y,
                     v_i[2] * p.getScaleFactor()[2] + p.getCenter().z);
}