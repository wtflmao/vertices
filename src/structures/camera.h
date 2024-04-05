//
// Created by root on 24-3-20.
//

/* Copyright 2024 wtflmao. All Rights Reserved.
 *
 * Distributed under MIT license.
 * See file LICENSE/LICENSE.MIT.md or copy at https://opensource.org/license/mit
 */

#ifndef VERTICES_CAMERA_H
#define VERTICES_CAMERA_H

#include "pixel.h"
#include "vec.h"
#include "ray.h"
#include "imagePlane.h"
#include <valarray>
#include <array>
#include <cstdio>
#include <map>
#include <ranges>
#include <algorithm>
#include "../basic/coordTransform.h"

constexpr double FACTOR = 1.0;

class Camera final {
private:
    ImagePlane imgPlane = ImagePlane();

public:

    std::array<double, spectralBands> sunlightSpectrum = {};

    // todo: deprecate spatialPosition and use CAMERA center
    // camera position
    // this should defines the pixel[0, 0]'s position, also be a flat platform
    [[deprecated]] std::array<Point, 2> spatialPosition = {Point(0.0, 0.0, 0.0), Point(0.0, 0.0, 0.0)};
    // camera direction
    // should be align with the normal of the platform
    [[deprecated]] Vec platformDirection = Vec(Point(0.0, 0.0, -1.0));

    // sensor spectral response
    std::shared_ptr<std::array<std::array<Pixel, resolutionY>, resolutionX> > spectralResp_p;

    // PSF core function
    void PSF();

    void shootRaysOut(const Vec &rayDirection, std::vector<Ray> *rays);

    Ray shootRay(const Vec &rayDirection, int cnt);

    Camera();

    void buildSunlightSpectrum();

    void addRaySpectrumResp(const Ray &ray) const noexcept;

    //Ray shootRayRandom(int cnt);

    [[nodiscard]] std::vector<Ray> shootRaysRandom() const noexcept;

    [[nodiscard]] std::vector<Ray> shootRaysRandomFromImgPlate() const noexcept;

    static double realOverlappingRatio(const Point &p1, const Point &p2);

    [[nodiscard]] Point findTheClosestPixel(const Point &source) const noexcept;
};


#endif //VERTICES_CAMERA_H
