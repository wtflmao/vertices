//
// Created by root on 2024/3/15.
//


#ifndef VERTICES_READLINES_H
#define VERTICES_READLINES_H

#include "readFace.h"
#include "readTexture.h"
#include "readVertex.h"
#include "readMtl.h"
#include "readNormal.h"

void processLine(const char* line, Item& p_item);

#endif //VERTICES_READLINES_H