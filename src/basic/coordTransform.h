//
// Created by root on 24-3-26.
//

#ifndef VERTEX_COORDTRANSFORM_H
#define VERTEX_COORDTRANSFORM_H

#define EIGEN_MPL2_ONLY

#include <iostream>
#include "../../lib/eigen3/Eigen/Dense"
#include "../structures/point.h"
#include "../../lib/transform3d/transforms3d.h"
#include <valarray>

//using namespace std;
//using namespace Eigen;
//using Eigen::MatrixXd;

class CoordTransform {
public:
    CoordTransform();

private:
    // pitch, yaw, roll are for x, y, and z axis rotation angle respectively
    // units are in degrees, not radians
    double pitch = 180;
    double yaw = 0;
    double roll = 180;

    Eigen::Matrix3d rotMat;

    // pan matrix, applied after rotation, applied on the BigO, direction is original axis's direction
    Eigen::Matrix3d panMatrix;

    // zoom vector, refers to what scale should be applied to every new axis
    Eigen::Matrix3d scaleMatrix;

    // why dont I use vector above? IDK, but it is easier to remember

    // three origin points, every element contains x,y,z double
    Eigen::Vector3d pointsInOriginal_std[3];
    Eigen::Vector3d pointsInNew_std[3];

public:
    //设置标定点原始点1 2 3
    void setOriPoint(const Point &origin, int whichOne);

    //设置标定点当前点1 2 3
    void setNewPoint(const Point &newPoint, int whichOne);

    //计算
    void calcTheRotMat();

private:
};


#endif //VERTEX_COORDTRANSFORM_H
