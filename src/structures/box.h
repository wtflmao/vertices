//
// Created by root on 2024/3/16.
//

#ifndef VERTEX_BOX_H
#define VERTEX_BOX_H

#include "point.h"
#include "item.h"
#include <memory>

constexpr std::size_t BVH_NODE_CHILDREN = 4;

// BVH node as box
class Box {
private:

public:
    std::array<std::unique_ptr<Box>, BVH_NODE_CHILDREN> children;
    std::array<Point, 2> bounds;
    std::size_t facesCount = 0;

    Box() noexcept;
    Box(const Point &min, const Point &max) noexcept;
    //[[nodiscard]] bool isInternal() const noexcept;
    [[nodiscard]] bool isRoot() const noexcept;
    [[nodiscard]] bool isLeaf() const noexcept;
    void setBounds(const Point &min, const Point &max) noexcept;
    void setBounds(const std::array<Point, 2> &bounds_t) noexcept;
    [[nodiscard]] const std::array<Point, 2> &getBounds() const noexcept;
    [[nodiscard]] const std::array<std::unique_ptr<Box>, BVH_NODE_CHILDREN> &getChildren() const noexcept;
    [[nodiscard]] bool isEmpty() const noexcept;
    void build(std::vector<Item> &objects);
};

#endif //VERTEX_BOX_H