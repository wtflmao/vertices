//
// Created by root on 2024/3/15.
//

#ifndef VERTICES_ITEM_H
#define VERTICES_ITEM_H

#include <vector>
#include <string>
#include "triangle.h"

class Item {
private:

    std::vector<Triangle> faces;
    bool isNilItem = false;
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

    explicit Item(bool nilItem);
    [[nodiscard]] const std::vector<Triangle> &getFaces() const;

    std::vector<Triangle> &getMutFaces();

    [[nodiscard]] bool isNil() const;
};

//Item NIL = Item(true);

#endif //VERTICES_ITEM_H
