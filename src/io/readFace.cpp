//
// Created by root on 2024/3/15.
//

#include "readFace.h"

void processFace(const char* line, Item& p) {
    /*
     * example:
     * f  5/1/1 3/2/1 1/3/1
     * f  vertex/texcoord/normal  vertex/texcoord/normal  vertex/texcoord/normal
     *
     * f  1/3 2/6 9/8
     * f  vertex/texcoord  vertex/texcoord  vertex/texcoord
     *
     * f  3 4 10
     * f  vertex  vertex  vertex
     */

    // we don't care about textures
    const std::vector<Point>& p_v = p.getVertices();
    std::vector<Triangle>& p_f = p.getMutFaces();
    p_f.emplace_back();
    Triangle& p_t = p_f.back();
    int v_i[3] = {0};

    int count = 0;
    for (int i = 0; line[i] != '\0'; i++) {
        if (line[i] == '/') {
            count++;
        }
    }

    if (count == 6) {
        sscanf(line, "f %d/%*d/%*d %d/%*d/%*d %d/%*d/%*d", &v_i[0], &v_i[1], &v_i[2]);
    } else if (count == 3) {
        sscanf(line, "f %d/%*d %d/%*d %d/%*d", &v_i[0], &v_i[1], &v_i[2]);
    } else {
        sscanf(line, "f %d %d %d", &v_i[0], &v_i[1], &v_i[2]);
    }
    v_i[0]--;
    v_i[1]--;
    v_i[2]--;
    p_t.v0.updatePoint(p_v[v_i[0]].x, p_v[v_i[0]].y, p_v[v_i[0]].z);
    p_t.v1.updatePoint(p_v[v_i[1]].x, p_v[v_i[1]].y, p_v[v_i[1]].z);
    p_t.v2.updatePoint(p_v[v_i[2]].x, p_v[v_i[2]].y, p_v[v_i[2]].z);
    p_t.updateNormalVec();

    // for Item::normalVecInspector()
    p.getMutFWVR().push_back({v_i[0] - 1, v_i[1] - 1, v_i[2] - 1});
}
