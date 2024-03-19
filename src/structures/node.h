//
// Created by root on 24-3-18.
//

#ifndef VERTICES_NODE_H
#define VERTICES_NODE_H


#include "box.h"


constexpr std::size_t BVH_NODE_CHILDREN = 4;
constexpr std::size_t MAX_FACES_PER_BOX = 16;
constexpr std::size_t MAX_DEPTH = 12;
constexpr std::size_t SAMPLINGS = 8192;

// node is for BVH Tree(Bounding Volume Hierarchy Tree)\
// box is box, node is node
class Node {
public:
    // bbox stands for "bounding box"
    Box bbox;
    std::vector<std::shared_ptr<Triangle> > boxedFaces;
    std::array<std::shared_ptr<Node>, BVH_NODE_CHILDREN> children;
    std::size_t depth = 0;
    std::random_device rd = std::random_device();
    int children_axis = -1;


    Node() noexcept;

    //explicit Node(const std::vector<Triangle*> &faces, std::size_t child_depth) noexcept;

    [[nodiscard]] bool isLeaf() const noexcept;

    [[nodiscard]] bool isEmpty() const noexcept;

    [[nodiscard]] std::size_t getDepth() const noexcept;

    void setDepth(std::size_t d) noexcept;

    [[nodiscard]] const std::array<std::shared_ptr<Node>, BVH_NODE_CHILDREN> &getChildren() const noexcept;

    void updateBox();

    Node(const std::vector<std::shared_ptr<Triangle> > &faces, std::size_t child_depth) noexcept;

    void split();

    //static std::unique_ptr<Box> computeBox(const std::vector<Triangle *> &faces);
};


#endif //VERTICES_NODE_H
