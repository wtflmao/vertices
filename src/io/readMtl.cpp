//
// Created by root on 2024/3/15.
//

#include "readMtl.h"
void processUsemtl(const char* line, Item& p) {
    std::istringstream ss(line);
    std::string temp, usemtlName;
    ss >> temp >> usemtlName;
    p.mtlDataset.mtlGroupName = usemtlName;
}

bool processNewmtl(const char* line, Item& p) {
    std::istringstream ss(line);
    std::string temp, newMtlName;
    ss >> temp >> newMtlName;
    if (newMtlName == p.mtlDataset.mtlGroupName) {
        return true;
    }
    return false;
}

void processNs(const char* line, Item& p) {
    sscanf(line, "Ns %lf", &p.mtlDataset.Ns);
}

void processKa(const char* line, Item& p) {
    sscanf(line, "Ka %lf %lf %lf", &p.mtlDataset.Ka[0], &p.mtlDataset.Ka[1], &p.mtlDataset.Ka[2]);
}

void processKd(const char* line, Item& p) {
    sscanf(line, "Kd %lf %lf %lf", &p.mtlDataset.Kd[0], &p.mtlDataset.Kd[1], &p.mtlDataset.Kd[2]);
}

void processKs(const char* line, Item& p) {
    sscanf(line, "Ks %lf %lf %lf", &p.mtlDataset.Ks[0], &p.mtlDataset.Ks[1], &p.mtlDataset.Ks[2]);
}

void processKe(const char* line, Item& p) {
    sscanf(line, "Ke %lf %lf %lf", &p.mtlDataset.Ke[0], &p.mtlDataset.Ke[1], &p.mtlDataset.Ke[2]);
}

void processNi(const char* line, Item& p) {
    sscanf(line, "Ni %lf", &p.mtlDataset.Ni);
}

void processD(const char* line, Item& p) {
    sscanf(line, "d %lf", &p.mtlDataset.d);
}

void processIllum(const char* line, Item& p) {
    sscanf(line, "illum %d", &p.mtlDataset.illum);
}

void processMap_Kd(const char* line, Item& p) {
    std::istringstream ss(line);
    std::string temp;
    ss >> temp >> p.mtlDataset.map_Kd;
}

void processMap_Ka(const char* line, Item& p) {
    std::istringstream ss(line);
    std::string temp;
    ss >> temp >> p.mtlDataset.map_Ka;
}

void processTr(const char* line, Item& p) {
    sscanf(line, "Tr %lf", &p.mtlDataset.Tr);
}

void processTf(const char* line, Item& p) {
    sscanf(line, "Tf %lf %lf %lf", &p.mtlDataset.Tf[0], &p.mtlDataset.Tf[1], &p.mtlDataset.Tf[2]);
}
