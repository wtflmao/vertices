//
// Created by root on 24-3-18.
//

#ifndef VERTICES_NODE_H
#define VERTICES_NODE_H

#include "../common.h"
#include "box.h"
#include <numeric>


/*constexpr std::size_t BVH_NODE_CHILDREN = 6;
constexpr std::size_t MAX_FACES_PER_BOX = 32;
constexpr std::size_t MAX_DEPTH = 12;
constexpr std::size_t SAMPLINGS = 64;*/

// node is for BVH Tree(Bounding Volume Hierarchy Tree)\
// box is box, node is node
class Node {
public:
    // bbox stands for "bounding box"
    Box bbox;
    std::vector<std::shared_ptr<Triangle> > boxedFaces;
    std::array<std::shared_ptr<Node>, BVH_NODE_CHILDREN> children;
    std::size_t depth = 0;
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

#if VERTICES_CONFIG_CXX_STANDARD <= 17
    template<typename Iter>
    [[nodiscard]] static std::vector<typename Iter::value_type> sample(Iter begin, Iter end, std::size_t num_random_samples)
    {
        std::size_t size = std::distance(begin, end);
        if (num_random_samples > size)
            num_random_samples = size;

        std::vector<typename Iter::value_type> res;
        res.reserve(num_random_samples);

        while(num_random_samples--)
        {
            std::uniform_int_distribution<> dis(0, --size);
            std::advance(begin, dis(generator));
            res.push_back(*begin);
            begin = end;    //reset iterator
        }
        return res;
    }

#endif
};

#endif //VERTICES_NODE_H
