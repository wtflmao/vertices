//
// Created by root on 2024/3/15.
//

/* Copyright 2024 wtflmao. All Rights Reserved.
 *
 * Distributed under MIT license.
 * See file LICENSE/LICENSE.MIT.md or copy at https://opensource.org/license/mit
 */

#ifndef VERTICES_ITEM_H
#define VERTICES_ITEM_H

#include "triangle.h"
#include "../basic/BRDF.h"
#include "mtlDataset.h"
#include <vector>
#include <string>
#include <array>
#include <memory>
#include <iostream> // for debug
#include <set>
#include <queue>
#include <tuple>
#include <stdexcept>


class Item final {
private:
    std::vector<Triangle> faces;
    std::vector<Point> vertices;
    std::vector<std::array<int, 3> > facesWithVertexRefs;
    std::array<double, 3> scaleFactor = {1.0, 1.0, 1.0};
    // This center should be the center AFTER applied the zoom factor
    Point center = BigO;

    std::string objPath = "";
    std::string mtlPath = "";
    std::array<double, 6> rawBoundary = {0.0};

    std::unique_ptr<std::vector<Point>> normals = std::make_unique<std::vector<Point>>();

public:
    // open mesh be like plane, ground, etc.
    // closed mesh be like ball, cube, cylinder
    bool isOpenMesh = false;
    short brdfIdx = -1;
    int forwardAxis = 6; // 1 2 3 for X Y Z, 4 5 6 for -X -Y -Z
    int upAxis = 3;
    MTLDataset mtlDataset = MTLDataset();
    std::array<int, 3> thatCorrectFaceVertices = {1, 2, 4};
    int thatCorrectFaceIndex = 0;
    std::vector<Point> innerPoints;
    bool requireFromOBJ = true;
    bool allowWriteMtlDataset = false;

    [[nodiscard]] const std::vector<Triangle> &getFaces() const noexcept;
    [[nodiscard]] const std::vector<Point> &getVertices() const noexcept;
    std::vector<Triangle> &getMutFaces() noexcept;
    std::vector<Point> &getMutVertices() noexcept;

    Item();
    [[nodiscard]] const std::array<double, 3>& getScaleFactor() const noexcept;
    [[nodiscard]] const Point &getCenter() const noexcept;

    Item &setScaleFactor(const std::array<double, 3> factor) noexcept;

    Item &setCenter(Point pos) noexcept;

    [[nodiscard]] std::vector<std::shared_ptr<Triangle> > getFaceRefs() noexcept;

    std::vector<std::array<int, 3> > &getMutFWVR() noexcept;

    [[nodiscard]] const std::vector<std::array<int, 3> > &getFWVR() const noexcept;

    Item &inspectNormalVecForAllFaces() noexcept;

    Item& verbose(int hashtag = -1) noexcept;

    [[maybe_unused]] [[nodiscard]] float getBRDFOpen(int waveLength, double i, double j, BRDF &b_ori) const noexcept;

    [[maybe_unused]] [[nodiscard]] double
    getBRDFClosed(int waveLength, BRDF &b_ori, double theta_i, double phi_i, double theta_r,
                  double phi_r) const noexcept;

    Item &setOBJPath(const std::string &objPath_t) noexcept;

    Item &setMTLPath(const std::string &mtlPath_t) noexcept;

    Item &setThatCorrectFaceVertices(const std::array<int, 3> &correctFaceVertices_t) noexcept;

    Item &setThatCorrectFaceIndex(const int tCFI) noexcept;

    Item &setInnerPoints(const std::vector<Point> &innerPoints_t) noexcept;

    Item &setForwardAxis(const int axis) noexcept;

    Item &setUpAxis(const int axis) noexcept;

    [[nodiscard]] const std::array<int, 3> &getThatCorrectFaceVertices() const;

    [[nodiscard]] int getThatCorrectFaceIndex() const;

    [[nodiscard]] const std::vector<Point> &getInnerPoints() const;

    Item &readFromOBJ();

    Item &readFromMTL();

    Item& setSelfAsBorderWall() noexcept;

    // raw boundary means the "orignal" position coordinates from .obj file, before moved and scaled
    Item& updateRawBoundary() noexcept;

    [[nodiscard]] const std::array<double, 6>& getRawBoundary() const noexcept;

    Item& setRawBoundary(const std::array<double, 6>& p) noexcept;

    [[nodiscard]] const std::unique_ptr<std::vector<Point>>& getNormalList() const noexcept;

    [[nodiscard]] std::unique_ptr<std::vector<Point>>& getMutNormalList() noexcept;

    Item& setNoNormalReqFromObjFile() noexcept;
};

bool readNewItem(const char *filename, Item &item);

#endif //VERTICES_ITEM_H
