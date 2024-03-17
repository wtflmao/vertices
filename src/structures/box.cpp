//
// Created by root on 2024/3/16.
//

#include "box.h"

Box::Box(const Point &min, const Point &max) noexcept
    : bounds({min, max}) {
    for (auto &child : children) {
        child = nullptr;
    }
    boxedFaces = {};
}

Box::Box() noexcept {
    for (auto &child : children) {
        child = nullptr;
    }
    boxedFaces = {};
}

bool Box::isLeaf() const noexcept {
    return children[0] == nullptr;
}

bool Box::isEmpty() const noexcept {
    return isLeaf();
}

void Box::setBounds(const Point &min, const Point &max) noexcept {
    bounds[0] = min;
    bounds[1] = max;
}

void Box::setBounds(const std::array<Point, 2> &bounds_t) noexcept {
    bounds[0] = bounds_t[0];
    bounds[1] = bounds_t[1];
}

const std::array<Point, 2>& Box::getBounds() const noexcept {
    return bounds;
}

const std::array<std::unique_ptr<Box>, BVH_NODE_CHILDREN>& Box::getChildren() const noexcept {
    return children;
}

// make every face from all objects into one vector
void Box::build(std::vector<Item>& objects) {
    try {
        for (Item& object: objects) {
            if (!object.getFaceRefs().empty()) {
                const auto prevSize = boxedFaces.size();
                boxedFaces.resize(prevSize + object.getFaceRefs().size());
                std::copy(object.getFaceRefs().cbegin(), object.getFaceRefs().cend(), boxedFaces.begin() + prevSize);
            }
            // You could remove this line if you don't need it for debugging
            std::cout << "object size: "<<object.getFaceRefs().size() << std::endl;
        }
    } catch (const std::bad_alloc& e) {
        // Handle errors as per your requirement
    }
}

std::unique_ptr<Box> computeBox(const std::vector<Triangle*>& faces) {
    if (faces.empty()) {
        return {};
    }

    auto box = std::make_unique<Box>();

    Point min = faces[0]->v0;
    Point max = faces[0]->v0;

    for (const auto& face: faces) {
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
    box->boxedFaces = faces;
    return box;
}

void Box::updateBox() {
    const auto& faces = boxedFaces;
    if (faces.empty()) {
        return;
    }

    Point min = faces[0]->v0;
    Point max = faces[0]->v0;

    for (const auto& face: faces) {
        for (const auto point: face->getVertices()) {
            min.x = std::min(min.x, point.get().x);
            min.y = std::min(min.y, point.get().y);
            min.z = std::min(min.z, point.get().z);
            max.x = std::max(max.x, point.get().x);
            max.y = std::max(max.y, point.get().y);
            max.z = std::max(max.z, point.get().z);
        }
    }
    setBounds(min, max);
}

// Stdev means standard deviation
double computeStdev(const std::vector<Triangle*>& samples, int axis) {
    std::vector<double> positions(samples.size());
    std::transform(samples.begin(), samples.end(), positions.begin(), [axis](const Triangle* face) {
        return face->centroid[axis];
    });
    double mean = std::accumulate(positions.begin(), positions.end(), 0.0, std::plus<>()) / positions.size();
    double variance = std::transform_reduce(positions.begin(), positions.end(), 0.0, std::plus<>(), [mean](double x) { return (x - mean) * (x - mean);});
    return std::sqrt(variance / (positions.size() - 1));
}

void Box::split() {
    if (boxedFaces.size() < MAX_FACES_PER_BOX) {
        return;
    }

    // calculate the bounds of the box
    updateBox();

    // use Monte-Carlo sampling to estimate the standard deviation
    std::vector<Triangle*> samples;
    // maximum of 1000 sampling triangular faces per box
    samples.reserve(std::min(static_cast<size_t>(1000), boxedFaces.size()));
    std::sample(boxedFaces.begin(), boxedFaces.end(), samples.begin(), std::min(size_t(1000), boxedFaces.size()), std::mt19937(std::random_device()()));
    std::array<double, 3> stdevs = {
            computeStdev(samples, 0),
            computeStdev(samples, 1),
            computeStdev(samples, 2)
    };

    // choose the axis with the largest standard deviation
    int splitAxis = std::distance(stdevs.begin(), std::max_element(stdevs.begin(), stdevs.end()));

    // sort the faces along the chosen axis
    std::sort(boxedFaces.begin(), boxedFaces.end(), [splitAxis](const Triangle* a, const Triangle* b) {
        return a->centroid[splitAxis] < b->centroid[splitAxis];
    });

    // split the box
    std::size_t steps = boxedFaces.size() / BVH_NODE_CHILDREN;

    for (std::size_t i = 0; i < BVH_NODE_CHILDREN; i++) {
        if ( i != BVH_NODE_CHILDREN - 1) {
            children[i] = computeBox(std::vector<Triangle*>(boxedFaces.begin() + i * steps, boxedFaces.begin() + (i + 1) * steps));
        } else {
            children[i] = computeBox(std::vector<Triangle*>(boxedFaces.begin() + i * steps, boxedFaces.end()));
        }
    }

    // update the children's bounds
    for (std::size_t i = 0; i < BVH_NODE_CHILDREN; i++) {
        children[i]->updateBox();
    }

    // recursively split the children
    for (std::size_t i = 0; i < BVH_NODE_CHILDREN; i++) {
        if (!children[i]->isEmpty()) {
            children[i]->split();
        }
    }
}