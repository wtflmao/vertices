//
// Created by root on 24-3-26.
//

/* Copyright 2024 wtflmao. All Rights Reserved.
 *
 * Distributed under MIT license.
 * See file LICENSE/LICENSE.MIT.md or copy at https://opensource.org/license/mit
 */

#include "coordTransform.h"


void CoordTransform::coordTransformTest() noexcept {
    auto mat = tfg.pushTransForm("ground", "camera", tbg);
    std::vector<Eigen::Vector3d> points;
    for (int i = 0; i < 3; i++) {
        points.push_back({0.0 + i / 10.0, 0.4 + i / 10.0, 0.0 + i / 100.0});
        printf("%d(%f,%f,%f),\n", i, points[i].x(), points[i].y(), points[i].z());
    }
    puts("");
    tfg.getTransWithPointCloud("camera", points, "ground");
    for (int i = 0; i < 3; i++) {
        printf("%d(%f, %f, %f),\n", i, points[i].x(), points[i].y(), points[i].z());
        if (points[i].z() + -(float) (i + 1) / 1000.0 > 1e-6) {
            //fprintf(stderr, "NOT OK %lf, %lf\a\n", points[i].z(), (float)(i + 1) / 1000.0);
        }
    }
}

CoordTransform::CoordTransform() noexcept {
    tfg.pushTransForm(GROUND, CAMERA, tbg);
    tfg.pushTransForm(CAMERA, IMAGE, tcg);
}

void CoordTransform::gndToCam(const Point &gnd, Point &cam) noexcept {
    std::vector<Eigen::Vector3d> gnd_v3d;
    gnd_v3d.emplace_back(gnd.getX(), gnd.getY(), gnd.getZ());
    tfg.getTransWithPointCloud(GROUND, gnd_v3d, CAMERA);
    cam.setX(gnd_v3d.at(0).x())
            .setY(gnd_v3d.at(0).y())
            .setZ(gnd_v3d.at(0).z());
}

void CoordTransform::camToGnd(const Point &cam, Point &gnd) noexcept {
    std::vector<Eigen::Vector3d> cam_v3d;
    cam_v3d.emplace_back(cam.getX(), cam.getY(), cam.getZ());
    tfg.getTransWithPointCloud(CAMERA, cam_v3d, GROUND);
    gnd.setX(cam_v3d.at(0).x())
            .setY(cam_v3d.at(0).y())
            .setZ(cam_v3d.at(0).z());
}

[[deprecated]] void CoordTransform::imgToGnd(const Point &img, Point &gnd) noexcept {
    std::vector<Eigen::Vector3d> img_v3d;
    img_v3d.emplace_back(img.getX(), img.getY(), img.getZ());
    tfg.getTransWithPointCloud(IMAGE, img_v3d, GROUND);
    gnd.setX(img_v3d.at(0).x())
            .setY(img_v3d.at(0).y())
            .setZ(img_v3d.at(0).z());
}

[[deprecated]] void CoordTransform::gndToImg(const Point &gnd, Point &img) noexcept {
    std::vector<Eigen::Vector3d> gnd_v3d;
    gnd_v3d.emplace_back(gnd.getX(), gnd.getY(), gnd.getZ());
    tfg.getTransWithPointCloud(GROUND, gnd_v3d, IMAGE);
    img.setX(gnd_v3d.at(0).x())
            .setY(gnd_v3d.at(0).y())
            .setZ(gnd_v3d.at(0).z());
}

// the cam should belongs to camera coord, not ground coord!
// img is the center of 像元, belongs to image coord, not ground coord, nor camera coord
void CoordTransform::camToImg(const Point &cam, Point &img) noexcept {
    const auto &center = CENTER_OF_CAMERA_IN_GND;
    const double distToCamCenterX = std::fabs(cam.getX() - center.at(0));
    const double distToCamCenterY = std::fabs(cam.getY() - center.at(1));
    img.setZ(cam.getZ() - CAM_IMG_DISTANCE)
            .setX(distToCamCenterX * picElemX * IMG_ZOOM_FACTOR / 2.0)
            .setY(distToCamCenterY * picElemY * IMG_ZOOM_FACTOR / 2.0);
}

void CoordTransform::imgToCam(const Point &img, Point &cam) noexcept {
    const auto &center = CENTER_OF_CAMERA_IN_GND;
    cam.setZ(img.getZ() + CAM_IMG_DISTANCE)
            .setX(std::sqrt(
                    (img.getX() * 2.0 / picElemX / IMG_ZOOM_FACTOR) * (img.getX() * 2.0 / picElemX / IMG_ZOOM_FACTOR) +
                    center.at(0) * center.at(0)))
            .setY(std::sqrt(
                    (img.getY() * 2.0 / picElemY / IMG_ZOOM_FACTOR) * (img.getY() * 2.0 / picElemY / IMG_ZOOM_FACTOR) +
                    center.at(1) * center.at(1)));
}
