//
// Created by root on 2024/3/15.
//

#include "readMtl.h"
void processNs(const char* line, Item& p) {
    sscanf(line, "Ns %lf", &p.Ns);
}

void processKa(const char* line, Item& p) {
    sscanf(line, "Ka %lf %lf %lf", &p.Ka[0], &p.Ka[1], &p.Ka[2]);
}

void processKd(const char* line, Item& p) {
    sscanf(line, "Kd %lf %lf %lf", &p.Kd[0], &p.Kd[1], &p.Kd[2]);
}

void processKs(const char* line, Item& p) {
    sscanf(line, "Ks %lf %lf %lf", &p.Ks[0], &p.Ks[1], &p.Ks[2]);
}

void processKe(const char* line, Item& p) {
    sscanf(line, "Ke %lf %lf %lf", &p.Ke[0], &p.Ke[1], &p.Ke[2]);
}

void processNi(const char* line, Item& p) {
    sscanf(line, "Ni %lf", &p.Ni);
}

void processD(const char* line, Item& p) {
    sscanf(line, "d %lf", &p.d);
}

void processIllum(const char* line, Item& p) {
    sscanf(line, "illum %lf", &p.illum);
}

void processMap_Kd(const char* line, Item& p) {
    std::istringstream ss(line);
    std::string temp;
    ss >> temp >> p.map_Kd;
}

void processMap_Ka(const char* line, Item& p) {
    std::istringstream ss(line);
    std::string temp;
    ss >> temp >> p.map_Ka;
}

void processTr(const char* line, Item& p) {
    sscanf(line, "Tr %lf", &p.Tr);
}

void processTf(const char* line, Item& p) {
    sscanf(line, "Tf %lf %lf %lf", &p.Tf[0], &p.Tf[1], &p.Tf[2]);
}
