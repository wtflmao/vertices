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

#include "pixel.h"
#include "../common.h"
//#include <pair>

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
    // they all haven't initialized
    std::vector<std::vector<Point>> samplePoints;
    std::vector<std::vector<std::shared_ptr<Pixel> > > toPixel_p;

    int countX = 0, countY = 0;

public:
    [[nodiscard]] std::vector<std::vector<Point> > &getSamplePoints() noexcept;

    std::vector<std::vector<std::shared_ptr<Pixel> > > getSamplePointsPixel() noexcept;

    ImagePlane &setAPixel(const std::shared_ptr<Pixel> &p, int x, int y) noexcept;

    [[nodiscard]] const Point &getPlaneCenter() const noexcept;

    [[nodiscard]] const Vec &getPlaneNormal() const noexcept;

    [[nodiscard]] const Vec &getOX() const noexcept;

    [[nodiscard]] const Vec &getOY() const noexcept;

    [[nodiscard]] int getCountX() const noexcept;

    [[nodiscard]] int getCountY() const noexcept;

    ImagePlane &setPlaneCenter(const Point &p) noexcept;

    ImagePlane &setPlaneNormal(const Vec &v) noexcept;

    ImagePlane &setOX(const Vec &v) noexcept;

    ImagePlane &setOY(const Vec &v) noexcept;

    ImagePlane();

    ImagePlane &buildImagePlane() noexcept;

    [[nodiscard]] std::vector<Ray> *shootRays(int N = 1) const noexcept;
};

#endif //VERTICES_IMAGEPLANE_H
