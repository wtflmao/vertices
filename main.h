//
// Created by root on 2024/3/16.
//

#ifndef VERTICES_MAIN_H
#define VERTICES_MAIN_H

#include "src/common.h"
#include "src/structures/ray.h"
#include "src/structures/triangle.h"
#include "src/structures/item.h"

#include <iostream>

bool readNewItem(const char *filename, Item& item);

const Point NO_INTERSECT = Point(DBL_MAX, DBL_MAX, DBL_MAX);

#endif //VERTICES_MAIN_H