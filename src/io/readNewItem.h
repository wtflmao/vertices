//
// Created by root on 2024/3/15.
//

#ifndef VERTICES_READNEWITEM_H
#define VERTICES_READNEWITEM_H

#include "../common.h"
#include "../structures/item.h"

#include <cstdio>
#include <cstring>

constexpr size_t MAX_FILE_SIZE (4LL * 100LL * 1024LL * 1024LL);
constexpr size_t MAX_LINE_LENGTH (64);

void processLine(const char* line, Item& p_item);

#endif //VERTICES_READNEWITEM_H