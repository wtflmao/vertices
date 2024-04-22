//
// Created by root on 24-4-6.
//

/* Copyright 2024 wtflmao. All Rights Reserved.
 *
 * Distributed under MIT license.
 * See file LICENSE/LICENSE.MIT.md or copy at https://opensource.org/license/mit
 */

#include "imagePlane.h"


std::vector<std::vector<Point> > &ImagePlane::getSamplePoints() noexcept {
    return samplePoints;
}

std::vector<std::vector<Pixel *> > &ImagePlane::getMutSamplePointsPixel() noexcept {
    return toPixel_p;
}

const Point &ImagePlane::getPlaneCenter() const noexcept {
    return planeCenter;
}

const Vec &ImagePlane::getPlaneNormal() const noexcept {
    return planeNormal;
}

const Vec &ImagePlane::getOX() const noexcept {
    return OX;
}

const Vec &ImagePlane::getOY() const noexcept {
    return OY;
}

int ImagePlane::getCountX() const noexcept {
    return countX;
}

int ImagePlane::getCountY() const noexcept {
    return countY;
}

ImagePlane &ImagePlane::setPlaneCenter(const Point &p) noexcept {
    planeCenter = p;
    return *this;
}

ImagePlane &ImagePlane::setPlaneNormal(const Vec &v) noexcept {
    planeNormal = v;
    return *this;
}

ImagePlane &ImagePlane::setOX(const Vec &v) noexcept {
    OX = v;
    return *this;
}

ImagePlane &ImagePlane::setOY(const Vec &v) noexcept {
    OY = v;
    return *this;
}

ImagePlane::ImagePlane(const std::shared_ptr<std::vector<std::vector<Pixel> > > &a) {
    buildImagePlane(a);
};

ImagePlane &ImagePlane::buildImagePlane(const std::shared_ptr<std::vector<std::vector<Pixel> > > &a) noexcept {
    coutLogger->writeInfoEntry("picElemX and Y: " + std::to_string(picElemX) + " " + std::to_string(picElemY));

    // here the 40 is for debug only
    auto Xcount = std::min(static_cast<int>(FIELD_LENGTH_X / picElemX), 80);
    auto Ycount = std::min(static_cast<int>(FIELD_LENGTH_Y / picElemY), 80);
    if (Xcount == 0) Xcount = 2;
    if (Ycount == 0) Ycount = 2;
    if (Xcount % 2 == 1) Xcount++;
    if (Ycount % 2 == 1) Ycount++;
    coutLogger->writeInfoEntry("Xcount and Ycount: " + std::to_string(Xcount) + " " + std::to_string(Ycount));
    coutLogger->writeInfoEntry(
        "X_Length and Y_Length: " + std::to_string(picElemX * Xcount) + " " + std::to_string(
            picElemY * Ycount));

    setOX(getOX() * picElemX);
    setOY(getOY() * picElemY);

    // but here row.size() == X.count+1 and col.size() = Y.count+1, they are odd numbers
    for (int i = -Xcount / 2, rowCnt = 0; i <= Xcount / 2; i++) {
        samplePoints.emplace_back();
        toPixel_p.emplace_back();
        countY = 0;
        auto &row = samplePoints.at(rowCnt);
        auto &rowForToPixel = toPixel_p.at(rowCnt++);
        for (int j = -Ycount / 2, colCnt = 0; j <= Ycount / 2; j++) {
            auto source = getOY() * j + (getOX() * i + getPlaneCenter());
            row.emplace_back(source);
            rowForToPixel.push_back(nullptr);

            //std::ostringstream s;
            //s << "source: " << source;
            //coutLogger->writeInfoEntry(s.view());
        }
    }
    countX = static_cast<int>(samplePoints.size());
    countY = static_cast<int>(samplePoints.front().size());
    coutLogger->writeInfoEntry("Done imageplane building");
    return *this;
}

std::vector<Ray> *ImagePlane::shootRays(const int N) const noexcept {
    auto *rays = new std::vector<Ray>();
    for (int n = 0; n < N; n++) {
        for (int i = 0; i < samplePoints.size(); i++) {
            for (int j = 0; j < samplePoints.at(i).size(); j++) {
                auto &source = samplePoints[i][j];
                Ray ray = {};
                ray.setOrigin(source)
                        .setAncestor(source)
                        .setDirection(planeNormal)
                        .setScatteredLevel(CAMERA_RAY_STARTER_SCATTER_LEVEL)
                        .setSourcePixel(static_cast<void *>(toPixel_p[i][j]));
                // TODO: this line below encountered SIGSEGV
                //.setSourcePixelPosInGnd(toPixel_p[i][j]->getPosInGnd());
                static_cast<Pixel *>(ray.getSourcePixel())->setPosInGnd(toPixel_p[i][j]->getPosInGnd());
                rays->push_back(ray);
            }
        }
    }
    return rays;
}
