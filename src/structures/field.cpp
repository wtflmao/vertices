//
// Created by root on 2024/3/16.
//

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
                allFaces.reserve(allFaces.size() + ptrVec.size());
                if (!ptrVec.empty()) {
                    allFaces.insert(allFaces.end(), ptrVec.begin(), ptrVec.end());
                }
            }
        }
    } catch (const std::length_error &e) {
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
    tree = std::make_shared<Node>(allFaces, 1LL);
    tree->split();
    std::cout << "BVH tree has been built." << std::endl;
    const std::array<std::size_t, 3> nodeCountArr = countLeafNodes(tree);
    std::cout << "There are " << (nodeCount = nodeCountArr[1]) << " nodes and " << nodeCountArr[0] <<
            " leaf nodes in the tree." << std::endl;
    std::cout << "Root node's height is " << (maxDepth = nodeCountArr[2]) << std::endl;
}

bool Field::insertObject(const std::string &objPath, const std::string &mtlPath,
                         const std::array<double, 3> &scaleFactor, Point center, std::array<int, 3> correctFaceVertices,
                         int tCFI, std::vector<Point> innerPoints, bool isOpenMesh, int forwardAxis, int upAxis) {
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
    object.normalVecInspector();

    std::cout<<"Object #"<<objects.size()<<" has "<<object.getFaces().size()<<" faces."<<std::endl;
    std::cout<<"Object #"<<objects.size()<<" has "<<object.getVertices().size()<<" vertices."<<std::endl;
    std::cout<<"Object #"<<objects.size()<<"'s refraction index is "<<object.Ni<<std::endl;
    std::cout<<"Object #"<<objects.size()<<"'s Ka is "<<object.Ka[0]<<" "<<object.Ka[1]<<" "<<object.Ka[2]<<std::endl;
    std::cout<<"Object #"<<objects.size()<<"'s Kd is "<<object.Kd[0]<<" "<<object.Kd[1]<<" "<<object.Kd[2]<<std::endl;
    std::cout<<"Object #"<<objects.size()<<"'s Ks is "<<object.Ks[0]<<" "<<object.Ks[1]<<" "<<object.Ks[2]<<std::endl;
    std::cout<<std::endl;

    return true;
}

std::vector<Item> &Field::getObjects() {
    return objects;
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
