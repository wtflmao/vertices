//
// Created by root on 2024/3/15.
//

/* Copyright 2024 wtflmao. All Rights Reserved.
 *
 * Distributed under MIT license.
 * See file LICENSE/LICENSE.MIT.md or copy at https://opensource.org/license/mit
 */

#include "readFace.h"

void processFace(const char* line, Item& p, const bool trigger) {
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
    auto v_i = std::array<int, 3>{0};
    auto vn_i = std::array<int, 3>{0};

    int count = 0;
    for (int i = 0; line[i] != '\0'; i++) {
        if (line[i] == '/') {
            count++;
        }
    }

#ifdef _WIN32
    if (count == 6) {
        sscanf_s(line, "f %d/%*d/%d %d/%*d/%d %d/%*d/%d", &v_i[0], &vn_i[0], &v_i[1], &vn_i[1], &v_i[2], &vn_i[2]);
    } else if (count == 3) {
        sscanf_s(line, "f %d/%*d %d/%*d %d/%*d", &v_i.at(0), &v_i.at(1), &v_i.at(2));
    } else {
        sscanf_s(line, "f %d %d %d", &v_i.at(0), &v_i.at(1), &v_i.at(2));
    }
#else
    if (count == 6) {
        sscanf(line, "f %d/%*d/%d %d/%*d/%d %d/%*d/%d", &v_i[0], &v_i[1], &v_i[2], &vn_i[0], &vn_i[1], &vn_i[2]);
    } else if (count == 3) {
        sscanf(line, "f %d/%*d %d/%*d %d/%*d", &v_i.at(0), &v_i.at(1), &v_i.at(2));
    } else {
        sscanf(line, "f %d %d %d", &v_i.at(0), &v_i.at(1), &v_i.at(2));
    }
#endif

    v_i[0]--;
    v_i[1]--;
    v_i[2]--;
    vn_i[0]--;
    vn_i[1]--;
    vn_i[2]--;
    // we don't compute these normal vec by ourselves for now, we use the normal vec from obj file
    // actually vn_i[0] [1] [2] should be the same, but it CAN be different
    if (p.requireFromOBJ) {
        // use the normal vec from obj file
        //std::cout<<"readFace switch "<<v_i[0] - p.minVertexIndex<<std::endl;
        //std::cout<<"readFace switch "<<vn_i[0] - p.minNormalIndex<<std::endl;
        try {
            //p_t.getMutV0().setX(p_v.at(v_i[0] - p.minVertexIndex).getX())
            //    .setY(p_v.at(v_i[0] - p.minVertexIndex).getY())
            //    .setZ(p_v.at(v_i[0] - p.minVertexIndex).getZ());
            //p_t.getMutV1().setX(p_v.at(v_i[1] - p.minVertexIndex).getX())
            //    .setY(p_v.at(v_i[1] - p.minVertexIndex).getY())
            //    .setZ(p_v.at(v_i[1] - p.minVertexIndex).getZ());
            //p_t.getMutV1().setX(p_v.at(v_i[2] - p.minVertexIndex).getX())
            //    .setY(p_v.at(v_i[2] - p.minVertexIndex).getY())
            //    .setZ(p_v.at(v_i[2] - p.minVertexIndex).getZ());
            p_t.getMutV0().setX(p_v.at(v_i[0]).getX())
                          .setY(p_v.at(v_i[0]).getY())
                          .setZ(p_v.at(v_i[0]).getZ());
            p_t.getMutV1().setX(p_v.at(v_i[1]).getX())
                          .setY(p_v.at(v_i[1]).getY())
                          .setZ(p_v.at(v_i[1]).getZ());
            p_t.getMutV2().setX(p_v.at(v_i[2]).getX())
                          .setY(p_v.at(v_i[2]).getY())
                          .setZ(p_v.at(v_i[2]).getZ());
            p_t.computeCentroid();
            p_t.setNormalVec(Vec{
                p.getNormalList()->at(vn_i[0]).getX(),
                p.getNormalList()->at(vn_i[0]).getY(),
                p.getNormalList()->at(vn_i[0]).getZ()
            });
        } catch (std::out_of_range& e) {
            std::cout<<"In processFace() at readFace.cpp "<<e.what()<<std::endl;
        } catch (...) {
            std::cout<<"unknown exception from processFace()"<<std::endl;
        }
        p.getMutFWVR().push_back({v_i[0], v_i[1], v_i[2]});
    }
    else {
        // sadly, obj doesn't have normal vec at all
        const auto &newV0 = p_v.at(v_i.at(0));
        const auto &newV1 = p_v.at(v_i.at(1));
        const auto &newV2 = p_v.at(v_i.at(2));
        p_t.getMutV0().setX(newV0.getX()).setY(newV0.getY()).setZ(newV0.getZ());
        p_t.getMutV1().setX(newV1.getX()).setY(newV1.getY()).setZ(newV1.getZ());
        p_t.getMutV2().setX(newV2.getX()).setY(newV2.getY()).setZ(newV2.getZ());
        p_t.computeCentroid();
        p_t.updateNormalVec();
        p.getMutFWVR().push_back({v_i[0], v_i[1], v_i[2]});
    }

    //p.getMutFWVR().push_back({v_i.at(0) - p.minVertexIndex, v_i.at(1) - p.minVertexIndex, v_i.at(2) - p.minVertexIndex});
}
