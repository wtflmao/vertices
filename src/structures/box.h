//
// Created by root on 2024/3/16.
//

#ifndef VERTICES_BOX_H
#define VERTICES_BOX_H

#include "item.h"
#include <iostream>//for debug purpose
#include <memory>
#include <random>


constexpr std::size_t BVH_NODE_CHILDREN = 4;
constexpr std::size_t MAX_FACES_PER_BOX = 5;
// BVH node as box
class Box {
private:

public:
    // bounds[0]: (minX, minY, minZ), bounds[1]: (maxX, maxY, maxZ)
    std::array<Point, 2> bounds;
    std::array<std::unique_ptr<Box>, BVH_NODE_CHILDREN> children;
    std::vector<Triangle*> boxedFaces;

    Box() noexcept;
    Box(const Point &min, const Point &max) noexcept;
    //[[nodiscard]] bool isInternal() const noexcept;
    //[[nodiscard]] bool isRoot() const noexcept;
    [[nodiscard]] bool isLeaf() const noexcept;
    void setBounds(const Point &min, const Point &max) noexcept;
    void setBounds(const std::array<Point, 2> &bounds_t) noexcept;
    [[nodiscard]] const std::array<Point, 2> &getBounds() const noexcept;
    [[nodiscard]] const std::array<std::unique_ptr<Box>, BVH_NODE_CHILDREN> &getChildren() const noexcept;
    [[nodiscard]] bool isEmpty() const noexcept;
    void build(std::vector<Item> &objects);

    void updateBox();

    void split();

};

#endif //VERTICES_BOX_H