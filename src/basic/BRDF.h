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
#include <vector>

#define BRDF_SAMPLING_RES_THETA_H       90
#define BRDF_SAMPLING_RES_THETA_D       90
#define BRDF_SAMPLING_RES_PHI_D         360

#define RED_SCALE (1.0/1500.0)
#define GREEN_SCALE (1.15/1500.0)
#define BLUE_SCALE (1.66/1500.0)

constexpr int BUFFER_SIZE_FOR_OPEN_MESH = 20480;

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
    std::vector<std::vector<short>> *val;

    BRDF(const char *pathToDataset, int type);

    [[nodiscard]] std::tuple<float, float, float>
    getBRDF(double theta_in, double phi_in, double theta_out, double phi_out) const;

    void closedMeshBRDF(const char *pathToDataset);

    void openMeshBRDF(const char *pathToDataset);
};


#endif //VERTICES_BRDF_H
