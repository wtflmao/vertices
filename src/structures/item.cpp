//
// Created by root on 2024/3/15.
//

#include "item.h"
/*
 * v：顶点(Vertex)定义
 * 这个语句后跟3个浮点数,分别表示顶点的x、y、z坐标。
 * 例如: v 1.0 2.0 3.0
 *
 * vn：法线(Normal)定义
 * 这个语句后跟3个浮点数,表示顶点的法线向量的x、y、z分量。
 * 例如: vn 0.707 0.0 0.707
 *
 * vt：纹理坐标(Texture Coordinate)定义
 * 这个语句后跟2个浮点数,表示顶点的u、v纹理坐标。
 * 例如: vt 0.5 0.75
 *
 * f：面(Face)定义
 * 这个语句用来定义三角面片,包含3个或更多的顶点索引。
 * 顶点索引可以包含纹理坐标和法线索引,用正斜杠分隔。
 * 例如: f 1/1/1 2/2/2 3/3/3
 * 1/1/1 表示: 顶点v索引为1 纹理vt坐标索引为1 法线vn索引也为1
 */


/*Item::Item() {
    // Initialize member variables here, but no 'return' statement
}*/

const std::vector<Triangle>& Item::getFaces() const {
    return faces;
}

std::vector<Triangle>& Item::getMutFaces() {
    return faces;
}

Item::Item(bool nilItem) {
    isNilItem = nilItem;
}

bool Item::isNil() const {
    return isNilItem;
}