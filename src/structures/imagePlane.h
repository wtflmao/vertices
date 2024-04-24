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
    Vec OX = Vec(Point{1, 0, 0.2}).getNormalized();

    // this vec indicates which is the positive Y axis of the image plane
    //Vec OY = Vec(Point{0, 1.732, -1}).getNormalized();
    Vec OY = Vec(Point{0, 1, 0.2}).getNormalized();

    // we want the normal of the image plane points to the ground, not the sky
    Vec planeNormal = OX.cross(OY).getTail().getZ() < 0 ? OX.cross(OY).getNormalized() : OY.cross(OX).getNormalized();

    // this is the angle between planeNormal and Vec(0, 0, -1), should be less than 90 degrees
    // i know the length of it must be 1.0, but i like to write its complete form
    double angleToZ = std::acos(planeNormal.getTail().getZ() / planeNormal.getLength());

    // sample points are where ORIGINAL rays are started
    // they all haven't initialized
    std::vector<std::vector<Point>> samplePoints;
    std::vector<std::vector<Pixel *> > toPixel_p;

    int countX = 0, countY = 0;

public:
    [[nodiscard]] std::vector<std::vector<Point> > &getSamplePoints() noexcept;

    std::vector<std::vector<Pixel *> > &getMutSamplePointsPixel() noexcept;

    [[nodiscard]] const Point &getPlaneCenter() const noexcept;

    [[nodiscard]] const Vec &getPlaneNormal() const noexcept;

    [[nodiscard]] const Vec &getOX() const noexcept;

    [[nodiscard]] const Vec &getOY() const noexcept;

    [[nodiscard]] int getCountX() const noexcept;

    [[nodiscard]] int getCountY() const noexcept;

    [[nodiscard]] double getAngleToZ() const noexcept;

    ImagePlane &setPlaneCenter(const Point &p) noexcept;

    ImagePlane &setPlaneNormal(const Vec &v) noexcept;

    ImagePlane &setOX(const Vec &v) noexcept;

    ImagePlane &setOY(const Vec &v) noexcept;

    ImagePlane& setAngleToZ(double angleToZ) noexcept;

    ImagePlane& updateNormalAndAngleToZ() noexcept;

    explicit ImagePlane(const std::shared_ptr<std::vector<std::vector<Pixel> > > &a);

    ImagePlane &buildImagePlane(const std::shared_ptr<std::vector<std::vector<Pixel> > > &a) noexcept;

    [[deprecated]] [[nodiscard]] std::vector<Ray> *shootRays(int N = 1) const noexcept;
};

#endif //VERTICES_IMAGEPLANE_H
