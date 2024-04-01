//
// Created by root on 2024/3/27.
//

/* Copyright 2024 wtflmao. All Rights Reserved.
 *
 * Distributed under MIT license.
 * See file LICENSE/LICENSE.MIT.md or copy at https://opensource.org/license/mit
 */

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
#include <memory>
#include <cctype>

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

enum class BRDFType {
    Default, Open, Closed
};

class BRDF {
public:

    // type suggests what this brdf type is
    // 0: undefined  1: ground(like grassland, desert, plateau)  2: objects (like ball, chair, car, rock)
    BRDFType type = BRDFType::Default;

    explicit BRDF(bool isOpen) noexcept;

    virtual ~BRDF() = default;

};

class OpenBRDF : public BRDF {
public:

// for open mesh, brdf data holder
    std::map<std::array<int, 2>, std::vector<std::vector<short>>> *valMap = new std::map<std::array<int, 2>, std::vector<std::vector<short>>>();

    int MODIS_HDF_DATA_DIM_X = 3000;
    int MODIS_HDF_DATA_DIM_Y = 3000;

    void OpenBRDFInsert(const char *pathToDataset, std::array<int, 2> band);

    OpenBRDF(const char *pathToDataset, std::array<int, 2> band) noexcept;

    OpenBRDF() noexcept;
};

class ClosedBRDF : public BRDF {
public:
// for closed mesh, brdf data holder
    std::map<std::tuple<short, short, short, short>, std::tuple<float, float, float>> *RGBVal = new std::map<std::tuple<short, short, short, short>, std::tuple<float, float, float>>();

    [[nodiscard]] std::tuple<float, float, float>
    getBRDF(double theta_in, double phi_in, double theta_out, double phi_out) const;

    void ClosedBRDFInsert(const char *pathToDataset);

    explicit ClosedBRDF(const char *pathToDataset) noexcept;

    ClosedBRDF() noexcept;

};
#endif //VERTICES_BRDF_H
