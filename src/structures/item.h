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

public:
    /*
     * Ns 10.0000000: 镜面指数为10.0,控制镜面高光的尖锐程度。
     * Ni 1.5000000: 折射指数为1.5。
     * d 1.0000000: 透明度为1.0,即完全不透明。
     * illum 2: 使用Blinn-Phong光照模型。
     * Ka r g b: 环境光反射颜色,以RGB三个浮点数表示。
     * Kd r g b: 漫反射颜色,以RGB三个浮点数表示。
     * Ks r g b: 镜面反射颜色,以RGB三个浮点数表示。
     * Ka 0.5880000 0.5880000 0.5880000: 环境光反射颜色为灰色。
     * Kd 0.5880000 0.5880000 0.5880000: 漫反射颜色也为灰色。
     * Ks 0.0000000 0.0000000 0.0000000: 镜面反射颜色为黑色,即没有镜面高光。
     * map_Ka F22.jpg: 使用名为"F22.jpg"的纹理贴图作为环境光反射贴图。
     * map_Kd F22.jpg: 使用同样的"F22.jpg"纹理作为漫反射贴图。
     */
    // open mesh be like plane, ground, etc.
    // closed mesh be like ball, cube, cylinder
    bool isOpenMesh = false;
    short brdfIdx = -1;
    int forwardAxis = 6; // 1 2 3 for X Y Z, 4 5 6 for -X -Y -Z
    int upAxis = 3;
    double Ns = 10.0;
    double Ni = 1.5;
    double d = 1.0;
    double Tr = 0.0;
    double Tf[3] = {1.0, 1.0, 1.0};
    double illum = 2.0;
    double Ka[3] = {0.588, 0.588, 0.588};
    double Kd[3] = {0.588, 0.588, 0.588};
    double Ks[3] = {0.588, 0.588, 0.588};
    double Ke[3] = {0.588, 0.588, 0.588};
    std::string map_Ka = "F22.jpg";
    std::string map_Kd = "F22.jpg";
    std::array<int, 3> thatCorrectFaceVertices = {1, 2, 4};
    int thatCorrectFaceIndex = 0;
    std::vector<Point> innerPoints;

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
};

bool readNewItem(const char *filename, Item &item);

#endif //VERTICES_ITEM_H
