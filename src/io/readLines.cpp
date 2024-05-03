//
// Created by root on 2024/3/15.
//

#include "readLines.h"


void processLine(const char* line, Item& p_item) {
    if (line == nullptr) {
        return;
    }

    // to check if it is a face
    if (line[0] == 'f' && line[1] == ' ') {
        processFace(line, p_item);
        return;
    }

    // to check if it is a comment or nothing
    if (line[0] == '#' || line[0] == '\0' || line[0] == '\n')
        return;

    // ignore smooth rays line
    if (line[0] == 's' && line[1] == ' ') {
        return;
    }

    // ignore object notation
    if (line[0] == 'o' && line[1] == ' ') {
        return;
    }

    // to check if it is a vertex
    if (line[0] == 'v' && line[1] == ' ') {
        processVertex(line, p_item);
        return;
    }

    // to check if it is a normal
    if (line[0] == 'v' && line[1] == 'n' && line[2] == ' ') {
        processNormal(line, p_item);
        return;
    }

    // to check if it is a texture
    if (line[0] == 'v' && line[1] == 't' && line[2] == ' ') {
        processTexture(line, p_item);
        return;
    }

    // we hard-coded .mtl path so we ignore mtllib(from .obj)
    // handle usemtl(from .obj) and newmtl(from .mtl)
    if (strstr(line, "usemtl") != nullptr) {
        processUsemtl(line, p_item);
        return;
    }

    if (strstr(line, "newmtl") != nullptr) {
        if (processNewmtl(line, p_item)) {
            p_item.allowWriteMtlDataset = true;
        } else {
            p_item.allowWriteMtlDataset = false;
        }
        return;
    }

    if (p_item.allowWriteMtlDataset) {
        if (strstr(line, "Ns") != nullptr) {
            processNs(line, p_item);
            return;
        }

        if (strstr(line, "Ka") != nullptr) {
            processKa(line, p_item);
            return;
        }

        if (strstr(line, "Kd") != nullptr) {
            processKd(line, p_item);
            return;
        }

        if (strstr(line, "Ks") != nullptr) {
            processKs(line, p_item);
            return;
        }

        if (strstr(line, "Ke") != nullptr) {
            processKe(line, p_item);
            return;
        }

        if (strstr(line, "map_Kd") != nullptr) {
            //processMap_Kd(line, p_item);
            return;
        }

        if (strstr(line, "Ni") != nullptr) {
            processNi(line, p_item);
            return;
        }

        if (strstr(line, "d") != nullptr) {
            processD(line, p_item);
            return;
        }

        if (strstr(line, "illum") != nullptr) {
            processIllum(line, p_item);
            return;
        }

        if (strstr(line, "map_Ka") != nullptr) {
            //processMap_Ka(line, p_item);
            return;
        }

        if (strstr(line, "Tr") != nullptr) {
            processTr(line, p_item);
            return;
        }

        if (strstr(line, "Tf") != nullptr) {
            processTf(line, p_item);
            return;
        }
    }
}

