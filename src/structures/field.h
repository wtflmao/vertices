//
// Created by root on 2024/3/16.
//

/* Copyright 2024 wtflmao. All Rights Reserved.
 *
 * Distributed under MIT license.
 * See file LICENSE/LICENSE.MIT.md or copy at https://opensource.org/license/mit
 */

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

class Field final {
private:
    std::shared_ptr<Node> tree;
    std::vector<std::shared_ptr<Triangle> > allFaces;
    std::vector<Item> objects;
    std::array<Point, 2> bounds;

public:
    std::size_t nodeCount = 0;
    std::size_t maxDepth = 0;
    const Vec sunlightDirectionToGround = Vec(Point(-1, 0, -std::sqrt(3)));
    std::vector<BRDF *> brdfList = {};

    Field() noexcept;
    Field(Point min, Point max) noexcept;
    void buildBVHTree();
    //void insertObject(Item &item);
    [[deprecated]] bool
    insertObject(const std::string &objPath, const std::string &mtlPath, const std::array<double, 3> &scaleFactor,
                      Point center, std::array<int, 3> correctFaceVertices, int tCFI, std::vector<Point> innerPoints,
                      bool isOpenMesh, int
                      forwardAxis, int upAxis);

    std::vector<Item> &getObjects();

    const std::vector<std::shared_ptr<Triangle>>& getAllFaces() const noexcept;

    [[nodiscard]] std::vector<std::shared_ptr<Node> > generateNodeList() const;

    void initPfaces(const std::vector<std::shared_ptr<Triangle> > &p_faces_t);

    void mergeFacesOfObjects();

    static std::array<std::size_t, 3> countLeafNodes(const std::shared_ptr<Node> &root);

    //static std::size_t countNodes(const std::unique_ptr<Node> &node);

    Item &newOpenObject() noexcept;

    Item &newClosedObject() noexcept;

    [[nodiscard]] int getAllFacesSize() const noexcept;

    [[nodiscard]] const Point& getBoundsMin() const noexcept;

    [[nodiscard]] const Point& getBoundsMax() const noexcept;
};

#endif //VERTICES_FIELD_H