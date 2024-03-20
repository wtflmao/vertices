//
// Created by root on 24-3-20.
//

#include "camera.h"


void Camera::PSF() {};

std::vector<Ray> Camera::shootRaysOut(const Vec &rayDirection) const {
    std::vector<Ray> rays;

    // generate a ray for every pixel
    for (int i = 0; i < resolutionX; i++) {
        for (int j = 0; j < resolutionY; j++) {
            // calc the spacial position of the pixel
            auto diagnal = Vec(spatialPosition[0], spatialPosition[1]);
            auto sourcePoint = BigO;
            sourcePoint = diagnal * (std::sqrt((i + 0.5) * (i + 0.5) + (j + 0.5) * (j + 0.5)) /
                                     std::sqrt(resolutionX * resolutionX + resolutionY * resolutionY)) +
                          spatialPosition[0];

            // generate the ray
            rays.emplace_back(sourcePoint, rayDirection);
        }
    }
    return rays;
}

Camera::Camera() = default;

void Camera::buildSunlightSpectrum() {
    // sunlight energy distribution should be constant here
    // source: MODTRAN
    // final_height = 0.0, observer height: 0.2, observer zenith angle: 0
    // sun zenith angle: 30, day of year: 173, aztmuth observer to sun: 180
    FILE *fp = fopen(R"(C:\Users\root\CLionProjects\vertices\data\sunlight.csv)", "rt");
    if (fp == nullptr) {
        fprintf(stderr, "Cannot open %s\a\n", R"(C:\PcModWin5\Bin\out_0.000000.csv)");
        exit(2);
    }
    int waveLength_t = 0;
    double totalRadiance_t = 0.0;
    double maximumTotRad = 0.0;

    std::map<int, double> sunlightSpectrumMap_t;
    while (fscanf(fp, "%d,%lf", &waveLength_t, &totalRadiance_t) != EOF) {
        sunlightSpectrumMap_t[waveLength_t] = totalRadiance_t;
        if (totalRadiance_t > maximumTotRad) maximumTotRad = totalRadiance_t;
    }

    int cnt = 0;
    for (int i = UPPER_WAVELENGTH; i <= LOWER_WAVELENGTH; i += WAVELENGTH_STEP) {
        sunlightSpectrum[cnt++] = sunlightSpectrumMap_t[i] / maximumTotRad;
    }
    std::cout << "Default sunlight spectrum has been built." << std::endl;
}

