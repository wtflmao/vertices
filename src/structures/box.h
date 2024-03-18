//
// Created by root on 2024/3/16.
//

#ifndef VERTICES_BOX_H
#define VERTICES_BOX_H

#include "item.h"
#include <iostream>//for debug purpose
#include <memory>
#include <random>


// BVH node as box
class Box {
private:

public:
    // bounds[0]: (minX, minY, minZ), bounds[1]: (maxX, maxY, maxZ)
    std::array<Point, 2> bounds;

    Box() noexcept;


    Box(const Point &min, const Point &max) noexcept;
    void setBounds(const Point &min, const Point &max) noexcept;
    void setBounds(const std::array<Point, 2> &bounds_t) noexcept;
    [[nodiscard]] const std::array<Point, 2> &getBounds() const noexcept;
    [[nodiscard]] bool isEmpty() const noexcept;
};

#endif //VERTICES_BOX_H
