//
// Created by root on 24-3-20.
//

/* Copyright 2024 wtflmao. All Rights Reserved.
 *
 * Distributed under MIT license.
 * See file LICENSE/LICENSE.MIT.md or copy at https://opensource.org/license/mit
 */

#include "camera.h"

[[deprecated]] inline void CrossProduct(const double v1[3], const double v2[3], double v3[3]) {
    v3[0] = v1[1] * v2[2] - v2[1] * v1[2];
    v3[1] = v1[2] * v2[0] - v2[2] * v1[0];
    v3[2] = v1[0] * v2[1] - v2[0] * v1[1];
}

[[deprecated]] inline void Normalize(double v[3]) {
    const double len = std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    if (std::abs(len) < 1e-9) {
        // handle error
        std::cout << "Error: Normalizing zero vector.\a" << std::endl;
    }
    v[0] /= len;
    v[1] /= len;
    v[2] /= len;
}

[[deprecated]] inline void InverseMatrix(double matrix[3][3], double inverse[3][3]) {
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
[[deprecated]] inline void groundToCamera(const Point &A, const Point &B, const Point &ori, Point &ret) {
    double dx = A.getX(), dy = A.getY(), dz = A.getZ();

    // construct the rotate matrix
    double x_axis[3] = {B.getX() - A.getX(), B.getY() - A.getY(), B.getZ() - A.getZ()}, z_axis[3] = {0, 0,
                                                                                                     1}, y_axis[3] = {
            0};

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
    double x = ori.getX(), y = ori.getY(), z = ori.getZ();

    // pan the point
    x -= dx;
    y -= dy;
    z -= dz;

    // rotate to the camera surface coordinate system
    double x_prime = x * rotation[0][0] + y * rotation[0][1] + z * rotation[0][2];
    double y_prime = x * rotation[1][0] + y * rotation[1][1] + z * rotation[1][2];
    double z_prime = x * rotation[2][0] + y * rotation[2][1] + z * rotation[2][2];

    ret.setX(x_prime * FACTOR)
            .setY(y_prime * FACTOR)
            .setZ(z_prime * FACTOR);
}

inline void cameraToGround(const Point &A, const Point &B, const Point &ori, Point &ret) {
    double dx = A.getX(), dy = A.getY(), dz = A.getZ();

    // construct the rotate matrix
    double x_axis[3] = {B.getX() - A.getX(), B.getY() - A.getY(), B.getZ() - A.getZ()}, z_axis[3] = {0, 0,
                                                                                                     1}, y_axis[3] = {
            0};

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
    double x_prime = ori.getX() / FACTOR, y_prime = ori.getY() / FACTOR, z_prime = ori.getZ() / FACTOR;

    // rotate to the camera surface coordinate system
    double x = x_prime * inverse[0][0] + y_prime * inverse[0][1] + z_prime * inverse[0][2];
    double y = x_prime * inverse[1][0] + y_prime * inverse[1][1] + z_prime * inverse[1][2];
    double z = x_prime * inverse[2][0] + y_prime * inverse[2][1] + z_prime * inverse[2][2];


    // then pan back
    x += dx;
    y += dy;
    z += dz;

    ret.setX(x).setY(y).setZ(z);
}


void Camera::PSF() {};
/*
[[deprecated]] void Camera::shootRaysOut(const Vec &rayDirection, std::vector<Ray> *rays) {
    rays->reserve(resolutionX * resolutionY);

    // generate a ray for every pixel
    for (int cnt = 0; cnt < resolutionX * resolutionY; cnt++) {
        // cameraPixelPoint is a point in camera coordinate system
        auto pixelInCameraCoord = BigO, pixelInGroundCoord = BigO;

        // row == std::ceil(cnt / resolutionX), col == cnt % resolutionX (row and col start from 1)
        pixelInCameraCoord.setX((cnt / resolutionX) * (pixelSize * 1e-6))
                .setY((cnt % resolutionX) * (pixelSize * 1e-6))
                .setZ(0.0);

        cameraToGround(spatialPosition[0], spatialPosition[1], pixelInCameraCoord, pixelInGroundCoord);

        // generate the ray
        rays->emplace_back(pixelInGroundCoord, rayDirection, sunlightSpectrum, 1);
    }
}

// x and y should starts at 0 and not bigger than resolutionX-1 and resolutionY-1
[[deprecated]] Point getCoord(const int x, const int y) {
    return {x * pixelSize * 1e-6, y * pixelSize * 1e-6, 0.0};
}

[[deprecated]] Ray Camera::shootRay(const Vec &rayDirection, const int cnt) {
    if (cnt < 0 || cnt >= resolutionX * resolutionY) {
        return {};
    }

    // generate a ray for the current pixel( given by cnt )
    // cameraPixelPoint is a point in camera coordinate system
    auto pixelInCameraCoord = BigO, pixelInGroundCoord = BigO;

    // row == std::ceil(cnt / resolutionX), col == cnt % resolutionX (row and col start from 1)
    pixelInCameraCoord.setX((cnt / resolutionX) * (pixelSize * 1e-6))
            .setY((cnt % resolutionX) * (pixelSize * 1e-6))
            .setZ(0.0);

    cameraToGround(spatialPosition[0], spatialPosition[1], pixelInCameraCoord, pixelInGroundCoord);

    // generate the ray
    return {pixelInGroundCoord, rayDirection, sunlightSpectrum, CAMERA_RAY_STARTER_SCATTER_LEVEL};
}
*/
/*
// this function is for a fixed origin point with random direction, yes u heard that
[[deprecated]] Ray Camera::shootRayRandom([[deprecated]] const int cnt) {
    // we need the origin point not out of the camera
    if (cnt < 0 || cnt >= resolutionX * resolutionY) {
        return {};
    }

    // generate a ray for the current pixel( given by cnt )
    // cameraPixelPoint is a point in camera coordinate system
    auto & pixelInCameraCoord = BigO;
    auto pixelInGroundCoord = BigO;

    // row == std::ceil(cnt / resolutionX), col == cnt % resolutionX (row and col start from 1)

    pixelInCameraCoord.setX((cnt / resolutionX) * (pixelSize * 1e-6))
            .setY((cnt % resolutionX) * (pixelSize * 1e-6))
            .setZ(0.0);

    cameraToGround(spatialPosition[0], spatialPosition[1], pixelInCameraCoord, pixelInGroundCoord);

    // generate the ray
    return {pixelInGroundCoord, uniformHemisphereDirection(Vec(Point(0, 0, -1))), sunlightSpectrum, 1};
}
*/
/*
// num is for "rays per pixel"
std::vector<Ray> Camera::shootRaysRandom() const noexcept {
    auto rays = std::vector<Ray>();

    // traverse every pixel to shoot rays
    for (auto &row: *spectralResp_p) {
        for (auto &pixel: row) {
            rays.push_back(pixel.shootRayFromPixel(platformDirection, sunlightSpectrum));
        }
    }

    // compiler will perform RVO here, so don't worry about returning a BIG vector
    return rays;
}

std::vector<Ray> Camera::shootRaysRandomFromImgPlate() const noexcept {
    auto rays = std::vector<Ray>();

    // traverse every pixel to shoot rays
    for (auto &row: *spectralResp_p) {
        for (auto &pixel: row) {
            rays.push_back(pixel.shootRayFromPixel(platformDirection, sunlightSpectrum));
        }
    }

    // compiler will perform RVO here, so don't worry about returning a BIG vector
    return rays;
}
*/
/*
Camera::Camera() {
    spectralResp_p = std::make_shared<std::array<std::array<Pixel, resolutionY>, resolutionX>>();
    // init for every pixel's pos in camera coord contained by camera
    for (int row = 0; row < resolutionX; row++) {
        for (int col = 0; col < resolutionY; col++) {
            auto &pixel = spectralResp_p->at(row).at(col);
            pixel = Pixel();
            pixel.setPosInCam(Point(row, col, 0))
                    .setPosInGnd(coordTransform->camToGnd(pixel.getPosInCam()))
                    .setPosInImg(coordTransform->camToImg(pixel.getPosInCam() * 1e-6));

            coutLogger->writeWarnEntry(
                std::to_string(row * 10 + col) + " posInCam: " + std::to_string(row) + ", " + std::to_string(col) +
                ", 0", 1);
            coutLogger->writeWarnEntry(
                std::to_string(row * 10 + col) + " posInGnd: " + std::to_string(pixel.getPosInGnd().getX()) + ", " +
                std::to_string(pixel.getPosInGnd().getY()) + ", " + std::to_string(pixel.getPosInGnd().getZ()), 1);
            coutLogger->writeWarnEntry(
                std::to_string(row * 10 + col) + " posInImg: " + std::to_string(pixel.getPosInImg().getX()) + ", " +
                std::to_string(pixel.getPosInImg().getY()) + ", " + std::to_string(pixel.getPosInImg().getZ()), 1);
        }
    }
}
*/

Camera::Camera() {
    // first initialize samplePoints from ImagePlane
    pixel2D = std::make_shared<std::vector<std::vector<Pixel> > >();
    imgPlane_u = std::make_unique<ImagePlane>(pixel2D);
    auto imgPlane = *imgPlane_u;
    // then initialize Pixel2D (real objects) and toPixel_p (just pointters to real object) from ImagePlane
    for (int i = 0; i < imgPlane.getCountX(); i++) {
        pixel2D->emplace_back();
        //coutLogger->writeInfoEntry("normal here");
        for (int j = 0; j < imgPlane.getCountY(); j++) {
            (*pixel2D)[i].emplace_back();
            auto sourcePoint = imgPlane.getSamplePoints()[i][j];
            // TODO: use real X and Y, insteal of wrong X and Y;  Z is correct btw.
            // TODO: edit: Now X and Y should be OK
            sourcePoint.setZ((sourcePoint.getZ() + CAM_IMG_DISTANCE - CAMERA_HEIGHT) * 1e-6 + CAMERA_HEIGHT)
                    //sourcePoint.setZ(sourcePoint.getZ() + CAM_IMG_DISTANCE)
                    .setX(sourcePoint.getX() * 1e-6)
                    .setY(sourcePoint.getY() * 1e-6);
            //coutLogger->writeInfoEntry("normal here222");
            if (sourcePoint.getZ() < 0)
                coutLogger->writeWarnEntry(
                    "With the height of " + std::to_string(CAMERA_HEIGHT) + " and CAM_IMG_DISTANCE of " +
                    std::to_string(CAM_IMG_DISTANCE) + ", negative Z encountered once.");
            (*pixel2D)[i][j].setPosInGnd(sourcePoint);
            imgPlane.getMutSamplePointsPixel().at(i).at(j) = &pixel2D->at(i).at(j);
            //imgPlane.getMutSamplePointsPixel()[i][j] = std::make_shared<Pixel>((*pixel2D)[i][j]);
            //imgPlane.getMutSamplePointsPixel()[i][j] = &((*pixel2D)[i][j]);
        }
    }
    buildSunlightSpectrum();
}

void Camera::buildSunlightSpectrum() {
    // sunlight energy distribution should be constant here
    // source: MODTRAN
    // final_height = 0.0, observer height: 0.2, observer zenith angle: 0
    // sun zenith angle: 30, day of year: 173, aztmuth observer to sun: 180
#ifdef _WIN32
    FILE *fp = fopen(R"(..\data\sunlight.csv)", "rt");
    if (fp == nullptr) {
        fprintf(stderr, "Cannot open %s\a\n", R"(..\data\sunlight.csv)");
        exit(2);
    }
#else
    FILE *fp = fopen(R"(./data/sunlight.csv)", "rt");
    if (fp == nullptr) {
        fprintf(stderr, "Cannot open %s\a\n", R"(./data/sunlight.csv)");
        exit(2);
    }
#endif
    int waveLength_t = 0;
    double totalRadiance_t = 0.0;
    double maximumTotRad = 0.0;

    std::map<int, double> sunlightSpectrumMap_t;
#ifdef _WIN32
    while (fscanf_s(fp, "%d,%lf", &waveLength_t, &totalRadiance_t) != EOF) {
        sunlightSpectrumMap_t[waveLength_t] = totalRadiance_t;
    }
#else
    while (fscanf(fp, "%d,%lf", &waveLength_t, &totalRadiance_t) != EOF) {
        sunlightSpectrumMap_t[waveLength_t] = totalRadiance_t;
    }
#endif

    fclose(fp); // close
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

/*
[[deprecated]] inline double overlappingFactor(const Point &source, int x, int y) noexcept {
    // todo
    return 1.0;
}


// input : pos in ground coord
// output: the closest pixel's pos in camera coord
Point Camera::findTheClosestPixel(const Point &source) const noexcept {
    // Initially set min_distance to the highest possible value
    double min_distance = std::numeric_limits<double>::max();
    Point closest;

    // check every pixel, and update the closest pixel and min_distance as necessary
    for (const auto &pixel_row: (*spectralResp_p)) {
        for (const auto &pixel: pixel_row) {
            double curr_distance = source.distance((pixel.getPosInGnd()));
            if (curr_distance < min_distance) {
                min_distance = curr_distance;
                closest = (pixel.getPosInGnd());
            }
        }
    }
    return closest;
}

// todo
void Camera::addRaySpectrumResp(const Ray &ray) const noexcept {
    // travese every pixel, this (i,j) pixel is the pixel taht where we store the response
    for (int i = 0; i < resolutionX; i++) {
        for (int j = 0; j < resolutionY; j++) {
            auto &thatPixel = spectralResp_p->at(i).at(j);
            // factor belong st [0.0, 1.0]
            double factor =
                    realOverlappingRatio(findTheClosestPixel(ray.getAncestor()), (thatPixel.getPosInGnd())) * 0.996;
            for (int k = 0; UPPER_WAVELENGTH + k * WAVELENGTH_STEP < LOWER_WAVELENGTH; k++) {
                // no matter what, it always multiplies at least 0.4% of the intensity as "base noise".
                thatPixel.pixelSpectralResp.at(k) += (ray.getIntensity_p().at(k) * (factor + 0.004));
            }
        }
    }
}

*/

inline double line_segment_intersect(const double startA, const double lenA, const double startB, const double lenB) {
    const double intersection_start = std::max(startA, startB);
    const double intersection_end = std::min(startA + lenA, startB + lenB);
    double intersection_length = 0.0;
    if (startA <= startB + lenB && startB <= startA + lenA) {
        intersection_length = intersection_end - intersection_start;
    } else {
        intersection_length = 0.0;
    }
    return intersection_length;
}

// p1 and p2 should on iamge coord
double Camera::realOverlappingRatio(const Point &p1, const Point &p2) {
    std::array<double, 6> p1_picElemBoundary = {0};
    std::array<double, 6> p2_picElemBoundary = {0};
    p1_picElemBoundary.at(0) = p1.getX() - picElemX;
    p1_picElemBoundary.at(1) = p1.getX() + picElemX;
    p1_picElemBoundary.at(2) = p1.getY() - picElemY;
    p1_picElemBoundary.at(3) = p1.getY() + picElemY;
    p1_picElemBoundary.at(4) = p1.getZ();
    p1_picElemBoundary.at(5) = p1.getZ();

    p2_picElemBoundary.at(0) = p2.getX() - picElemX;
    p2_picElemBoundary.at(1) = p2.getX() + picElemX;
    p2_picElemBoundary.at(2) = p2.getY() - picElemY;
    p2_picElemBoundary.at(3) = p2.getY() + picElemY;
    p2_picElemBoundary.at(4) = p2.getZ();
    p2_picElemBoundary.at(5) = p2.getZ();

    const auto &p1pEB = p1_picElemBoundary;
    const auto &p2pEB = p2_picElemBoundary;

    double overlapX = 0.0, overlapY = 0.0;
    overlapX = line_segment_intersect(p1pEB.at(0), p1pEB.at(1) - p1pEB.at(0), p2pEB.at(0),
                                      p2pEB.at(1) - p2pEB.at(0));
    overlapY = line_segment_intersect(p1pEB.at(2), p1pEB.at(3) - p1pEB.at(2), p2pEB.at(2),
                                      p2pEB.at(3) - p2pEB.at(2));
    const auto p1Size = (p1pEB.at(1) - p1pEB.at(0)) * (p1pEB.at(3) - p1pEB.at(2));
    const auto p2Size = (p2pEB.at(1) - p2pEB.at(0)) * (p2pEB.at(3) - p2pEB.at(2));
    stdoutLogger->writeInfoEntry("overlap: " + std::to_string((overlapX * overlapY) / ((p1Size + p2Size) * 0.5)));
    return (overlapX * overlapY) / ((p1Size + p2Size) * 0.5);
}

std::vector<Ray> *Camera::shootRays(const int multiplier) const noexcept {
    //return imgPlane_u->shootRays(multiplier);

    // I should use multi-threading to accelerate
    // this takes soooooo long I can't bear it when debugging
    const auto &planeNormVec = imgPlane_u->getPlaneNormal();
    auto rays = new std::vector<Ray>;
    /*
    for (int i = 0; i < multiplier; i++)
        for (auto &pixelRow: *pixel2D) {
            for (auto &pixel: pixelRow) {
                rays->push_back(pixel.shootRayFromPixelFromImgPlate(planeNormVec, sunlightSpectrum, &pixel));
            }
        }
    */
#ifdef VERTICES_CONFIG_MULTI_THREAD_FOR_CAMRAYS
#error "[ abort ] VERTICES_CONFIG_MULTI_THREAD_FOR_CAMRAYS is deserted, say, it's discontinued and deprecated.\n          Please use VERTICES_CONFIG_MULTI_THREAD_FOR_CAMRAYS_WORKAROUND instead."
#endif
#ifdef VERTICES_CONFIG_SINGLE_THREAD_FOR_CAMRAYS
    for (int n = 0; n < multiplier; n++) {
        for (int i = 0; i < pixel2D->size(); i++) {
            for (auto &pixel: pixel2D->at(i)) {
                rays->push_back(pixel.shootRayFromPixelFromImgPlate(planeNormVec, sunlightSpectrum, &pixel));
            }
        }
    }
#endif
#ifdef VERTICES_CONFIG_MULTI_THREAD_FOR_CAMRAYS_WORKAROUND
    auto threadAmount = HARDWARE_CONCURRENCY;
    auto rets = new std::vector<wrappedRays>();
    std::vector<std::thread> threads;

    std::atomic_int activeThreads(0);
    for (int n = 0; n < multiplier; n++) {
        for (int i = 0; i < pixel2D->size(); i++) {
            if (activeThreads.load() <= threadAmount) {
                rets->emplace_back();
                // we place ++activeThreads AFTER a thread is created cuz it easily exceeds the limit A LOT, so it's faster than not to do so
                threads.emplace_back([&](int j) {
                    ++activeThreads;
                    auto raysForThisThread = new std::vector<Ray>;
                    for (auto &pixel: pixel2D->at(j)) {
                        raysForThisThread->push_back(
                            pixel.shootRayFromPixelFromImgPlate(planeNormVec, sunlightSpectrum, &pixel));
                    }
                    //coutLogger->writeInfoEntry("raysForThisThread size is " + std::to_string(raysForThisThread->size()) + " " + std::to_string(activeThreads.load()));
                    rets->at(j).rays = std::move(*raysForThisThread);
                    rets->at(j).done = true;
                    --activeThreads;
                    return;
                }, i);
            } else {
                i--;
                std::this_thread::sleep_for(std::chrono::milliseconds(randab(4, 49)));
            }
        }
    }
    for (auto &thread: threads) {
        try {
            thread.join();
        } catch (const std::exception &e) {
            coutLogger->writeErrorEntry(
                "Exception caught when at threads.join() in Camera::shootRays(): " + std::string(e.what()));
        } catch (...) {
            coutLogger->writeErrorEntry("threads.join() in Camera::shootRays() encountered unknown exception");
        }
    }

    //coutLogger->writeInfoEntry("Camera::shootRays() created " + std::to_string(threadAmount) + " threads");
    // now we await for all the threads to exit
    for (auto &[rays_t, done]: *rets) {
        if (done) rays->insert(rays->end(), rays_t.begin(), rays_t.end());
    }
    delete rets;
    threads.clear();
    //coutLogger->writeInfoEntry("Camera::shootRays() finished with rays of " + std::to_string(rays->size()));
#endif
    return rays;
}

Camera &Camera::addSingleRaySpectralRespToPixel(Ray &ray) noexcept {
    static_cast<Pixel *>(ray.getSourcePixel())->addRaySpectralResp(ray);
    return *this;
}

std::shared_ptr<std::vector<std::vector<Pixel> > > &Camera::getPixel2D() noexcept {
    return pixel2D;
}

