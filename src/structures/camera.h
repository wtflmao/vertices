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
#include <algorithm>
#include <thread>
#include <atomic>
#include <chrono>
#include "../basic/coordTransform.h"
//#include "../../lib/thread_pool/thread_pool.h"

constexpr double FACTOR = 1.0;

class Camera final {
private:
    std::unique_ptr<ImagePlane> imgPlane_u;
    std::shared_ptr<std::vector<std::vector<Pixel> > > pixel2D;

public:

    std::array<double, spectralBands> sunlightSpectrum = {};

    // todo: deprecate spatialPosition and use CAMERA center
    // camera position
    // this should defines the pixel[0, 0]'s position, also be a flat platform
    //[[deprecated]] std::array<Point, 2> spatialPosition = {Point(0.0, 0.0, 0.0), Point(0.0, 0.0, 0.0)};
    // camera direction
    // should be align with the normal of the platform
    //[[deprecated]] Vec platformDirection = Vec(Point(0.0, 0.0, -1.0));

    // sensor spectral response
    //[[deprecated]] std::shared_ptr<std::array<std::array<Pixel, resolutionY>, resolutionX> > spectralResp_p

    // PSF core function
    void PSF();

    //void shootRaysOut(const Vec &rayDirection, std::vector<Ray> *rays);

    //Ray shootRay(const Vec &rayDirection, int cnt);

    Camera();

    void buildSunlightSpectrum();

    //void addRaySpectrumResp(const Ray &ray) const noexcept;

    //Ray shootRayRandom(int cnt);

    //[[nodiscard]] std::vector<Ray> shootRaysRandom() const noexcept;

    //[[nodiscard]] std::vector<Ray> shootRaysRandomFromImgPlate() const noexcept;

    static double realOverlappingRatio(const Point &p1, const Point &p2);

    [[nodiscard]] std::vector<Ray> *shootRays(int multiplier = 1) const noexcept;

    Camera &addSingleRaySpectralRespToPixel(Ray &ray) noexcept;

    [[nodiscard]] std::shared_ptr<std::vector<std::vector<Pixel> > > &getPixel2D() noexcept;

    //[[nodiscard]] Point findTheClosestPixel(const Point &source) const noexcept;

    [[nodiscard]] Point getImagePlaneCenter() const noexcept;
};

struct wrappedRays {
    std::vector<Ray> rays;
    bool done = false;
};

#endif //VERTICES_CAMERA_H
