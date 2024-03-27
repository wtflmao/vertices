//
// Created by root on 2024/3/13.
//

#include "ray.h"

#include <utility>

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

const Point &Ray::getOrigin() const noexcept {
    return this->origin;
}

const Vec &Ray::getDirection() const noexcept {
    return this->direction;
}

// Cross product: self x other
Vec Ray::crossVec(const Vec &other) const noexcept {
    const double v[3] = {
        this->direction.tail.y * other.tail.z - this->direction.tail.z * other.tail.y,
        this->direction.tail.z * other.tail.x - this->direction.tail.x * other.tail.z,
        this->direction.tail.x * other.tail.y - this->direction.tail.y * other.tail.x
    };
    return Vec(Point(v[0], v[1], v[2]));
}

// Dot product: self . other
double Ray::dotVec(const Vec &other) const noexcept {
    return this->direction.tail.x * other.tail.x + this->direction.tail.y * other.tail.y + this->direction.tail.z *
           other.tail.z;
}

// Member function
Point Ray::mollerTrumboreIntersection(const Triangle &tri) const {
    double det, inv_det, u, v, t;
    Point intersection = Point{DBL_MAX, DBL_MAX, DBL_MAX};


    //dge1 = tri.v1 - tri.v0;
    Vec edge1 = Vec(tri.v0, tri.v1);
    //edge2 = tri.v2 - tri.v0;
    Vec edge2 = Vec(tri.v0, tri.v2);
    Vec pvec = this->crossVec(edge2);
    det = edge1.dot(pvec);

    // Check if ray is parallel to triangle
    if (det > -(1e-10) && det < 1e-10) {
        // Ray is parallel to the triangle, no intersection
        return intersection;
    }

    // Calculate distance from v0 to ray origin
    inv_det = 1.0 / det;
    Vec tvec = Vec(tri.v0, this->origin);
    u = tvec.dot(pvec) * inv_det;

    // Check if intersection point is outside the first edge
    if (u < 0.0 || u > 1.0) {
        // The intersection lies outside of the first edge
        return intersection;
    }

    // Prepare to test the second edge
    auto qvec = tvec.cross(edge1);
    v = this->dotVec(qvec) * inv_det;

    // Check if intersection point is outside the second edge
    if (v < 0 || u + v > 1.0) {
        // The intersection lies outside of the triangle 2nd egde
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

    const auto directionXYZ = direction.tail.getXYZ();
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

// compute two Vecs that zhengjiao to teh normal vector
void computeCoordinateSystem(const Vec &normal, Vec &tangent, Vec &bitangent) {
    tangent = std::fabs(normal.tail.x) > std::fabs(normal.tail.z)
                  ? Vec(Point(-normal.tail.y, normal.tail.x, 0.0))
                  : Vec(Point(0.0, -normal.tail.z, normal.tail.y));
    tangent = tangent.getNormalized();
    bitangent = normal.cross(tangent);
}

// 在上半球空间里随机生成一个向量
// FOVs are in degrees
Vec uniformHemisphereDirection(const Vec &normal) {
    // generate two random numbers stored as u and v
    double u = rand01();
    double v = rand01();

    // this is a random angle theta, from 0 to pi/2.
    //double theta = std::acos(std::sqrt(1.0 - v));
    double fovRatio_x = FOVx / 90.0;
    double theta = std::acos(std::sqrt(1.0 - v * fovRatio_x));

    // this is a random angle phi, ranges from 0 to 2*pi
    //double phi = 2.0 * std::numbers::pi * u;
    double phi = FOVy * u * std::numbers::pi / 180.0;

    // get random xyz
    double x = std::cos(phi) * std::sin(theta);
    double y = std::sin(phi) * std::sin(theta);
    double z = std::cos(theta);

    // construct a random direction
    const Vec dir = Vec(Point(x, y, z));

    // rotate the normal vector to the upper hemisphere
    auto tangent = Vec(BigO), bitangent = Vec(BigO);
    computeCoordinateSystem(normal, tangent, bitangent);
    return tangent * dir.tail.x + bitangent * dir.tail.y + normal * dir.tail.z;
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

// scatter the ray with a give triangle(needs its normal vector) and intersection point
// returns a Ray object
// ior means index of refraction, for materials that non-transparent we set ior = 1.0
std::array<Ray, SCATTER_RAYS + 1> Ray::scatter(const Triangle &tri, const Point &intersection,
                                               const double reflectance) const {
    if (scatteredLevel >= 2) {
        //std::cout << "...scattered level: " << scatteredLevel << ", scatter aborted." << std::endl;
        return {std::array<Ray, SCATTER_RAYS + 1>{}};
    }

    /* refletance is a value between 0 and 1
     * 1.0: total mirror reflection
     * 0.0: total scatter, direction is random
     * 0.0-1.0: include both
     */
    // todo: fetch the reflectance from the material

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
        reflectedIntensity[j] = intensity_p[j] * reflectance * reflectanceCorrection(
                                    UPPER_WAVELENGTH + j * WAVELENGTH_STEP);
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
        totalScatteredIntensity[j] = intensity_p[j] - reflectedIntensity[j];
    }

    // create scattered rays
    for (int i = 1; i <= SCATTER_RAYS; i++) {
        Vec scatteredDirection = uniformHemisphereDirection(normal);
        for (int j = 0; j < totalScatteredIntensity.size(); j++) {
            scatteredIntensity[j] = totalScatteredIntensity[j] / (SCATTER_RAYS - reflectionCnt + 1);
        }
        //if (i >= 1 && i < SCATTER_RAYS) {
        theRays[i] = (Ray(intersection, scatteredDirection, scatteredIntensity, scatteredLevel + 1));
        theRays[i].ancestor = ancestor;
        //std::cout << "...level " << scatteredLevel << ", so, scatteredDirection is " << scatteredDirection.tail <<
        //        " "
        //        << theRays[i].intensity_p[0] << std::endl;
        //}
    }

    if (reflectionRayIdx != -1) {
        //std::cout << "...level " << scatteredLevel << "...reflectedDirection is " << reflectedDirection.tail << " " <<
        //        theRays[reflectionRayIdx].intensity_p[0]
        //        << std::endl;
    } else {
        std::cout << "...level " << scatteredLevel << "...has no reflectedDirection." << std::endl;
    }
    return theRays;
}

void Ray::setRayStopPoint(const Point &stopPoint_t) noexcept {
    stopLength = Vec(origin, stopPoint_t).getLength() - 1e-8;
}
