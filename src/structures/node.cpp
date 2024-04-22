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

    Point min = faces[0]->getV0();
    Point max = faces[0]->getV0();

    for (const auto &face: faces) {
        for (const auto point: face->getVertices()) {
            min.setX(std::min(min.getX(), point.get().getX()));
            min.setY(std::min(min.getY(), point.get().getY()));
            min.setZ(std::min(min.getZ(), point.get().getZ()));
            max.setX(std::max(max.getX(), point.get().getX()));
            max.setY(std::max(max.getY(), point.get().getY()));
            max.setZ(std::max(max.getZ(), point.get().getZ()));
        }
    }
    bbox.setBounds(min, max);
}


// Stdev means standard deviation
double computeStdev(const std::vector<std::shared_ptr<Triangle> > &samples, int axis) {
    std::vector<double> positions(samples.size());
#if VERTICES_CONFIG_CXX_STANDARD >= 20
    std::ranges::transform(samples, positions.begin(), [axis](const std::shared_ptr<Triangle> &face) {
        return face->centroid[axis];
    });
#elif VERTICES_CONFIG_CXX_STANDARD <= 17
    std::transform(samples.begin(), samples.end(), positions.begin(), [axis](const std::shared_ptr<Triangle> &face) {
        return face->centroid[axis];
    });
#endif

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

    Point min = faces[0]->getV0();
    Point max = faces[0]->getV0();

    for (const auto &face: faces) {
        for (const auto point: face->getVertices()) {
            min.setX(std::min(min.getX(), point.get().getX()));
            min.setY(std::min(min.getY(), point.get().getY()));
            min.setZ(std::min(min.getZ(), point.get().getZ()));
            max.setX(std::max(max.getX(), point.get().getX()));
            max.setY(std::max(max.getY(), point.get().getY()));
            max.setZ(std::max(max.getZ(), point.get().getZ()));
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
#if VERTICES_CONFIG_CXX_STANDARD >= 20
    std::ranges::sample(boxedFaces, std::back_inserter(samples), std::min(SAMPLINGS, boxedFaces.size()),
                        generator);
#elif VERTICES_CONFIG_CXX_STANDARD <= 17
    std::sample(boxedFaces.begin(), boxedFaces.end(), std::back_inserter(samples),
                std::min(SAMPLINGS, boxedFaces.size()), generator);
#endif
    std::array<double, 3> stdevs = {
        computeStdev(samples, 0),
        computeStdev(samples, 1),
        computeStdev(samples, 2)
    };

    //std::cout << "333 HERE NORMAL" << std::endl;

    // choose the axis with the largest standard deviation
#if VERTICES_CONFIG_CXX_STANDARD >= 20
    int splitAxis = std::distance(stdevs.begin(), std::ranges::max_element(stdevs));
#elif VERTICES_CONFIG_CXX_STANDARD <= 17
    int splitAxis = std::distance(stdevs.begin(), std::max_element(stdevs.begin(), stdevs.end()));
#endif
    children_axis = splitAxis;

    // sort the faces along the chosen axis
#if VERTICES_CONFIG_CXX_STANDARD >= 20
    std::ranges::sort(boxedFaces, [splitAxis](const std::shared_ptr<Triangle> &a, const std::shared_ptr<Triangle> &b) {
        return a->centroid[splitAxis] < b->centroid[splitAxis];
    });
#elif VERTICES_CONFIG_CXX_STANDARD <= 17
    std::sort(boxedFaces.begin(), boxedFaces.end(),
              [splitAxis](const std::shared_ptr<Triangle> &a, const std::shared_ptr<Triangle> &b) {
                  return a->centroid[splitAxis] < b->centroid[splitAxis];
              });
#endif

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

