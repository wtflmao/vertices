//
// Created by root on 24-4-6.
//

/* Copyright 2024 wtflmao. All Rights Reserved.
 *
 * Distributed under MIT license.
 * See file LICENSE/LICENSE.MIT.md or copy at https://opensource.org/license/mit
 */

#ifndef VERTICES_IMAGEPLANE_H
#define VERTICES_IMAGEPLANE_H

#include "vec.h"
#include "../common.h"

class ImagePlane final {
private:
    // the center of the image plane, gnd coord
    Point planeCenter = {0, 0, CAMERA_HEIGHT - CAM_IMG_DISTANCE};

    // this vec indicates which is the positive X axis of the image plane
    Vec OX = Vec(Point{1, 0, 0}).getNormalized();

    // this vec indicates which is the positive Y axis of the image plane
    Vec OY = Vec(Point{0, 1.732, -1}).getNormalized();

    // we want the normal of the image plane points to the ground, not the sky
    Vec planeNormal = OX.cross(OY).getTail().getZ() < 0 ? OX.cross(OY).getNormalized() : OY.cross(OX).getNormalized();

    // sample points are where ORIGINAL rays are started
    std::vector<std::vector<Point>> samplePoints;

public:
    [[nodiscard]] std::vector<std::vector<Point>>& getSamplePoints() noexcept;

    [[nodiscard]] const Point &getPlaneCenter() const noexcept;

    [[nodiscard]] const Vec &getPlaneNormal() const noexcept;

    [[nodiscard]] const Vec& getOX() const noexcept;

    [[nodiscard]] const Vec& getOY() const noexcept;

    ImagePlane &setSamplePoints(const std::vector<std::vector<Point>> &s) noexcept;

    ImagePlane &setPlaneCenter(const Point& p) noexcept;

    ImagePlane &setPlaneNormal(const Vec& v) noexcept;

    ImagePlane &setOX(const Vec& v) noexcept;

    ImagePlane &setOY(const Vec& v) noexcept;

    ImagePlane();

    ImagePlane& buildImagePlane() noexcept;
};

#endif //VERTICES_IMAGEPLANE_H
