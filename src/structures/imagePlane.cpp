//
// Created by root on 24-4-6.
//

/* Copyright 2024 wtflmao. All Rights Reserved.
 *
 * Distributed under MIT license.
 * See file LICENSE/LICENSE.MIT.md or copy at https://opensource.org/license/mit
 */

#include "imagePlane.h"


std::vector<std::vector<Point>>& ImagePlane::getSamplePoints() noexcept {
    return samplePoints;
}

const Point& ImagePlane::getPlaneCenter() const noexcept {
    return planeCenter;
}

const Vec& ImagePlane::getPlaneNormal() const noexcept {
    return planeNormal;
}

const Vec& ImagePlane::getOX() const noexcept {
    return OX;
}

const Vec& ImagePlane::getOY() const noexcept {
    return OY;
}

ImagePlane& ImagePlane::setSamplePoints(const std::vector<std::vector<Point>> &s) noexcept {
    samplePoints = s;
    return *this;
}

ImagePlane& ImagePlane::setPlaneCenter(const Point& p) noexcept {
    planeCenter = p;
    return *this;
}

ImagePlane& ImagePlane::setPlaneNormal(const Vec& v) noexcept {
    planeNormal = v;
    return *this;
}

ImagePlane& ImagePlane::setOX(const Vec& v) noexcept {
    OX = v;
    return *this;
}

ImagePlane& ImagePlane::setOY(const Vec& v) noexcept {
    OY = v;
    return *this;
}

ImagePlane::ImagePlane() {
    buildImagePlane();
};

ImagePlane& ImagePlane::buildImagePlane() noexcept {
    coutLogger->writeInfoEntry("picElemX and Y: " + std::to_string(picElemX) + " " + std::to_string(picElemY));

    // to make sure that the Xcount and Ycount are even numbers
    auto Xcount = static_cast<int>(FIELD_LENGTH_X / picElemX);
    auto Ycount = static_cast<int>(FIELD_LENGTH_Y / picElemY);
    if (Xcount == 0) Xcount = 2;
    if (Ycount == 0) Ycount = 2;
    if (Xcount % 2 == 1) Xcount++;
    if (Ycount % 2 == 1) Ycount++;
    coutLogger->writeInfoEntry("Xcount and Ycount: " + std::to_string(Xcount) + " " + std::to_string(Ycount));

    setOX(getOX() * picElemX);
    setOY(getOY() * picElemY);

    for (int i = -Xcount/2, rowCnt = 0; i <= Xcount/2; i++) {
        if (i == 0) continue;
        samplePoints.emplace_back();
        auto& row = samplePoints.at(rowCnt++);
        for (int j = -Ycount/2, colCnt = 0; j <= Ycount/2; j++) {
            if (j == 0) continue;
            auto source = getOY() * j + (getOX() * i + getPlaneCenter());
            row.push_back(source);
        }
    }

    return *this;
}
