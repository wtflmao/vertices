//
// Created by root on 2024/3/16.
//

#ifndef VERTICES_FIELD_H
#define VERTICES_FIELD_H

#include "node.h"
#include <array>
#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <queue>

bool readNewItem(const char *filename, Item& item);

class Field {
private:
    std::shared_ptr<Node> tree;
    std::vector<std::shared_ptr<Triangle> > allFaces;
    std::vector<Item> objects;

public:
    std::array<Point, 2> bounds;
    std::size_t nodeCount = 0;
    std::size_t maxDepth = 0;

    Field() noexcept;
    Field(Point min, Point max) noexcept;
    void buildBVHTree();
    //void insertObject(Item &item);
    bool insertObject(const std::string& objPath, const std::string& mtlPath, const std::array<double, 3> &scaleFactor, Point center);

    std::vector<Item> &getObjects();

    std::vector<std::shared_ptr<Node> > generateNodeList() const;

    void initPfaces(const std::vector<std::shared_ptr<Triangle> > &p_faces_t);

    void mergeFacesOfObjects();

    static std::array<std::size_t, 3> countLeafNodes(const std::shared_ptr<Node> &root);

    //static std::size_t countNodes(const std::unique_ptr<Node> &node);
};

#endif //VERTICES_FIELD_H