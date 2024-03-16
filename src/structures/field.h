//
// Created by root on 2024/3/16.
//

#ifndef VERTICES_FIELD_H
#define VERTICES_FIELD_H

#include "box.h"
#include "item.h"
#include <array>
#include <vector>
#include <string>
#include <iostream>

bool readNewItem(const char *filename, Item& item);

class Field {
private:
    std::unique_ptr<Box> tree;

public:
    std::array<Point, 2> bounds;
    std::vector<Item> objects;
    Field() noexcept;
    Field(Point min, Point max) noexcept;
    void buildBVHTree();
    //void insertObject(Item &item);
    bool insertObject(const std::string& objPath, const std::string& mtlPath, const std::array<double, 3> &scaleFactor, Point center);
};

#endif //VERTICES_FIELD_H