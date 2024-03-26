//
// Created by root on 24-3-26.
//

#include "coordTransform.h"


CoordTransform::CoordTransform() {
}


//给6个标定点赋初值
void CoordTransform::setOriPoint(const Point &origin, const int whichOne) {
    if (whichOne < 1 || whichOne > 3) {
        fprintf(stderr, "GOSH DARN IT!!!\a in CoordTransform::setOriPoint() whichOne should between 1 and 3\n");
        exit(0);
    }
    pointsInOriginal_std[whichOne] = Eigen::Vector3d(origin.x, origin.y, origin.z);
}


void CoordTransform::setNewPoint(const Point &newPoint, const int whichOne) {
    if (whichOne < 1 || whichOne > 3) {
        fprintf(stderr, "GOSH DARN IT!!!\a in CoordTransform::setNewPoint() whichOne should between 1 and 3\n");
        exit(0);
    }
    pointsInNew_std[whichOne] = Eigen::Vector3d(newPoint.x, newPoint.y, newPoint.z);
}

void CoordTransform::calcTheRotMat() {
    rotMat = TransForms::EulerAngle2Mat(pitch, yaw, roll);
}


