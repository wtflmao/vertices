//
// Created by root on 2024/3/15.
//


#ifndef VERTICES_READMTL_H
#define VERTICES_READMTL_H

#include "readNewItem.h"
#include "../structures/item.h"

#include <sstream>

void processNs(const char*, Item&);
void processKa(const char*, Item&);
void processKd(const char*, Item&);
void processKs(const char*, Item&);
void processKe(const char*, Item&);
void processNi(const char*, Item&);
void processTr(const char*, Item&);
void processTf(const char*, Item&);
void processD(const char*, Item&);
void processIllum(const char*, Item&);
void processMap_Kd(const char*, Item&);
void processMap_Ka(const char*, Item&);

#endif //VERTICES_READMTL_H