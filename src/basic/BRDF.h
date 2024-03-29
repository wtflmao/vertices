//
// Created by root on 2024/3/27.
//

#ifndef VERTICES_BRDF_H
#define VERTICES_BRDF_H

#include "../common.h"
#include <cstdio>
#include <cstdlib>
#include <valarray>
#include <numbers>
#include <tuple>
#include <map>
#include <vector>
#include <array>

#define BRDF_SAMPLING_RES_THETA_H       90
#define BRDF_SAMPLING_RES_THETA_D       90
#define BRDF_SAMPLING_RES_PHI_D         360

#define RED_SCALE (1.0/1500.0)
#define GREEN_SCALE (1.15/1500.0)
#define BLUE_SCALE (1.66/1500.0)

constexpr int BUFFER_SIZE_FOR_OPEN_MESH = 20480;
constexpr int BLUE_UPPER = UPPER_WAVELENGTH;
constexpr int BLUE_LOWER = 512;
constexpr int GREEN_UPPER = 512;
constexpr int GREEN_LOWER = 605;
constexpr int RED_UPPER = 605;
// actually the RED_LOWER should be 752 but im lazy to add shortwave infrared BRDF
constexpr int RED_LOWER = LOWER_WAVELENGTH;

class BRDF {
public:
    // type suggests what this brdf type is
    // 0: undefined  1: ground(like grassland, desert, plateau)  2: objects (like ball, chair, car, rock)
    int type = 0;

    // for closed mesh
    std::map<std::tuple<short, short, short, short>, std::tuple<float, float, float>> *RGBVal;

    // for open mesh
    int MODIS_HDF_DATA_DIM_X = 2048;
    int MODIS_HDF_DATA_DIM_Y = 2048;
    std::map<std::array<int, 2>, std::vector<std::vector<short>> *> *valMap;
    //std::vector<std::vector<short>> *val;

    BRDF(const char *pathToDataset, int type, std::array<int, 2> band);

    [[nodiscard]] std::tuple<float, float, float>
    getBRDF(double theta_in, double phi_in, double theta_out, double phi_out) const;

    void closedMeshBRDF(const char *pathToDataset);

    void openMeshBRDF(const char *pathToDataset, std::array<int, 2> band);
};


#endif //VERTICES_BRDF_H
