//
// Created by root on 2024/3/27.
//

#ifndef VERTICES_BRDF_H
#define VERTICES_BRDF_H

#include <cstdio>
#include <cstdlib>
#include <valarray>
#include <numbers>
#include <tuple>
#include <map>

#define BRDF_SAMPLING_RES_THETA_H       90
#define BRDF_SAMPLING_RES_THETA_D       90
#define BRDF_SAMPLING_RES_PHI_D         360

#define RED_SCALE (1.0/1500.0)
#define GREEN_SCALE (1.15/1500.0)
#define BLUE_SCALE (1.66/1500.0)

class BRDF {
public:
    std::map<std::tuple<short, short, short, short>, std::tuple<float, float, float>> *RGBVal;

    explicit BRDF(const char *pathToDataset);

    [[nodiscard]] std::tuple<float, float, float>
    getBRDF(double theta_in, double phi_in, double theta_out, double phi_out) const;
};


#endif //VERTICES_BRDF_H
