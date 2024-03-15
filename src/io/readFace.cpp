//
// Created by root on 2024/3/15.
//

#include "readFace.h"

void processFace(const char* line, Item& p) {
    /*
     * example:
     * f 5/1/1 3/2/1 1/3/1
     *   vertex / texcoord / normal
     */

    // we assume it always like f 1/2/3 4/5/6 7/8/9
    // rather than f 1 2 3
    // and here we don't care about textures
    const std::vector<Point>& p_v = p.getVertices();
    //std::cout<<"p_v.size"<<p_v.size()<<std::endl;
    std::vector<Triangle>& p_f = p.getMutFaces();
    p_f.emplace_back();
    //std::cout<<"p_f.size"<<p_f.size()<<std::endl;
    Triangle& p_t = p_f.back();
    int v_i[3] = {0};
    sscanf(line, "f %d/%*d/%*d %d/%*d/%*d %d/%*d/%*d", &v_i[0], &v_i[1], &v_i[2]);
    v_i[0]--;
    v_i[1]--;
    v_i[2]--;
    p_t.v0.updatePoint(p_v[v_i[0]].x, p_v[v_i[0]].y, p_v[v_i[0]].z);
    p_t.v1.updatePoint(p_v[v_i[1]].x, p_v[v_i[1]].y, p_v[v_i[1]].z);
    p_t.v2.updatePoint(p_v[v_i[2]].x, p_v[v_i[2]].y, p_v[v_i[2]].z);
    p_t.updateNormalVec();
}