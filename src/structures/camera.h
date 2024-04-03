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

public:

    std::array<double, spectralBands> sunlightSpectrum = {};

    // todo: deprecate spatialPosition and use CAMERA center
    // camera position
    // this should defines the pixel[0, 0]'s position, also be a flat platform
    std::array<Point, 2> spatialPosition = {Point(0.0, 0.0, 0.0), Point(0.0, 0.0, 0.0)};
    // camera direction
    // should be align with the normal of the platform
    Vec platformDirection = Vec(Point(0.0, 0.0, -1.0));

    // sensor spectral response
    std::shared_ptr<std::array<std::array<Pixel, resolutionY>, resolutionX> > spectralResp_p;

    // coord transform from ground to camera


    // PSF core function
    void PSF();

    void shootRaysOut(const Vec &rayDirection, std::vector<Ray> *rays);

    Ray shootRay(const Vec &rayDirection, int cnt);

    Camera();

    void buildSunlightSpectrum();

    void addRaySpectrumResp(Ray &ray) noexcept;

    Ray shootRayRandom(int cnt);

    std::vector<Ray> shootRaysRandom(int num);

    double realOverlappingRatio(const Point &p1, const Point &p2);

    Point findTheClosestPixel(const Point &source);
};


#endif //VERTICES_CAMERA_H
