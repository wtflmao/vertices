//
// Created by root on 2024/3/16.
//

/* Copyright 2024 wtflmao. All Rights Reserved.
 *
 * Distributed under MIT license.
 * See file LICENSE/LICENSE.MIT.md or copy at https://opensource.org/license/mit
 */

#include "field.h"

#include <utility>

Field::Field() noexcept
    : bounds({BigO, BigO}), tree() {
}

Field::Field(Point min, Point max) noexcept
    : bounds({min, max}), tree() {
}

void Field::initPfaces(const std::vector<std::shared_ptr<Triangle> > &p_faces_t) {
    allFaces = p_faces_t;
}

// make every face from all objects into one vector
void Field::mergeFacesOfObjects() {
    try {
        for (Item &object: objects) {
            auto ptrVec = object.getFaceRefs();

            if (!ptrVec.empty()) {
                auto filteredFaces = new std::vector<std::shared_ptr<Triangle> >();
                for (const auto &face: ptrVec) {
                    if ([&face, this]() {
                        auto centroid = face->getCentroid();
                        if (centroid.getX() >= bounds[0].getX() && centroid.getX() <= bounds[1].getX()
                            && centroid.getY() >= bounds[0].getY() && centroid.getY() <= bounds[1].getY()
                            && centroid.getZ() >= bounds[0].getZ() && centroid.getZ() <= bounds[1].getZ())
                            return true;
                        return false;
                    }())
                        filteredFaces->push_back(face);
                }
                allFaces.reserve(allFaces.size() + filteredFaces->size());
                if (!filteredFaces->empty()) {
                    allFaces.insert(allFaces.end(), filteredFaces->begin(), filteredFaces->end());
                }
                delete filteredFaces;
                //allFaces.reserve(allFaces.size() + ptrVec.size());
                //allFaces.insert(allFaces.end(), ptrVec.begin(), ptrVec.end());
            }
        }
    } catch (const std::exception &e) {
        std::cerr << "Error in Field::mergeFacesOfObjects(): " << e.what() << std::endl;
    }
}

// calculate how many nodes in the tree
std::array<std::size_t, 3> Field::countLeafNodes(const std::shared_ptr<Node> &root) {
    if (root == nullptr) return {0, 0, 1};

    std::queue<std::shared_ptr<Node> > nodeQueue;
    std::size_t leafCount = 0;
    std::size_t nodeCount = 0;
    std::size_t maxDepth_t = 0;
    nodeQueue.push(root);

    while (!nodeQueue.empty()) {
        const auto currentNode = nodeQueue.front();
        nodeQueue.pop();

        bool isLeaf = true;
        for (const auto &child: currentNode->children) {
            if (child != nullptr) {
                nodeQueue.push(child);
                //nodeList.emplace_back(child);
                isLeaf = false;
            }
        }

        if (isLeaf)
            leafCount++;
        nodeCount++;

        if (currentNode->depth > maxDepth_t) {
            maxDepth_t = currentNode->depth;
        }
    }
    return std::array{leafCount, nodeCount, maxDepth_t};
}


void Field::buildBVHTree() {
    // this "tree" should be the root node
    //initPfaces(p_faces);
    mergeFacesOfObjects();
    coutLogger->writeInfoEntry("There are " + std::to_string(allFaces.size()) + " faces in the boundry");
    tree = std::make_shared<Node>(allFaces, 1LL);
    tree->split();
    std::cout << "BVH tree has been built." << std::endl;
    const std::array<std::size_t, 3> nodeCountArr = countLeafNodes(tree);
    std::cout << "There are " << (nodeCount = nodeCountArr[1]) << " nodes and " << nodeCountArr[0] <<
            " leaf nodes in the tree." << std::endl;
    std::cout << "Root node's height is " << (maxDepth = nodeCountArr[2]) << std::endl;
}

Item &Field::newOpenObject() noexcept {
    objects.emplace_back();
    objects.back().isOpenMesh = true;
    return objects.back();
}

Item &Field::newClosedObject() noexcept {
    objects.emplace_back();
    objects.back().isOpenMesh = false;
    return objects.back();
}

// this "allFaces" is the vector of all faces IN THE BOUNDRY
int Field::getAllFacesSize() const noexcept
{
    return allFaces.size();
}

const Point& Field::getBoundsMin() const noexcept
{
    return bounds[0];
}

const Point& Field::getBoundsMax() const noexcept
{
    return bounds[1];
}

[[deprecated]] bool Field::insertObject(const std::string &objPath, const std::string &mtlPath,
                                        const std::array<double, 3>& scaleFactor, Point center,
                                        std::array<int, 3> correctFaceVertices,
                                        int tCFI, std::vector<Point> innerPoints, bool isOpenMesh, int forwardAxis,
                                        int upAxis)
{
    objects.emplace_back();
    Item& object = objects.back();
    object.setScaleFactor(scaleFactor);
    object.setCenter(center);
    object.forwardAxis = forwardAxis;
    object.upAxis = upAxis;
    if (!readNewItem(objPath.c_str(), object)) {
        std::cerr << "Error when doing readNewItem(#" << objects.size() << ") in Field::insertObject(" << objPath << ")" << std::endl;
        return false;
    }

    if (!readNewItem(mtlPath.c_str(), object)) {
        std::cerr << "Error when doing readNewItem(#" << objects.size() << ") in Field::insertObject(" << mtlPath << ")"
                << std::endl;
        return false;
    }

    if (isOpenMesh) {
        object.thatCorrectFaceVertices = correctFaceVertices;
        object.thatCorrectFaceIndex = tCFI;
        object.isOpenMesh = true;
    } else {
        object.innerPoints = std::move(innerPoints);
    }

    // check normal vec's directon is correct or not, if not, revise it
    // u need to EXPLICITLY call it or ir won't be called automatically due to the tedious preparition of tCFI and innerPoints
    //object.inspectNormalVecForAllFaces();

    // get its boundry box for some reason
    object.updateRawBoundary();
    const auto rawBoundary = object.getRawBoundary();

    std::cout<<"Object #"<<objects.size()<<" has "<<object.getFaces().size()<<" faces."<<std::endl;
    std::cout<<"Object #"<<objects.size()<<" has "<<object.getVertices().size()<<" vertices."<<std::endl;
    std::cout << "Object #" << objects.size() << "'s minRawBound is " << Point{
        rawBoundary[0], rawBoundary[1], rawBoundary[2]
    } << std::endl;
    std::cout << "Object #" << objects.size() << "'s maxRawBound is " << Point{
        rawBoundary[3], rawBoundary[4], rawBoundary[5]
    } << std::endl;
    //std::cout<<"Object #"<<objects.size()<<"'s refraction index is "<<object.Ni<<std::endl;
    //std::cout<<"Object #"<<objects.size()<<"'s Ka is "<<object.Ka[0]<<" "<<object.Ka[1]<<" "<<object.Ka[2]<<std::endl;
    //std::cout<<"Object #"<<objects.size()<<"'s Kd is "<<object.Kd[0]<<" "<<object.Kd[1]<<" "<<object.Kd[2]<<std::endl;
    //std::cout<<"Object #"<<objects.size()<<"'s Ks is "<<object.Ks[0]<<" "<<object.Ks[1]<<" "<<object.Ks[2]<<std::endl;
    std::cout<<std::endl;

    return true;
}

std::vector<Item> &Field::getObjects() {
    return objects;
}

const std::vector<std::shared_ptr<Triangle>>& Field::getAllFaces() const noexcept {
    return allFaces;
}

// BFS Traversal method, same as the leaf-counting function above
std::vector<std::shared_ptr<Node> > Field::generateNodeList() const {
    // Create empty vector for bfs nodes
    std::vector<std::shared_ptr<Node> > bfs_nodes;

    // Base case: If tree is null
    if (tree == nullptr)
        return bfs_nodes;

    // Create a queue for BFS
    std::queue<std::shared_ptr<Node> > nodeQueue;

    nodeQueue.push(tree);

    while (!nodeQueue.empty()) {
        // Dequeue a node from front and push it into the vector
        std::shared_ptr<Node> current_node = nodeQueue.front();
        bfs_nodes.push_back(current_node);

        nodeQueue.pop();

        // Enqueue all children of the dequeued node
        for (auto &child: current_node->children) {
            if (child != nullptr) {
                nodeQueue.push(child);
            }
        }
    }

    return bfs_nodes;
}
