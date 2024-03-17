//
// Created by root on 2024/3/16.
//

#ifndef VERTICES_FIELD_H
#define VERTICES_FIELD_H

#include "box.h"
#include <array>
#include <vector>
#include <string>
#include <iostream>
#include <memory>

constexpr std::size_t randomPicks = 1000LL;

bool readNewItem(const char *filename, Item& item);

class Field {
private:
    std::unique_ptr<Box> tree;
    std::vector<Triangle *> p_faces;
    std::vector<Item> objects;

public:
    std::array<Point, 2> bounds;
    Field() noexcept;
    Field(Point min, Point max) noexcept;
    void buildBVHTree();
    //void insertObject(Item &item);
    bool insertObject(const std::string& objPath, const std::string& mtlPath, const std::array<double, 3> &scaleFactor, Point center);

    std::vector<Item> &getObjects();

    void initPfaces(const std::vector<Triangle *> &p_faces_t);
};

#endif //VERTICES_FIELD_H