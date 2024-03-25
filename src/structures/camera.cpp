//
// Created by root on 24-3-20.
//

#include "camera.h"

inline void CrossProduct(const double v1[3], const double v2[3], double v3[3]) {
    v3[0] = v1[1] * v2[2] - v2[1] * v1[2];
    v3[1] = v1[2] * v2[0] - v2[2] * v1[0];
    v3[2] = v1[0] * v2[1] - v2[0] * v1[1];
}

inline void Normalize(double v[3]) {
    const double len = std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    if (std::abs(len) < 1e-9) {
        // handle error
        std::cout << "Error: Normalizing zero vector.\a" << std::endl;
    }
    v[0] /= len;
    v[1] /= len;
    v[2] /= len;
}

inline void InverseMatrix(double matrix[3][3], double inverse[3][3]) {
    const double det = matrix[0][0] * (matrix[1][1] * matrix[2][2] - matrix[2][1] * matrix[1][2])
                       - matrix[0][1] * (matrix[1][0] * matrix[2][2] - matrix[1][2] * matrix[2][0])
                       + matrix[0][2] * (matrix[1][0] * matrix[2][1] - matrix[1][1] * matrix[2][0]);

    if (std::abs(det) < 1e-9) {
        // throw error message or handle as necessary
        std::cout << "Error: Matrix is not invertible.\a" << std::endl;
        exit(0);
    }

    inverse[0][0] = (matrix[1][1] * matrix[2][2] - matrix[2][1] * matrix[1][2]) / det;
    inverse[0][1] = -(matrix[0][1] * matrix[2][2] - matrix[0][2] * matrix[2][1]) / det;
    inverse[0][2] = (matrix[0][1] * matrix[1][2] - matrix[0][2] * matrix[1][1]) / det;
    inverse[1][0] = -(matrix[1][0] * matrix[2][2] - matrix[1][2] * matrix[2][0]) / det;
    inverse[1][1] = (matrix[0][0] * matrix[2][2] - matrix[0][2] * matrix[2][0]) / det;
    inverse[1][2] = -(matrix[0][0] * matrix[1][2] - matrix[0][2] * matrix[1][0]) / det;
    inverse[2][0] = (matrix[1][0] * matrix[2][1] - matrix[2][0] * matrix[1][1]) / det;
    inverse[2][1] = -(matrix[0][0] * matrix[2][1] - matrix[0][1] * matrix[2][0]) / det;
    inverse[2][2] = (matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0]) / det;
}

// camera coord system, origin point is the camera rectangle's the most left and up point
// its x' axis and y' axis are its two egdes, z' axis comes from cross product(right-handed)
inline void groundToCamera(const Point &A, const Point &B, const Point &ori, Point &ret) {
    double dx = A.x, dy = A.y, dz = A.z;

    // construct the rotate matrix
    double x_axis[3] = {B.x - A.x, B.y - A.y, B.z - A.z}, z_axis[3] = {0, 0, 1}, y_axis[3] = {0};

    // y' axis equals to x' cross z
    CrossProduct(x_axis, z_axis, y_axis);
    Normalize(y_axis);

    // x' is y' cross z
    double x_axis_prime[3];
    CrossProduct(y_axis, z_axis, x_axis_prime);

    // rotate matrix
    double rotation[3][3] = {
        {x_axis_prime[0], y_axis[0], z_axis[0]},
        {x_axis_prime[1], y_axis[1], z_axis[1]},
        {x_axis_prime[2], y_axis[2], z_axis[2]}
    };

    // here's a coord in the camera platform coord system
    double x = ori.x, y = ori.y, z = ori.z;

    // pan the point
    x -= dx;
    y -= dy;
    z -= dz;

    // rotate to the camera surface coordinate system
    double x_prime = x * rotation[0][0] + y * rotation[0][1] + z * rotation[0][2];
    double y_prime = x * rotation[1][0] + y * rotation[1][1] + z * rotation[1][2];
    double z_prime = x * rotation[2][0] + y * rotation[2][1] + z * rotation[2][2];

    ret.x = x_prime * FACTOR;
    ret.y = y_prime * FACTOR;
    ret.z = z_prime * FACTOR;
}

inline void cameraToGround(const Point &A, const Point &B, const Point &ori, Point &ret) {
    double dx = A.x, dy = A.y, dz = A.z;

    // construct the rotate matrix
    double x_axis[3] = {B.x - A.x, B.y - A.y, B.z - A.z}, z_axis[3] = {0, 0, 1}, y_axis[3] = {0};

    // y' axis equals to x' cross z
    CrossProduct(x_axis, z_axis, y_axis);
    Normalize(y_axis);

    // x' is y' cross z
    double x_axis_prime[3];
    CrossProduct(y_axis, z_axis, x_axis_prime);

    // rotate matrix
    double rotation[3][3] = {
        {x_axis_prime[0], y_axis[0], z_axis[0]},
        {x_axis_prime[1], y_axis[1], z_axis[1]},
        {x_axis_prime[2], y_axis[2], z_axis[2]}
    };

    // calc the inverse mat of rotate mat
    double inverse[3][3] = {0};
    InverseMatrix(rotation, inverse);

    // here's a coord in the camera platform coord system
    double x_prime = ori.x / FACTOR, y_prime = ori.y / FACTOR, z_prime = ori.z / FACTOR;

    // rotate to the camera surface coordinate system
    double x = x_prime * inverse[0][0] + y_prime * inverse[0][1] + z_prime * inverse[0][2];
    double y = x_prime * inverse[1][0] + y_prime * inverse[1][1] + z_prime * inverse[1][2];
    double z = x_prime * inverse[2][0] + y_prime * inverse[2][1] + z_prime * inverse[2][2];


    // then pan back
    x += dx;
    y += dy;
    z += dz;

    ret.x = x;
    ret.y = y;
    ret.z = z;
}


void Camera::PSF() {};

void Camera::shootRaysOut(const Vec &rayDirection, std::vector<Ray> *rays) {
    rays->reserve(resolutionX * resolutionY);

    // generate a ray for every pixel
    for (int cnt = 0; cnt < resolutionX * resolutionY; cnt++) {
        // cameraPixelPoint is a point in camera coordinate system
        auto pixelInCameraCoord = BigO, pixelInGroundCoord = BigO;

        // row == std::ceil(cnt / resolutionX), col == cnt % resolutionX (row and col start from 1)
        pixelInCameraCoord.x = (cnt / resolutionX) * (pixelSize * 1e-6);
        pixelInCameraCoord.y = (cnt % resolutionX) * (pixelSize * 1e-6);
        pixelInCameraCoord.z = 0.0;

        cameraToGround(spatialPosition[0], spatialPosition[1], pixelInCameraCoord, pixelInGroundCoord);

        // generate the ray
        rays->emplace_back(pixelInGroundCoord, rayDirection, sunlightSpectrum, 1);
    }
}

// x and y should starts at 0 and not bigger than resolutionX-1 and resolutionY-1
Point getCoord(const int x, const int y) {
    return {x * pixelSize * 1e-6, y * pixelSize * 1e-6, 0.0};
}

Ray Camera::shootRay(const Vec &rayDirection, const int cnt) {
    if (cnt < 0 && cnt >= resolutionX * resolutionY) {
        return {};
    }

    // generate a ray for the current pixel( given by cnt )
    // cameraPixelPoint is a point in camera coordinate system
    auto pixelInCameraCoord = BigO, pixelInGroundCoord = BigO;

    // row == std::ceil(cnt / resolutionX), col == cnt % resolutionX (row and col start from 1)
    pixelInCameraCoord.x = (cnt / resolutionX) * (pixelSize * 1e-6);
    pixelInCameraCoord.y = (cnt % resolutionX) * (pixelSize * 1e-6);
    pixelInCameraCoord.z = 0.0;

    cameraToGround(spatialPosition[0], spatialPosition[1], pixelInCameraCoord, pixelInGroundCoord);

    // generate the ray
    return {pixelInGroundCoord, rayDirection, sunlightSpectrum, CAMERA_RAY_STARTER_SCATTER_LEVEL};
}

Camera::Camera() = default;

void Camera::buildSunlightSpectrum() {
    // sunlight energy distribution should be constant here
    // source: MODTRAN
    // final_height = 0.0, observer height: 0.2, observer zenith angle: 0
    // sun zenith angle: 30, day of year: 173, aztmuth observer to sun: 180
    FILE *fp = fopen(R"(C:\Users\root\CLionProjects\vertices\data\sunlight.csv)", "rt");
    if (fp == nullptr) {
        fprintf(stderr, "Cannot open %s\a\n", R"(C:\Users\root\CLionProjects\vertices\data\sunlight.csv)");
        exit(2);
    }
    int waveLength_t = 0;
    double totalRadiance_t = 0.0;
    double maximumTotRad = 0.0;

    std::map<int, double> sunlightSpectrumMap_t;
    while (fscanf(fp, "%d,%lf", &waveLength_t, &totalRadiance_t) != EOF) {
        sunlightSpectrumMap_t[waveLength_t] = totalRadiance_t;
    }

    for (int i = UPPER_WAVELENGTH; i <= LOWER_WAVELENGTH; i += WAVELENGTH_STEP) {
        if (sunlightSpectrumMap_t[i] > maximumTotRad) {
            maximumTotRad = sunlightSpectrumMap_t[i];
        }
    }

    int cnt = 0;
    for (int i = UPPER_WAVELENGTH; i < LOWER_WAVELENGTH; i += WAVELENGTH_STEP) {
        sunlightSpectrum[cnt++] = sunlightSpectrumMap_t[i] / maximumTotRad;
    }
    std::cout << "Default sunlight spectrum has been built." << std::endl;
}

double overlappingFactor(const Point &source, int x, int y) noexcept {
    // todo
    return 1.0;
}

// todo
void Camera::addRaySpectrumResp(Ray &ray) noexcept {
    auto cameraArr = *spectralResp_p;
    //Point thisPixelPoint_camera = BigO;
    //Point thisPixelPoint_ground = ray.getOrigin();
    //groundToCamera(spatialPosition[0], spatialPosition[1], thisPixelPoint_ground, thisPixelPoint_camera);
    //thisPixelPoint_camera.x = std::round(thisPixelPoint_camera.x * 1e6) / 1e6;
    // travese every pixel, this (i,j) pixel is the pixel taht where we store the response
    for (int i = 0; i < resolutionX; i++) {
        for (int j = 0; j < resolutionY; j++) {
            auto &thatPixel = cameraArr[i][j];
            double factor = overlappingFactor(ray.getOrigin(), i, j);
            for (int k = UPPER_WAVELENGTH; k < LOWER_WAVELENGTH; k += WAVELENGTH_STEP) {
                thatPixel.pixelSpectralResp[k] += (ray.intensity_p[k] * factor);
            }
        }
    }
}

