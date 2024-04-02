//
// Created by root on 24-3-26.
//

/* Copyright 2024 wtflmao. All Rights Reserved.
 *
 * Distributed under MIT license.
 * See file LICENSE/LICENSE.MIT.md or copy at https://opensource.org/license/mit
 */

#ifndef VERTEX_COORDTRANSFORM_H
#define VERTEX_COORDTRANSFORM_H

#define EIGEN_MPL2_ONLY

#include "../../lib/eigen3/Eigen/Dense"
#include "../structures/point.h"
#include "../../lib/transform3d/transforms3d.h"
#include "../common.h"
#include <valarray>
#include <iostream>

constexpr const char *IMAGE = "image";
constexpr const char *CAMERA = "camera";
constexpr const char *GROUND = "ground";

class CoordTransform {
private:
    // ground@camera
    // pitch, yaw, roll are for x, y, and z axis rotation angle respectively
    // units are in degrees, not radians
    // delta_xyz are in meters
    double pitch = 0;
    double yaw = 0;
    double roll = 180;
    double delta_x = 0;
    double delta_y = 0;
    double delta_z = CAMERA_HEIGHT;

    // ground@camera
    const Eigen::Matrix4d tbg = TransForms::ComposeEuler(delta_x, delta_y, delta_z, pitch, yaw, roll);

    // camera @ image platform
    const Eigen::Matrix4d tcg = TransForms::ComposeEuler(0, 0, CAM_IMG_DISTANCE, 0, 0, 0);

public:
    // transform group
    TransFormsGroup tfg;

    CoordTransform() noexcept;

    void coordTransformTest() noexcept;

    void gndToCam(const Point &gnd, Point &cam) noexcept;

    void camToGnd(const Point &cam, Point &gnd) noexcept;

    [[deprecated]] void imgToCam(const Point &img, Point &cam) noexcept;

    [[deprecated]] void camToImg(const Point &cam, Point &img) noexcept;

    [[deprecated]] void imgToGnd(const Point &img, Point &gnd) noexcept;

    [[deprecated]] void gndToImg(const Point &gnd, Point &img) noexcept;
};


#endif //VERTEX_COORDTRANSFORM_H
