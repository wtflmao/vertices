//
// Created by root on 24-3-20.
//

#ifndef VERTICES_CAMERA_H
#define VERTICES_CAMERA_H

#include "pixel.h"
#include "vec.h"
#include "ray.h"
#include <valarray>
#include <array>
#include <cstdio>
#include <map>
#include "../basic/coordTransform.h"

constexpr double FACTOR = 1.0;

class Camera {
private:

public:

    // FOVx
    double FOVx = 2 * std::atan(resolutionX * pixelSize * 1e-6 / (2 * focalLength));
    // FOVy
    double FOVy = 2 * std::atan(resolutionY * pixelSize * 1e-6 / (2 * focalLength));

    std::array<double, spectralBands> sunlightSpectrum = {};

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

    Ray shootRayRandom(const Vec &rayDirection, const int cnt);
};


#endif //VERTICES_CAMERA_H
