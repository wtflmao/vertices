//
// Created by root on 24-5-2.
//

#ifndef VERTICES_MTLDATASET_H
#define VERTICES_MTLDATASET_H

class MTLDataset final {
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
     * Ke 0.0000000 0.0000000 0.0000023: emissive color (Physically-based rendering extenxsion)
     * map_Ka F22.jpg: 使用名为"F22.jpg"的纹理贴图作为环境光反射贴图。
     * map_Kd F22.jpg: 使用同样的"F22.jpg"纹理作为漫反射贴图。
     */
    double Ns = 10.0;
    double Ni = 0.0;
    double d = 0.996;
    double Tr = 0.0;
    double Tf[3] = {1.0, 1.0, 1.0};
    int illum = 2;
    double Ka[4] = {1.0, 1.0, 1.0, 0.0};
    double Kd[4] = {0.8, 0.8, 0.8, 0.0};
    double Ks[4] = {0.5, 0.5, 0.5, 0.0};
    double Ke[4] = {0.8, 0.8, 0.8, 0.0};
    std::string map_Ka = "F22.jpg";
    std::string map_Kd = "F22.jpg";
    std::string mtlGroupName = "default";
};



#endif //VERTICES_MTLDATASET_H
