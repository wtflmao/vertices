//
// Created by root on 2024/3/27.
//

#include "BRDF.h"

/* Copyright 2024 wtflmao. All Rights Reserved.
 *
 * Distributed under MIT license.
 * See file LICENSE/LICENSE.MIT.md or copy at https://opensource.org/license/mit
 */

OpenBRDF::OpenBRDF() noexcept: BRDF(true) {
}

ClosedBRDF::ClosedBRDF() noexcept: BRDF(false) {
    //availThetaIn = std::make_shared<std::set<short>>();
    //availPhiIn = std::make_shared<std::set<short>>();
    //availThetaOut = std::make_shared<std::set<short>>();
    //availPhiOut = std::make_shared<std::set<short>>();
}

auto getFileSize(const std::shared_ptr<FILE *> &fp_s) {
    auto *fp = *fp_s;
    fseek(fp, 0, SEEK_END);
    size_t size = 0;
    // get the size of the file
#ifdef _WIN32
    _fseeki64(fp, 0, SEEK_END);
    size = _ftelli64(fp);
#elif __unix__ || __unix || __APPLE__ || __MACH__ || __linux__
    fseeko(fp, 0, SEEK_END);
                size = ftello(fp);
#else
                fseeko64(fp, 0, SEEK_END);
                size = ftello64(fp);
#endif

    fseek(fp, 0, SEEK_SET);
    return size;
}

void OpenBRDF::OpenBRDFInsert(const char *pathToDataset, std::array<int, 2> band) {
    //valMap = new std::map<std::array<int, 2>, std::vector<std::vector<short>>>();
    // always remember to init your variable
    valMap->insert({band, {{static_cast<short>(0)}}});
    auto &val = valMap->at(band);
    val.reserve(MODIS_HDF_DATA_DIM_X);
    FILE *fp = nullptr;

#ifdef _WIN32
    if (fopen_s(&fp, pathToDataset, "rt") != 0) {
        fprintf(stderr, "Cannot open %s\a\n", pathToDataset);
        exit(4);
    }
#else
    fp = fopen(pathToDataset, "rt");
    if (fp == nullptr) {
        fprintf(stderr, "Cannot open %s\a\n", pathToDataset);
        exit(4);
    }
#endif

    fprintf(stdout, "BRDF file size: %zu bytes\n", getFileSize(std::make_shared<FILE *>(fp)));
    fseek(fp, 0, SEEK_SET);

    char line[BUFFER_SIZE_FOR_OPEN_MESH], *line_p = nullptr;
    // i for latitude(+ for N, - for S), j_min for longitude(+ for E, - for W)
    int i = 0, i_max = std::numeric_limits<int>::min(), j_min = std::numeric_limits<int>::max();
    while (fgets(line, BUFFER_SIZE_FOR_OPEN_MESH, fp) != nullptr) {
        auto &temp = val.at(i);
        short t;
        int j_t = 0, n = 1;
        temp.reserve(MODIS_HDF_DATA_DIM_Y);
        line_p = line;
#ifdef _WIN32
        while (sscanf_s(line_p, "%hd%n", &t, &n) == 1) {
            if (n == 0) {
                break;
            }
            temp.push_back(t);
            // move the line pointer to after the first found digit
            line_p += n;
            j_t++;
        }
#else
        while (sscanf(line_p, "%hd%n", &t, &n) == 1) {
            if (n == 0) {
                break;
            }
            temp.push_back(t);
            // move the line pointer to after the first found digit
            line_p += n;
            j_t++;
        }
#endif
        if (j_t < j_min) j_min = j_t;
        i++;
        if (i > i_max) i_max = i;
        val.emplace_back();
    }
    val.pop_back();
    MODIS_HDF_DATA_DIM_X = i;
    MODIS_HDF_DATA_DIM_Y = j_min;
    printf("DIMX: %d   DIMY: %d\n", MODIS_HDF_DATA_DIM_X, MODIS_HDF_DATA_DIM_Y);
    fclose(fp);
}

BRDF::BRDF(bool isOpen) noexcept {
    if (isOpen) {
        type = BRDFType::Open;
    } else {
        type = BRDFType::Closed;
    }
}

OpenBRDF::OpenBRDF(const char *pathToDataset, std::array<int, 2> band) noexcept: BRDF(true) {
    OpenBRDFInsert(pathToDataset, band);
}

/*
// all four angles in radians, both in_angles are in [0, 0.5*pi], both out_angles are in [0, 2*pi]
std::tuple<float, float, float>
ClosedBRDF::getBRDF(double theta_in, double phi_in, double theta_out, double phi_out) const {
    return RGBVal->at(std::make_tuple(
            static_cast<short>(std::round(theta_in * 10000)),
            static_cast<short>(std::round(phi_in * 10000)),
            static_cast<short>(std::round(theta_out * 10000)),
            static_cast<short>(std::round(phi_out * 10000))
    ));
}
*/

// Copyright 2005 Mitsubishi Electric Research Laboratories All Rights Reserved.

// Permission to use, copy and modify this software and its documentation without
// fee for educational, research and non-profit purposes, is hereby granted, provided
// that the above copyright notice and the following three paragraphs appear in all copies.

// To request permission to incorporate this software into commercial products contact:
// Vice President of Marketing and Business Development;
// Mitsubishi Electric Research Laboratories (MERL), 201 Broadway, Cambridge, MA 02139 or
// <license@merl.com>.

// IN NO EVENT SHALL MERL BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL,
// OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND
// ITS DOCUMENTATION, EVEN IF MERL HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

// MERL SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED
// HEREUNDER IS ON AN "AS IS" BASIS, AND MERL HAS NO OBLIGATIONS TO PROVIDE MAINTENANCE, SUPPORT,
// UPDATES, ENHANCEMENTS OR MODIFICATIONS.

// cross product of two vectors
void cross_product(const double *v1, const double *v2, double *out) {
    out[0] = v1[1] * v2[2] - v1[2] * v2[1];
    out[1] = v1[2] * v2[0] - v1[0] * v2[2];
    out[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

// normalize vector
void normalize(double *v) {
    // normalize
    double len = std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    v[0] = v[0] / len;
    v[1] = v[1] / len;
    v[2] = v[2] / len;
}

// rotate vector along one axis
void rotate_vector(double *vector, double *axis, double angle, double *out) {
    double temp;
    double cross[3];
    double cos_ang = std::cos(angle);
    double sin_ang = std::sin(angle);

    out[0] = vector[0] * cos_ang;
    out[1] = vector[1] * cos_ang;
    out[2] = vector[2] * cos_ang;

    temp = axis[0] * vector[0] + axis[1] * vector[1] + axis[2] * vector[2];
    temp = temp * (1.0 - cos_ang);

    out[0] += axis[0] * temp;
    out[1] += axis[1] * temp;
    out[2] += axis[2] * temp;

    cross_product(axis, vector, cross);

    out[0] += cross[0] * sin_ang;
    out[1] += cross[1] * sin_ang;
    out[2] += cross[2] * sin_ang;
}


// convert standard coordinates to half vector/difference vector coordinates
void std_coords_to_half_diff_coords(double theta_in, double fi_in, double theta_out, double fi_out,
                                    double &theta_half, double &fi_half, double &theta_diff, double &fi_diff) {

    // compute in vector
    double in_vec_z = std::cos(theta_in);
    double proj_in_vec = std::sin(theta_in);
    double in_vec_x = proj_in_vec * std::cos(fi_in);
    double in_vec_y = proj_in_vec * std::sin(fi_in);
    double in[3] = {in_vec_x, in_vec_y, in_vec_z};
    normalize(in);


    // compute out vector
    double out_vec_z = std::cos(theta_out);
    double proj_out_vec = std::sin(theta_out);
    double out_vec_x = proj_out_vec * std::cos(fi_out);
    double out_vec_y = proj_out_vec * std::sin(fi_out);
    double out[3] = {out_vec_x, out_vec_y, out_vec_z};
    normalize(out);


    // compute halfway vector
    double half_x = (in_vec_x + out_vec_x) / 2.0f;
    double half_y = (in_vec_y + out_vec_y) / 2.0f;
    double half_z = (in_vec_z + out_vec_z) / 2.0f;
    double half[3] = {half_x, half_y, half_z};
    normalize(half);

    // compute  theta_half, fi_half
    theta_half = std::acos(half[2]);
    fi_half = std::atan2(half[1], half[0]);


    double bi_normal[3] = {0.0, 1.0, 0.0};
    double normal[3] = {0.0, 0.0, 1.0};
    double temp[3];
    double diff[3];

    // compute diff vector
    rotate_vector(in, normal, -fi_half, temp);
    rotate_vector(temp, bi_normal, -theta_half, diff);

    // compute  theta_diff, fi_diff
    theta_diff = std::acos(diff[2]);
    fi_diff = std::atan2(diff[1], diff[0]);

}


// Lookup theta_half index
// This is a non-linear mapping!
// In:  [0 .. pi/2]
// Out: [0 .. 89]
inline int theta_half_index(double theta_half) {
    if (theta_half <= 0.0)
        return 0;
#if VERTICES_CONFIG_CXX_STANDARD >= 20
    double theta_half_deg = ((theta_half / (std::numbers::pi / 2.0)) * BRDF_SAMPLING_RES_THETA_H);
#elif VERTICES_CONFIG_CXX_STANDARD <= 17
    double theta_half_deg = ((theta_half / (M_PI / 2.0)) * BRDF_SAMPLING_RES_THETA_H);
#endif
    double temp = theta_half_deg * BRDF_SAMPLING_RES_THETA_H;
    temp = std::sqrt(temp);
    int ret_val = (int) temp;
    if (ret_val < 0) ret_val = 0;
    if (ret_val >= BRDF_SAMPLING_RES_THETA_H)
        ret_val = BRDF_SAMPLING_RES_THETA_H - 1;
    return ret_val;
}


// Lookup theta_diff index
// In:  [0 .. pi/2]
// Out: [0 .. 89]
inline int theta_diff_index(double theta_diff) {
#if VERTICES_CONFIG_CXX_STANDARD >= 20
    int tmp = static_cast<int>(theta_diff / (std::numbers::pi * 0.5) * BRDF_SAMPLING_RES_THETA_D);
#elif VERTICES_CONFIG_CXX_STANDARD <= 17
    int tmp = static_cast<int>(theta_diff / (M_PI * 0.5) * BRDF_SAMPLING_RES_THETA_D);
#endif
    if (tmp < 0)
        return 0;
    else if (tmp < BRDF_SAMPLING_RES_THETA_D - 1)
        return tmp;
    else
        return BRDF_SAMPLING_RES_THETA_D - 1;
}


// Lookup phi_diff index
inline int phi_diff_index(double phi_diff) {
    // Because of reciprocity, the BRDF is unchanged under
    // phi_diff -> phi_diff + std::numbers::pi
    if (phi_diff < 0.0)
#if VERTICES_CONFIG_CXX_STANDARD >= 20
        phi_diff += std::numbers::pi;
#elif VERTICES_CONFIG_CXX_STANDARD <= 17
        phi_diff += M_PI;
#endif

    // In: phi_diff in [0 .. pi]
    // Out: tmp in [0 .. 179]
#if VERTICES_CONFIG_CXX_STANDARD >= 20
    int tmp = static_cast<int>(phi_diff / std::numbers::pi * BRDF_SAMPLING_RES_PHI_D / 2);
#elif VERTICES_CONFIG_CXX_STANDARD <= 17
    int tmp = static_cast<int>(phi_diff / M_PI * BRDF_SAMPLING_RES_PHI_D / 2);
#endif
    if (tmp < 0)
        return 0;
    else if (tmp < BRDF_SAMPLING_RES_PHI_D / 2 - 1)
        return tmp;
    else
        return BRDF_SAMPLING_RES_PHI_D / 2 - 1;
}


// Given a pair of incoming/outgoing angles, look up the BRDF.
void lookup_brdf_val(const double *brdf, double theta_in, double fi_in,
                     double theta_out, double fi_out,
                     double &red_val, double &green_val, double &blue_val) {
    // Convert to halfangle / difference angle coordinates
    double theta_half, fi_half, theta_diff, fi_diff;

    std_coords_to_half_diff_coords(theta_in, fi_in, theta_out, fi_out,
                                   theta_half, fi_half, theta_diff, fi_diff);


    // Find index.
    // Note that phi_half is ignored, since isotropic BRDFs are assumed
    int ind = phi_diff_index(fi_diff) +
              theta_diff_index(theta_diff) * BRDF_SAMPLING_RES_PHI_D / 2 +
              theta_half_index(theta_half) * BRDF_SAMPLING_RES_PHI_D / 2 *
              BRDF_SAMPLING_RES_THETA_D;

    red_val = brdf[ind] * RED_SCALE;
    green_val = brdf[ind + BRDF_SAMPLING_RES_THETA_H * BRDF_SAMPLING_RES_THETA_D * BRDF_SAMPLING_RES_PHI_D / 2] *
                GREEN_SCALE;
    blue_val = brdf[ind + BRDF_SAMPLING_RES_THETA_H * BRDF_SAMPLING_RES_THETA_D * BRDF_SAMPLING_RES_PHI_D] * BLUE_SCALE;


    if (red_val < 0.0 || green_val < 0.0 || blue_val < 0.0)
        fprintf(stderr, "Below horizon.\n");

}


// Read BRDF data
bool read_brdf(const char *filename, double *&brdf) {
    FILE *fp = fopen(filename, "rb");
    if (!fp)
        return false;

    //fprintf(stdout, "BRDF file size: %zu bytes\n", getFileSize(std::make_shared<FILE *>(fp)));
    fseek(fp, 0, SEEK_SET);

    int dims[3];
    fread(dims, sizeof(int), 3, fp);
    int n = dims[0] * dims[1] * dims[2];
    if (n != BRDF_SAMPLING_RES_THETA_H *
             BRDF_SAMPLING_RES_THETA_D *
             BRDF_SAMPLING_RES_PHI_D / 2) {
        fprintf(stderr, "Dimensions don't match\n");
        //fclose(fp);
        return false;
    }

    //brdf = (double*) malloc (sizeof(double)*3*n);
    brdf = new double[3 * n];
    fread(brdf, sizeof(double), 3 * n, fp);

    fclose(fp);
    return true;
}
// Copyright 2005 MERL ends here

std::tuple<double, double, double> ClosedBRDF::lookUpBRDF(const char *filename, const double theta_in,
                                                          const double phi_in, const double theta_out,
                                                          const double phi_out) const noexcept {
    double red, green, blue;
    double *brdf;
    // read brdf
    if (!read_brdf(filename, brdf)) {
        fprintf(stderr, "Error reading %s\n", filename);
        exit(1);
    }
    lookup_brdf_val(brdf, theta_in, phi_in, theta_out, phi_out, red, green, blue);
    free(brdf);
    return {red, green, blue};
}

/*
void ClosedBRDF::ClosedBRDFInsert(const char *pathToDataset) {
    const char *filename = pathToDataset;
    double *brdf;

    // read brdf
    if (!read_brdf(filename, brdf)) {
        fprintf(stderr, "Error reading %s\n", filename);
        exit(1);
    }

    // print out a 8x32x8x32 table table of BRDF values
    const int n = 8;
    for (int i = 0; i < n; i++) {
        const double theta_in = i * 0.5 * std::numbers::pi / n;
        for (int j = 0; j < 4 * n; j++) {
            const double phi_in = j * 2.0 * std::numbers::pi / (4 * n);
            for (int k = 0; k < n; k++) {
                const double theta_out = k * 0.5 * std::numbers::pi / n;
                for (int l = 0; l < 4 * n; l++) {
                    const double phi_out = l * 2.0 * std::numbers::pi / (4 * n);
                    double red, green, blue;
                    lookup_brdf_val(brdf, theta_in, phi_in, theta_out, phi_out, red, green, blue);
                    //RGBVal = new std::map<std::tuple<short, short, short, short>, std::tuple<float, float, float>>();

                    const auto theta_in_s = static_cast<short>(std::round(theta_in * 10000));
                    const auto phi_in_s = static_cast<short>(std::round(phi_in * 10000));
                    const auto theta_out_s = static_cast<short>(std::round(theta_out * 10000));
                    const auto phi_out_s = static_cast<short>(std::round(phi_out * 10000));

                    if (theta_in_s >= BOUNDRY_LOW_THETA && theta_in_s <= BOUNDRY_HIGH_THETA)
                        if (theta_out_s >= BOUNDRY_LOW_THETA && theta_out_s <= BOUNDRY_HIGH_THETA)
                            if (phi_in_s >= BOUNDRY_LOW_PHI && phi_in_s <= BOUNDRY_HIGH_PHI)
                                if (phi_out_s >= BOUNDRY_LOW_PHI && phi_out_s <= BOUNDRY_HIGH_PHI) {
                                    RGBVal->insert({std::make_tuple(theta_in_s, phi_in_s, theta_out_s, phi_out_s),
                                        std::make_tuple(static_cast<float>(red), static_cast<float>(green), static_cast<float>(blue))});
                                    availThetaIn->insert(theta_in_s);
                                    availPhiIn->insert(phi_in_s);
                                    availThetaOut->insert(theta_out_s);
                                    availPhiOut->insert(phi_out_s);
                                    //coutLogger->writeInfoEntry(std::to_string(static_cast<float>(red)) + " " + std::to_string(static_cast<float>(green)) + " " + std::to_string(static_cast<float>(blue)));
                                }
                }
            }
        }
    }
    free(brdf);
}
*/

BRDF& BRDF::buildSpectrum(const std::string &a) {
    if (isSpectrumRBuilt && a == "r")
        return *this;
    if (isSpectrumGBuilt && a == "g")
        return *this;
    if (isSpectrumBBuilt && a == "b")
        return *this;
    if (a != "r" && a != "g" && a != "b") {
        coutLogger->writeErrorEntry("Wrong spectrum type: " + a);
        exit(888);
    }
    // sunlight energy distribution should be constant here
    // source: MODTRAN
    // final_height = 0.0, observer height: 0.2, observer zenith angle: 0
    // sun zenith angle: 30, day of year: 173, aztmuth observer to sun: 180
#ifdef _WIN32
    //const std::string path = R"(..\data\)" + a + R"(.csv)";
    const std::string path = R"(..\data\one.csv)";
    FILE *fp = fopen(path.c_str(), "rt");
    if (fp == nullptr) {
        fprintf(stderr, "Cannot open %s\a\n", path.c_str());
        exit(2);
    }
#else
    //const std::string path = R"(./data/)" + a + R"(.csv)";
    const std::string path = R"(./data/one.csv)";
    FILE *fp = fopen(path.c_str(), "rt");
    if (fp == nullptr) {
        fprintf(stderr, "Cannot open %s\a\n", path.c_str());
        exit(2);
    }
#endif
    int waveLength_t = 0;
    double totalRadiance_t = 0.0;
    double maximumTotRad = 0.0;

    std::map<int, double> spectrumMap_t;
#ifdef _WIN32
    while (fscanf_s(fp, "%d,%lf", &waveLength_t, &totalRadiance_t) != EOF) {
        spectrumMap_t[waveLength_t] = totalRadiance_t * 100.0;
    }
#else
    while (fscanf(fp, "%d,%lf", &waveLength_t, &totalRadiance_t) != EOF) {
        spectrumMap_t[waveLength_t] = totalRadiance_t * 100.0;
    }
#endif

    fclose(fp); // close
    for (int i = UPPER_WAVELENGTH; i <= LOWER_WAVELENGTH; i += WAVELENGTH_STEP) {
        if (spectrumMap_t[i] > maximumTotRad) {
            maximumTotRad = spectrumMap_t[i];
        }
    }

    int cnt = 0;
    if (a == "r") {
        for (int i = UPPER_WAVELENGTH; i < LOWER_WAVELENGTH; i += WAVELENGTH_STEP)
            R[cnt++] = spectrumMap_t[i] / maximumTotRad;
        isSpectrumRBuilt = true;
        //std::cout << "Default R spectrum has been built." << std::endl;
    } else if (a == "g") {
        for (int i = UPPER_WAVELENGTH; i < LOWER_WAVELENGTH; i += WAVELENGTH_STEP)
            G[cnt++] = spectrumMap_t[i] / maximumTotRad;
        isSpectrumGBuilt = true;
        //std::cout << "Default G spectrum has been built." << std::endl;
    } else {
        for (int i = UPPER_WAVELENGTH; i < LOWER_WAVELENGTH; i += WAVELENGTH_STEP)
            B[cnt++] = spectrumMap_t[i] / maximumTotRad;
        isSpectrumBBuilt = true;
        //std::cout << "Default B spectrum has been built." << std::endl;
    }
    return *this;
}
