//
// Created by root on 2024/3/16.
//

#include "field.h"

Field::Field() noexcept
    : bounds({BigO, BigO}), tree() {

}

Field::Field(Point min, Point max) noexcept
    : bounds({min, max}), tree() {

}

void Field::initPfaces(const std::vector<std::shared_ptr<Triangle>> &p_faces_t) {
    allFaces = p_faces_t;
}

// make every face from all objects into one vector
void Field::build() {
    try {
        for (Item &object: objects) {
            auto ptrVec = object.getFaceRefs();

            if (!ptrVec.empty()) {
                allFaces.reserve( allFaces.size() + ptrVec.size());
                if (!ptrVec.empty()) {
                    allFaces.insert(allFaces.end(), ptrVec.begin(), ptrVec.end());
                }
            }
        }
    } catch (const std::length_error &e) {
        std::cerr << "Error in Field::build(): " << e.what() << std::endl;
    }
}

// calculate how many nodes in the tree
std::size_t Field::countLeafNodes(const std::unique_ptr<Node>& root)
{
    if(root == nullptr) return 0;

    std::size_t leafCount = 0;
    std::queue<Node*> nodeQueue;
    nodeQueue.push(root.get());

    while(!nodeQueue.empty())
    {
        const Node* currentNode = nodeQueue.front();
        nodeQueue.pop();

        bool isLeaf = true;
        for(auto& child : currentNode->children)
        {
            if(child != nullptr)
            {
                nodeQueue.push(child.get());
                isLeaf = false;
            }
        }

        if(isLeaf)
            leafCount++;
    }
    return leafCount;
}


void Field::buildBVHTree() {
    // this "tree" should be the root node
    //initPfaces(p_faces);
    build();
    tree = std::make_unique<Node>(allFaces, 1LL);
    tree->split();
    std::cout << "BVH tree has been built." << std::endl;
    nodeCount = countLeafNodes(tree);
    std::cout << "There are " << nodeCount << " nodes in the tree." << std::endl;
}

bool Field::insertObject(const std::string& objPath, const std::string& mtlPath, const std::array<double, 3>& scaleFactor, Point center) {
    objects.emplace_back();
    Item& object = objects.back();
    object.setScaleFactor(scaleFactor);
    object.setCenter(center);
    if (!readNewItem(objPath.c_str(), object)) {
        std::cerr << "Error when doing readNewItem(#" << objects.size() << ") in Field::insertObject(" << objPath << ")" << std::endl;
        return false;
    }

    if (!readNewItem(mtlPath.c_str(), object)) {
        std::cerr << "Error when doing readNewItem(#" << objects.size() << ") in Field::insertObject(" << mtlPath << ")" << std::endl;
        return false;
    }

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
