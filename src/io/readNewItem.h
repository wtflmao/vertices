//
// Created by root on 2024/3/15.
//

#ifndef VERTICES_READNEWITEM_H
#define VERTICES_READNEWITEM_H

#include "../common.h"
#include "../structures/item.h"

#include <cstdio>
#include <cstring>

#define MAX_FILE_SIZE (100 * 1024 * 1024)
#define MAX_LINE_LENGTH 64

void processLine(const char* line, Item& p_item);

#endif //VERTICES_READNEWITEM_H