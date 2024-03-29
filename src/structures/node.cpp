//
// Created by root on 24-3-18.
//

#include "node.h"


Node::Node() noexcept {
    for (auto &child: children) {
        child = nullptr;
    }
    boxedFaces = {};
}

bool Node::isLeaf() const noexcept {
    return children[0] == nullptr;
}

bool Node::isEmpty() const noexcept {
    return isLeaf();
}

std::size_t Node::getDepth() const noexcept {
    return depth;
}

void Node::setDepth(std::size_t d) noexcept {
    depth = d;
}

const std::array<std::shared_ptr<Node>, BVH_NODE_CHILDREN> &Node::getChildren() const noexcept {
    return children;
}

void Node::updateBox() {
    auto &faces = boxedFaces;
    if (faces.empty()) {
        return;
    }

    Point min = faces[0]->v0;
    Point max = faces[0]->v0;

    for (const auto &face: faces) {
        for (const auto point: face->getVertices()) {
            min.x = std::min(min.x, point.get().x);
            min.y = std::min(min.y, point.get().y);
            min.z = std::min(min.z, point.get().z);
            max.x = std::max(max.x, point.get().x);
            max.y = std::max(max.y, point.get().y);
            max.z = std::max(max.z, point.get().z);
        }
    }
    bbox.setBounds(min, max);
}


// Stdev means standard deviation
double computeStdev(const std::vector<std::shared_ptr<Triangle> > &samples, int axis) {
    std::vector<double> positions(samples.size());
    std::ranges::transform(samples, positions.begin(), [axis](const std::shared_ptr<Triangle> &face) {
        return face->centroid[axis];
    });
    double mean = std::accumulate(positions.begin(), positions.end(), 0.0, std::plus<>()) / positions.size();
    const double variance = std::transform_reduce(positions.begin(), positions.end(), 0.0, std::plus<>(),
                                                  [mean](double x) { return (x - mean) * (x - mean); });
    return std::sqrt(variance / std::max(positions.size() - 1, static_cast<std::size_t>(1)));
}

static std::shared_ptr<Box> computeBox(const std::vector<std::shared_ptr<Triangle> > &faces) {
    if (faces.empty()) {
        return {};
    }

    // put the bbox on the heap
    auto box = std::make_shared<Box>();

    Point min = faces[0]->v0;
    Point max = faces[0]->v0;

    for (const auto &face: faces) {
        for (const auto point: face->getVertices()) {
            min.x = std::min(min.x, point.get().x);
            min.y = std::min(min.y, point.get().y);
            min.z = std::min(min.z, point.get().z);
            max.x = std::max(max.x, point.get().x);
            max.y = std::max(max.y, point.get().y);
            max.z = std::max(max.z, point.get().z);
        }
    }
    box->setBounds(min, max);
    return box;
}

Node::Node(const std::vector<std::shared_ptr<Triangle> > &faces, std::size_t child_depth) noexcept {
    for (auto &child: children) {
        child = nullptr;
    }
    boxedFaces = faces;
    bbox = *computeBox(faces);
    depth = child_depth;
}

void Node::split() {
    if (boxedFaces.size() < MAX_FACES_PER_BOX || getDepth() >= MAX_DEPTH) {
        return;
    }
    //std::cout << "111 HERE NORMAL" << std::endl;

    // calculate the bounds of the box
    updateBox();

    //std::cout << "222 HERE NORMAL" << std::endl;

    // use Monte-Carlo sampling to estimate the standard deviation
    std::vector<std::shared_ptr<Triangle> > samples;
    // maximum of 1024 sampling triangular faces per box
    samples.reserve(std::min(SAMPLINGS, boxedFaces.size()));
    std::ranges::sample(boxedFaces, std::back_inserter(samples), std::min(SAMPLINGS, boxedFaces.size()),
                        std::mt19937(rd()));
    std::array<double, 3> stdevs = {
        computeStdev(samples, 0),
        computeStdev(samples, 1),
        computeStdev(samples, 2)
    };

    //std::cout << "333 HERE NORMAL" << std::endl;

    // choose the axis with the largest standard deviation
    int splitAxis = std::distance(stdevs.begin(), std::ranges::max_element(stdevs));
    children_axis = splitAxis;

    // sort the faces along the chosen axis
    std::ranges::sort(boxedFaces, [splitAxis](const std::shared_ptr<Triangle> &a, const std::shared_ptr<Triangle> &b) {
        return a->centroid[splitAxis] < b->centroid[splitAxis];
    });

    // split the box
    const std::size_t steps = std::ceil(boxedFaces.size() / BVH_NODE_CHILDREN);

    //std::cout << "444 HERE NORMAL" << std::endl;
    for (int i = 0; i < BVH_NODE_CHILDREN; i++) {
        if (i != BVH_NODE_CHILDREN - 1) {
            children[i] = std::make_shared<Node>(
                std::vector(boxedFaces.begin() + i * steps, boxedFaces.begin() + (i + 1) * steps), getDepth() + 1);
        } else {
            children[i] = std::make_shared<Node>(std::vector(boxedFaces.begin() + i * steps, boxedFaces.end()),
                                                 getDepth() + 1);
        }
    }

    //std::cout << "555 HERE NORMAL" << std::endl;

    // update the children's bounds
    for (std::size_t i = 0; i < BVH_NODE_CHILDREN; i++) {
        children[i]->updateBox();
    }
    //std::cout << "666 HERE NORMAL" << std::endl;

    // recursively split the children
    for (std::size_t i = 0; i < BVH_NODE_CHILDREN; i++) {
        children[i]->split();
    }
}

