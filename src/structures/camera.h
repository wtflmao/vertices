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


class Camera {
private:

public:

    // FOV in degrees
    double FOV = 18;

    // focalLength in millimeter
    double focalLength = 100.0;
    // F/# = f/D = 1 / (2 * tan(FOV / 2))
    // when FOV = 18 and focal = 420, F/# = 3.16
    double relativeF = 1 / (2 * std::tan(FOV / 2));
    double apetureD = focalLength * relativeF;

    std::array<double, spectralBands> sunlightSpectrum = {};

    // camera position
    // this should defines the pixel[0, 0]'s position, also be a flat platform
    std::array<Point, 2> spatialPosition = {Point(0.0, 0.0, 0.0), Point(0.0, 0.0, 0.0)};
    // camera direction
    // should be align with the normal of the platform
    Vec platformDirection = Vec(Point(0.0, 0.0, -1.0));

    // sensor spectral response
    std::array<std::array<Pixel, resolutionY>, resolutionX> spectralResp;


    // PSF core function
    void PSF();

    std::vector<Ray> shootRaysOut(const Vec &rayDirection) const;

    Camera();

    void buildSunlightSpectrum();
};


#endif //VERTICES_CAMERA_H
