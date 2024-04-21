//
// Created by root on 2024/3/13.
//

/* Copyright 2024 wtflmao. All Rights Reserved.
 *
 * Distributed under MIT license.
 * See file LICENSE/LICENSE.MIT.md or copy at https://opensource.org/license/mit
 */

#include "ray.h"

// Constructor
Ray::Ray(const Point &origin, const Vec &direction) noexcept
    : origin(origin), direction(direction) {
    scatteredLevel = 0;
    ancestor = origin;
    intensity_p = {};
}

Ray::Ray(const Point &direction) noexcept
    : direction(Vec(direction)) {
    scatteredLevel = 0;
    ancestor = origin;
    intensity_p = {};
}

Ray::Ray() noexcept {
    intensity_p = {};
}

Ray::Ray(const Point &origin, const Vec &direction,
         std::array<double, spectralBands> intensity_p) noexcept
    : origin(origin), direction(direction), intensity_p(intensity_p) {
    scatteredLevel = 0;
    ancestor = origin;
}

void *Ray::getSourcePixel() const noexcept {
    return sourcePixel;
}

Ray &Ray::setSourcePixel(void *p) noexcept {
    sourcePixel = p;
    return *this;
}

const Point &Ray::getOrigin() const noexcept {
    return origin;
}

Ray &Ray::setOrigin(const Point &origin_t) noexcept {
    origin = origin_t;
    return *this;
}

const Point &Ray::getAncestor() const noexcept {
    return ancestor;
}

Ray &Ray::setAncestor(const Point &ancestor_t) noexcept {
    ancestor = ancestor_t;
    return *this;
}

const Point &Ray::getSourcePixelPosInGnd() const noexcept {
    return sourcePixelPosInGnd;
}

Ray &Ray::setSourcePixelPosInGnd(const Point &t) noexcept {
    sourcePixelPosInGnd = t;
    return *this;
}

const Vec &Ray::getDirection() const noexcept {
    return direction;
}

Ray &Ray::setDirection(const Vec &d_t) noexcept {
    direction = d_t;
    return *this;
}

int Ray::getScatteredLevel() const noexcept {
    return scatteredLevel;
}

Ray &Ray::setScatteredLevel(const int t) noexcept {
    scatteredLevel = t;
    return *this;
}

const std::array<double, spectralBands> &Ray::getIntensity_p() const noexcept {
    return intensity_p;
}

std::array<double, spectralBands> &Ray::getMutIntensity_p() noexcept {
    return intensity_p;
}

Ray &Ray::setIntensity_p(const std::array<double, spectralBands> &t) noexcept {
    intensity_p = t;
    return *this;
}


// Cross product: self x other
Vec Ray::crossVec(const Vec &other) const noexcept {
    const double v[3] = {
            this->direction.getTail().getY() * other.getTail().getZ() -
            this->direction.getTail().getZ() * other.getTail().getY(),
            this->direction.getTail().getZ() * other.getTail().getX() -
            this->direction.getTail().getX() * other.getTail().getZ(),
            this->direction.getTail().getX() * other.getTail().getY() -
            this->direction.getTail().getY() * other.getTail().getX()
    };
    return Vec(Point(v[0], v[1], v[2]));
}

// Dot product: self . other
double Ray::dotVec(const Vec &other) const noexcept {
    return this->direction.getTail().getX() * other.getTail().getX() +
           this->direction.getTail().getY() * other.getTail().getY() + this->direction.getTail().getZ() *
                                                                       other.getTail().getZ();
}

// Member function
Point Ray::mollerTrumboreIntersection(const Triangle &tri) const {
    double det, inv_det, u, v, t;
    Point intersection = Point{DBL_MAX, DBL_MAX, DBL_MAX};


    //dge1 = tri.v1 - tri.v0;
    Vec edge1 = Vec(tri.getV0(), tri.getV1());
    //edge2 = tri.v2 - tri.v0;
    Vec edge2 = Vec(tri.getV0(), tri.getV2());
    Vec pvec = crossVec(edge2);
    det = edge1.dot(pvec);

    // Check if ray is parallel to triangle
    if (det > -(1e-10) && det < 1e-10) {
        // Ray is parallel to the triangle, no intersection
        return intersection;
    }

    // Calculate distance from v0 to ray origin
    inv_det = 1.0 / det;
    Vec tvec = Vec(tri.getV0(), this->origin);
    u = tvec.dot(pvec) * inv_det;

    // Check if intersection point is outside the first edge
    if (u < 0.0 || u > 1.0) {
        // The intersection lies outside the first edge
        return intersection;
    }

    // Prepare to test the second edge
    auto qvec = tvec.cross(edge1);
    v = dotVec(qvec) * inv_det;

    // Check if intersection point is outside the second edge
    if (v < 0 || u + v > 1.0) {
        // The intersection lies outside the triangle 2nd egde
        return intersection;
    }

    // Compute the t value for the intersection point
    t = edge2.dot(qvec) * inv_det;

    // Check if the intersection point is behind the ray
    if (t < 0.0) {
        // The intersection lies behind the ray
        return intersection;
    }

    // to check if the intersection is viable
    if (t > static_cast<double>(stopLength)) {
        return intersection;
    }

    // Intersection point is t distance away from ray origin in direction of ray
    intersection = this->direction * t + this->origin;
    return intersection;
}

bool Ray::intersectsWithBox(const Box &box) const {
    double tMin = -std::numeric_limits<double>::infinity();
    double tMax = std::numeric_limits<double>::infinity();

    const auto directionXYZ = direction.getTail().getXYZ();
    const auto originXYZ = origin.getXYZ();
    const auto minBoundXYZ = box.getBounds()[0].getXYZ();
    const auto maxBoundXYZ = box.getBounds()[1].getXYZ();

    for (int i = 0; i < 3; i++) {
        if (directionXYZ[i] != 0.0) {
            double t1 = (minBoundXYZ[i] - originXYZ[i]) / directionXYZ[i];
            double t2 = (maxBoundXYZ[i] - originXYZ[i]) / directionXYZ[i];
            if (t1 > t2) {
                const double tmp = t1;
                t1 = t2;
                t2 = tmp;
            }
            if (t1 > tMin) {
                tMin = t1;
            }
            if (t2 < tMax) {
                tMax = t2;
            }
            if (tMin > tMax) {
                return false;
            }
        } else {
            if (originXYZ[i] < minBoundXYZ[i] || originXYZ[i] > maxBoundXYZ[i]) {
                return false;
            }
        }
    }
    return true;
}



Ray::Ray(const Point &origin, const Vec &direction, const std::array<double, spectralBands> &intesity,
         int scatteredCount)
    : origin(origin), direction(direction), scatteredLevel(scatteredCount) {
    intensity_p = intesity;
}

// reflectance correction, x is in nanometer
// todo: make it works
inline double reflectanceCorrection(const double x) {
    return 1 - rand01() * rand01() * rand01() * rand01();
    constexpr double mu = UPPER_WAVELENGTH;
    constexpr double sigma = 0.03;
    const double a = (x - mu) / sigma;
    return (1 - sigma) + std::exp(-0.5 * a * a) / (sigma * std::sqrt(2.0 * std::numbers::pi));
}

// scatter the ray with a given triangle(needs its normal vector) and intersection point
// returns a Ray object
// ior means index of refraction, for materials that non-transparent we set ior = 1.0
std::array<Ray, SCATTER_RAYS + 1> Ray::scatter(const Triangle &tri, const Point &intersection,
                                               BRDF *itemBRDF, void *sourcePixel_p) const {
    if (scatteredLevel >= 2) {
        //std::cout << "...scattered level: " << scatteredLevel << ", scatter aborted." << std::endl;
        return {std::array<Ray, SCATTER_RAYS + 1>{}};
    }
    bool allPositive = true;
    for (auto &intensity_: getIntensity_p()) {
        if (intensity_ < 0) allPositive = false;
    }
    if (!allPositive)
        return {std::array<Ray, SCATTER_RAYS + 1>{}};
    /* refletance is a value between 0 and 1
     * 1.0: total mirror reflection
     * 0.0: total scatter, direction is random
     * 0.0-1.0: include both
     */

    std::array<Ray, SCATTER_RAYS + 1> theRays;
    const Vec incidentDirection = direction.getNormalized();
    const Vec normal = tri.getNormal().getNormalized();

    // get the reflection direction
    const Vec reflectedDirection = (incidentDirection - (normal * normal.dot(incidentDirection)) * 2.0).getNormalized();
    int reflectionRayIdx = -1;
    int reflectionCnt = 0;
    //reflectedDirection = reflectedDirection.getNormalized();

    // assign the intensity
    std::array<double, spectralBands> reflectedIntensity = {};

    // create reflected ray

    for (int j = 0; j < reflectedIntensity.size(); j++) {
        if (itemBRDF->type == BRDFType::Open) {
            // here's open mesh's BRDF
            auto w = UPPER_WAVELENGTH + j * WAVELENGTH_STEP;
            if (w <= UPPER_WAVELENGTH || w >= LOWER_WAVELENGTH) {
                reflectedIntensity[j] = intensity_p[j] * 0.0;
            } else if (w >= BLUE_UPPER && w < BLUE_LOWER) {
                reflectedIntensity[j] =
                        intensity_p[j] * dynamic_cast<OpenBRDF *>(itemBRDF)->valMap->at({BLUE_UPPER, BLUE_LOWER}).at(
                                                static_cast<std::size_t>(std::round(rand01() *
                                                                                            (dynamic_cast<OpenBRDF *>(itemBRDF)->MODIS_HDF_DATA_DIM_X -
                                                                                             1)))).at(
                                                static_cast<std::size_t>(std::round(rand01() *
                                                                                            (dynamic_cast<OpenBRDF *>(itemBRDF)->MODIS_HDF_DATA_DIM_Y -
                                                                                                1)))) / 32767;
            } else if (w >= GREEN_UPPER && w < GREEN_LOWER) {
                reflectedIntensity[j] = intensity_p[j] *
                        dynamic_cast<OpenBRDF *>(itemBRDF)->valMap->at({GREEN_UPPER, GREEN_LOWER}).at(
                                                static_cast<std::size_t>(std::round(rand01() *
                                                                                            (dynamic_cast<OpenBRDF *>(itemBRDF)->MODIS_HDF_DATA_DIM_X -
                                                                                             1)))).at(
                                                static_cast<std::size_t>(std::round(rand01() *
                                                                                            (dynamic_cast<OpenBRDF *>(itemBRDF)->MODIS_HDF_DATA_DIM_Y -
                                                                                                1)))) / 32767;
            } else if (w >= RED_UPPER && w <= RED_LOWER) {
                reflectedIntensity[j] = intensity_p[j] *
                        dynamic_cast<OpenBRDF *>(itemBRDF)->valMap->at({RED_UPPER, RED_LOWER}).at(
                                                static_cast<std::size_t>(std::round(rand01() *
                                                                                            (dynamic_cast<OpenBRDF *>(itemBRDF)->MODIS_HDF_DATA_DIM_X -
                                                                                             1)))).at(
                                                static_cast<std::size_t>(std::round(rand01() *
                                                                                            (dynamic_cast<OpenBRDF *>(itemBRDF)->MODIS_HDF_DATA_DIM_Y -
                                                                                                1)))) / 32767;
            } else {
                reflectedIntensity[j] = intensity_p[j] * reflectanceCorrection(UPPER_WAVELENGTH + j * WAVELENGTH_STEP);
            }
            // debug only
            for (auto &intensity_: reflectedIntensity)
                if (intensity_ < 0) {
                    std::ostringstream ss;
                    ss << "OpenBRDF negative ray intensity generated " << std::setprecision(4) << intensity_;
                    coutLogger->writeErrorEntry(ss.view());
                }
        } else if (itemBRDF->type == BRDFType::Closed) {
            // here's closed mesh's BRDF
            // todo: rotate the coordinate system to compute phi_in
            // here's two dummy in_angles
            double theta_in = std::abs(
                std::acos(
                    this->dotVec(tri.getNormal()) / this->getDirection().getLength() / tri.getNormal().getLength()) -
                0.5 * std::numbers::pi), phi_in = std::numbers::pi * 2.0 * rand01() - std::numbers::pi;
            // we use random out_angles that resides in upper hemisphere
            double theta_out = std::numbers::pi / 2.0 * rand01(), phi_out =
                    std::numbers::pi * 2.0 * rand01() - std::numbers::pi;

            /*
            auto theta_in_s = static_cast<short>(std::round(theta_in * 1e4));
            auto phi_in_s = static_cast<short>(std::round(phi_in * 1e4));
            auto theta_out_s = static_cast<short>(std::round(theta_out * 1e4));
            auto phi_out_s = static_cast<short>(std::round(phi_out * 1e4));

            if (dynamic_cast<ClosedBRDF *>(itemBRDF)->availThetaIn->empty()) {
                coutLogger->writeErrorEntry("availThetaIn from ClosedBRDF is empty");
                exit(111);
            }
            if (dynamic_cast<ClosedBRDF *>(itemBRDF)->availPhiIn->empty()) {
                coutLogger->writeErrorEntry("availPhiIn from ClosedBRDF is empty");
                exit(112);
            }
            if (dynamic_cast<ClosedBRDF *>(itemBRDF)->availThetaOut->empty()) {
                coutLogger->writeErrorEntry("availThetaOut from ClosedBRDF is empty");
                exit(113);
            }
            if (dynamic_cast<ClosedBRDF *>(itemBRDF)->availPhiOut->empty()) {
                coutLogger->writeErrorEntry("availPhiOut from ClosedBRDF is empty");
                exit(114);
            }

            bool flag = false;
            for (const auto item: *(dynamic_cast<ClosedBRDF *>(itemBRDF)->availThetaIn)) {
                if (item >= theta_in_s) {
                    theta_in_s = item;
                    flag = true;
                    break;
                }
            }
            if (!flag) {
                theta_in_s = *dynamic_cast<ClosedBRDF *>(itemBRDF)->availThetaIn->end();
            }

            flag = false;
            for (const auto item: *(dynamic_cast<ClosedBRDF *>(itemBRDF)->availThetaOut)) {
                if (item >= theta_out_s) {
                    theta_out_s = item;
                    flag = true;
                    break;
                }
            }
            if (!flag) {
                theta_out_s = *dynamic_cast<ClosedBRDF *>(itemBRDF)->availThetaOut->end();
            }

            flag = false;
            for (const auto item: *(dynamic_cast<ClosedBRDF *>(itemBRDF)->availPhiIn)) {
                if (item >= phi_in_s) {
                    phi_in_s = item;
                    flag = true;
                    break;
                }
            }
            if (!flag) {
                phi_in_s = *dynamic_cast<ClosedBRDF *>(itemBRDF)->availPhiIn->end();
            }

            flag = false;
            for (const auto item: *(dynamic_cast<ClosedBRDF *>(itemBRDF)->availPhiOut)) {
                if (item >= phi_out_s) {
                    phi_out_s = item;
                    flag = true;
                    break;
                }
            }
            if (!flag) {
                phi_out_s = *dynamic_cast<ClosedBRDF *>(itemBRDF)->availPhiOut->end();
            }
            */

            //coutLogger->writeInfoEntry("theta_in: " + std::to_string(theta_in) + " phi_in: " + std::to_string(phi_in) + " theta_out: " + std::to_string(theta_out) + " phi_out: " + std::to_string(phi_out), 1);

            /*
            auto w = UPPER_WAVELENGTH + j * WAVELENGTH_STEP;
            if (w <= UPPER_WAVELENGTH || w >= LOWER_WAVELENGTH) {
                reflectedIntensity[j] = intensity_p[j] * 0.0;
            } else if (w >= BLUE_UPPER && w < BLUE_LOWER) {
                reflectedIntensity[j] = intensity_p[j] * std::get<2>(dynamic_cast<ClosedBRDF *>(itemBRDF)
                        ->lookUpBRDF(dynamic_cast<ClosedBRDF *>(itemBRDF)->filename.c_str(), theta_in, phi_in, theta_out, phi_out));
            } else if (w >= GREEN_UPPER && w < GREEN_LOWER) {
                reflectedIntensity[j] = intensity_p[j] * std::get<1>(dynamic_cast<ClosedBRDF *>(itemBRDF)
                        ->lookUpBRDF(dynamic_cast<ClosedBRDF *>(itemBRDF)->filename.c_str(), theta_in, phi_in, theta_out, phi_out));
            } else if (w >= RED_UPPER && w <= RED_LOWER) {
                reflectedIntensity[j] = intensity_p[j] * std::get<0>(dynamic_cast<ClosedBRDF *>(itemBRDF)
                        ->lookUpBRDF(dynamic_cast<ClosedBRDF *>(itemBRDF)->filename.c_str(), theta_in, phi_in, theta_out, phi_out));
            } else {
                reflectedIntensity[j] = intensity_p[j] * reflectanceCorrection(UPPER_WAVELENGTH + j * WAVELENGTH_STEP);
            }
            */
            // for debug-stage acceleration only, plz remove the one line below and uncomment code above
            reflectedIntensity[j] = intensity_p[j] ;//* (1.0 - rand01() * rand01() * rand01());

            // debug only
            for (auto &intensity_: reflectedIntensity)
                if (intensity_ < 0) {
                    std::ostringstream ss;
                    ss << "ClosedBRDF negative ray intensity generated " << std::setprecision(4) << intensity_;
                    coutLogger->writeErrorEntry(ss.view());
                }
        } else {
            std::cerr << "[Error] no implementation for BRDFType::Default typed BRDF\a" << std::endl;
        }
    }
    theRays[0] = (Ray(intersection, reflectedDirection, reflectedIntensity, scatteredLevel + 1));
    reflectionRayIdx = 0;
    reflectionCnt++;


    // build a temp array for scattered rays
    auto scatteredIntensity_p = std::make_unique<std::array<double, spectralBands> >();
    auto &scatteredIntensity = *scatteredIntensity_p;

    auto totalScatteredIntensity_p = std::make_unique<std::array<double, spectralBands> >();
    auto &totalScatteredIntensity = *totalScatteredIntensity_p;

    for (int j = 0; j < reflectedIntensity.size(); j++) {
        if (intensity_p[j] - reflectedIntensity[j] < -1e-10) {
            std::ostringstream ss;
            ss << "Negative ray intensity generated " << std::setprecision(8) << intensity_p[j] << " " <<
                    reflectedIntensity[j];
            coutLogger->writeErrorEntry(ss.view());
        }
        totalScatteredIntensity[j] = std::max(intensity_p[j] - reflectedIntensity[j], 0.0);
    }

    // create scattered rays
    for (int i = 1; i <= SCATTER_RAYS; i++) {
        Vec scatteredDirection = uniformHemisphereDirectionWithCenterOfMonteCarloSpace(normal, reflectedDirection);
        for (int j = 0; j < totalScatteredIntensity.size(); j++) {
            scatteredIntensity[j] = totalScatteredIntensity[j] / SCATTER_RAYS;
        }
        //if (i >= 1 && i < SCATTER_RAYS) {
        theRays[i] = (Ray(intersection, scatteredDirection, scatteredIntensity, scatteredLevel + 1));
        theRays[i].setAncestor(getAncestor());
        //std::cout << "...level " << scatteredLevel << ", so, scatteredDirection is " << scatteredDirection.tail <<
        //        " "
        //        << theRays[i].intensity_p[0] << std::endl;
        //}
    }

    // set a pointer to source pixel
    if (sourcePixel_p != nullptr) {
        for (auto &ray: theRays) {
            ray.setSourcePixel(sourcePixel_p);
        }
    } else {
        if (sourcePixel != nullptr) {
            for (auto &ray: theRays) {
                ray.setSourcePixel(sourcePixel);
            }
        } else {
            // trying to find the Pixel by Pixel's position
            coutLogger->writeErrorEntry(
                "in Ray::scatter() the class member `void* sourcePixel` AND fallback argument `void* sourcePixel_p` both are nullptr");
        }
    }
    return theRays;
}

void Ray::setRayStopPoint(const Point &stopPoint_t) noexcept {
    stopLength = Vec(origin, stopPoint_t).getLength() - 1e-8;
}
