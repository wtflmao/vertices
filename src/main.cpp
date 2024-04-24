// Using C++17 standard, not C++20 due to HPC related issues.
//
// Created by root on 2024/3/13.
//

/* Copyright 2024 wtflmao. All Rights Reserved.
 *
 * Distributed under MIT license.
 * See file LICENSE/LICENSE.MIT.md or copy at https://opensource.org/license/mit
 */

#include "main.h"

void checker(Field &field, const std::vector<std::shared_ptr<Node> > &node_ptrs, std::vector<Ray> *rays,
             const std::pair<int, int> &constSubVec,
             int idx, wrappedRays *ret) {
    // advanced return if empty
    if (constSubVec.first >= constSubVec.second) {
        ret->rays = {};
        ret->done = true;
        return;
    }

    auto goodRays_per_thread = new std::vector<Ray>();
    std::ostringstream s_;
    s_ << "checker thread " << std::this_thread::get_id() << " started ANd " << rays->size() << " " << constSubVec.first
            << " " << constSubVec.second;
#if VERTICES_CONFIG_CXX_STANDARD >= 20
    coutLogger->writeInfoEntry(s_.view());
#elif VERTICES_CONFIG_CXX_STANDARD <= 17
    coutLogger->writeInfoEntry(s_.str());
#endif

    for (auto it = constSubVec.first; it < constSubVec.second; ++it) {
        //std::cout<<"it "<<it<<std::endl;
        //for (int rayIndex = 0; rayIndex < rays->size() || flag; rayIndex++) {
        // get one ray at a time to avoid memory overhead
        //auto ray = Ray();
        //if (cnt < resolutionX * resolutionY) {
        //ray = camera.shootRayRandom(cnt++);
        //ray.ancestor = ray.getOrigin();
        //rayIndex--;
        //} else {
        // now every raw ray from camera has been investigated, traverse the rays vector
        bool flag = true;
        flag = false;
        auto &ray = rays->at(it);
        //}
        // Iterate over all nodes(boxes) and using BVH algorithm
        for (int nodeIndex = 0; nodeIndex < field.nodeCount; nodeIndex++) {
            //std::cout<<"nodeIndex "<<nodeIndex<<std::endl;
            auto &node = node_ptrs[nodeIndex];
            // Check if the ray intersects with the node (box)
            if (ray.intersectsWithBox(node->bbox)) {
                // If intersects, iterate over all faces in this bounding box, the faces may come from diffrent objects
                for (int faceIndex = 0; faceIndex < node->boxedFaces.size(); faceIndex++) {
                    //std::cout<<"faceIndex "<<faceIndex<<std::endl;
                    auto &face = node->boxedFaces[faceIndex];
                    if (auto intersection = ray.mollerTrumboreIntersection(*face); NO_INTERSECT != intersection) {
                        ray.setRayStopPoint(intersection);
                        //std::cout << "The ray " << rayIndex + 1 << " intersects the face #" << faceIndex + 1 << " at "
                        //        << intersection << " with intensity[0] " << ray.intensity_p[0] << std::endl;
                        // check if this ray is valid by checking if there's no any faces in the way from the intersection, in the direction of the matching real pixel from camrea
                        Ray ray_t = Ray(intersection, Vec(intersection, ray.getSourcePixelPosInGnd()));
                        std::ostringstream sss;
                        //sss << std::setprecision(4) << ray.getOrigin() << " sourcePixelPosInGnd " << ray.getSourcePixelPosInGnd().getX() << " " << ray.getSourcePixelPosInGnd().getY() << " " << ray.getSourcePixelPosInGnd().getZ();
                        //coutLogger->writeInfoEntry(sss.view());
                        ray_t.setAncestor(ray.getAncestor());
                        bool validity = true;
                        Point intersection_t = BigO;
                        for (int nodeIndex_t = 0; nodeIndex_t < field.nodeCount && validity; nodeIndex_t++) {
                            //std::cout<<"nodeIndex_t "<<nodeIndex_t<<std::endl;
                            auto &node_t = node_ptrs.at(nodeIndex_t);
                            // Check if the ray intersects with the node (box)
                            if (ray_t.intersectsWithBox(node_t->bbox)) {
                                // If intersects, iterate over all faces in this bounding box, the faces may come from diffrent objects
                                for (int faceIndex_t = 0;
                                     faceIndex_t < node_t->boxedFaces.size() && validity; faceIndex_t++) {
                                    //std::cout<<"faceIndex_t "<<faceIndex_t<<std::endl;
                                    auto &face_t = node_t->boxedFaces.at(faceIndex_t);
                                    if (intersection_t = ray_t.mollerTrumboreIntersection(*face_t); (
                                        NO_INTERSECT != intersection_t && intersection_t != intersection)) {
                                        // bad, has intersection, the original ray is not valid
                                        validity = false;
                                        break;
                                    }
                                }
                            } else {
                                // If not intersects, skip all children
                                nodeIndex_t += node_t->boxedFaces.size();
                            }
                        }
                        if (!validity) {
                            continue;
                        }
                        //goodRays_per_thread->push_back(ray);

                        // here we handle the scattered rays
                        // the intensity for every scattered rays should be determined by BRDF(....)
                        auto scatteredRays = ray.scatter(*face, intersection, field.brdfList.at(
                                                             face->faceBRDF), ray.getSourcePixel());
                        for (const auto &ray_sp: scatteredRays) {
                            //std::cout << "origOrigin" << ray.getOrigin() << " tail" << ray.getDirection().getTail() << " intensity[20]" << ray.getIntensity_p()[20] << " level" << ray.getScatteredLevel()<< std::endl;
                            // config all scattered rays' sourcePixelInGnd
                            for (auto &r: scatteredRays) {
                                if (r.getSourcePixelPosInGnd() == BigO) {
                                    r.setSourcePixelPosInGnd(ray.getSourcePixelPosInGnd());
                                    r.setSourcePixel(ray.getSourcePixel());
                                }
                                //std::cout << "\tscatOrigin" << r.getOrigin() << " tail" << r.getDirection().getTail() << " intensity[20]" << r.getIntensity_p()[20] << " level" << r.getScatteredLevel()<< std::endl;
                            }
                            // debug test only
                            goodRays_per_thread->insert(goodRays_per_thread->end(), scatteredRays.begin(),
                                                        scatteredRays.end());
                            continue;

                            for (int j = 0; j < scatteredRays.size(); j++) {
                                bool flag_tt = false;
                                for (int k = 0; k < scatteredRays.at(j).getIntensity_p().size(); k++)
                                    if (scatteredRays.at(j).getIntensity_p().at(k) > 1e-10) flag_tt = true;
                                if (flag_tt) {
                                    //rays->push_back(scatteredRays[j]);
                                    // for 2+ scattered rays, the source of them is not THE SUN but the original ray
                                    // so, eh, yeah, IDK how to write this, whatever, just see the code below
                                    auto ray_tt = scatteredRays.at(j);
                                    ray_tt.setAncestor(ray_t.getAncestor());
                                    bool validity_tt = false;
                                    for (int nodeIndex_tt = 0;
                                         nodeIndex_tt < field.nodeCount && !validity_tt; nodeIndex_tt++) {
                                        auto &node_tt = node_ptrs.at(nodeIndex_tt);
                                        // Check if the ray intersects with the node (box)
                                        if (ray_tt.intersectsWithBox(node_tt->bbox)) {
                                            // If intersects, iterate over all faces in this bounding box, the faces may come from diffrent objects
                                            for (int faceIndex_tt = 0;
                                                 faceIndex_tt < node_tt->boxedFaces.size() &&
                                                 !validity_tt; faceIndex_tt++) {
                                                auto &face_tt = node_tt->boxedFaces.at(faceIndex_tt);
                                                if (auto intersection_tt = ray_tt.mollerTrumboreIntersection(
                                                    *face_tt); (
                                                    NO_INTERSECT != intersection_tt &&
                                                    intersection_tt != intersection)) {
                                                    // good, scatter ray has intersection
                                                    validity_tt = true;
                                                    break;
                                                }
                                            }
                                        } else {
                                            // If not intersects, skip all children
                                            nodeIndex_tt += node_tt->boxedFaces.size();
                                        }
                                    }
                                    //std::cout << "(";
                                    if (validity_tt) {
                                        goodRays_per_thread->push_back(std::move(ray_tt));
                                        //printf("Good ray intensity [0]%lf, [20%%]%lf, [75%%]%lf, level%d\n", ray_tt.getIntensity_p().at(0), ray_tt.getIntensity_p().at(std::round(0.2*spectralBands)), ray_tt.getIntensity_p().at(0.75*spectralBands), ray_tt.getScatteredLevel());
                                        //std::cout << goodCnt++;
                                    } else {
                                        for (auto &inten: ray_tt.getMutIntensity_p()) {
                                            //inten *= -1;
                                        }
                                        //goodRays_per_thread->push_back(std::move(ray_tt));
                                        //std::cout << "  ";
                                        continue;
                                    }
                                    //std::cout << "," << totCnt++ << ")";
                                    //std::cout << std::endl;
                                }
                            }
                        }
                    }
                }
            } else {
                // If not intersects, skip all children
                nodeIndex += node->boxedFaces.size();
            }
        }
    }

    //return goodRays_per_thread;
    ret->rays = std::move(*goodRays_per_thread);
    ret->done = true;
    std::ostringstream s;
    s << "checker thread " << std::this_thread::get_id() << " done with " << ret->rays.size() << " rays." <<
            constSubVec.first << " " << constSubVec.second;
#if VERTICES_CONFIG_CXX_STANDARD >= 20
    coutLogger->writeInfoEntry(s.view());
#elif VERTICES_CONFIG_CXX_STANDARD <= 17
    coutLogger->writeInfoEntry(s.str());
#endif
}

void mixer(const int left, const int right, Camera &camera,
           const std::shared_ptr<std::vector<std::vector<Pixel> > > &camPixelsBackup) {
    if (left >= right) {
        return;
    }
    std::ostringstream s_;
    s_ << "mixer thread " << std::this_thread::get_id() << " started " << left << " " << right;
#if VERTICES_CONFIG_CXX_STANDARD >= 20
    coutLogger->writeInfoEntry(s_.view());
#elif VERTICES_CONFIG_CXX_STANDARD <= 17
    coutLogger->writeInfoEntry(s_.str());
#endif

    if (camera.getPixel2D()->size() < 5 || camera.getPixel2D()->at(0).size() < 5)
        return;
    // then mix up neighboring pixel's intensity

    for (int i = 0; i < camera.getPixel2D()->size(); i++) {
        for (int j = 0; j < camera.getPixel2D()->at(0).size(); j++) {
            for (int k = left; k < right; k++) {
                if (i != 0 && i != camera.getPixel2D()->size() - 1 && j != 0 && j != camera.getPixel2D()->at(0).size() -
                    1) {
                    (*camera.getPixel2D())[i][j].getMutPixelSpectralResp()[k] += (
                        (*camPixelsBackup)[i - 1][j].getPixelSpectralResp()[k] * mixRatioU
                        + (*camPixelsBackup)[i + 1][j].getPixelSpectralResp()[k] * mixRatioD
                        + (*camPixelsBackup)[i][j - 1].getPixelSpectralResp()[k] * mixRatioL
                        + (*camPixelsBackup)[i][j + 1].getPixelSpectralResp()[k] * mixRatioR
                    );

                    (*camera.getPixel2D())[i][j].getMutPixelSpectralResp()[k] += (
                        (*camPixelsBackup)[i - 1][j - 1].getPixelSpectralResp()[k] * mixRatioL * mixRatioU
                        + (*camPixelsBackup)[i - 1][j + 1].getPixelSpectralResp()[k] * mixRatioL * mixRatioD
                        + (*camPixelsBackup)[i + 1][j - 1].getPixelSpectralResp()[k] * mixRatioR * mixRatioU
                        + (*camPixelsBackup)[i + 1][j + 1].getPixelSpectralResp()[k] * mixRatioR * mixRatioD
                    );
                } else {
                    if (i == 0 && j == 0)
                        (*camera.getPixel2D())[i][j].getMutPixelSpectralResp()[k] += (
                            (*camPixelsBackup)[i + 1][j + 1].getPixelSpectralResp()[k] * mixRatioD * mixRatioR
                            + (*camPixelsBackup)[i + 1][j].getPixelSpectralResp()[k] * mixRatioD
                            + (*camPixelsBackup)[i][j + 1].getPixelSpectralResp()[k] * mixRatioR
                        );
                    else if (i == camera.getPixel2D()->size() - 1 && j == camera.getPixel2D()->at(0).size() - 1)
                        (*camera.getPixel2D())[i][j].getMutPixelSpectralResp()[k] += (
                            (*camPixelsBackup)[i - 1][j - 1].getPixelSpectralResp()[k] * mixRatioU * mixRatioL
                            + (*camPixelsBackup)[i - 1][j].getPixelSpectralResp()[k] * mixRatioU
                            + (*camPixelsBackup)[i][j - 1].getPixelSpectralResp()[k] * mixRatioL
                        );
                    else if (i == 0 && j == camera.getPixel2D()->at(0).size() - 1)
                        (*camera.getPixel2D())[i][j].getMutPixelSpectralResp()[k] += (
                            (*camPixelsBackup)[i + 1][j - 1].getPixelSpectralResp()[k] * mixRatioD * mixRatioL
                            + (*camPixelsBackup)[i + 1][j].getPixelSpectralResp()[k] * mixRatioD
                            + (*camPixelsBackup)[i][j - 1].getPixelSpectralResp()[k] * mixRatioL
                        );
                    else if (i == camera.getPixel2D()->size() - 1 && j == 0)
                        (*camera.getPixel2D())[i][j].getMutPixelSpectralResp()[k] += (
                            (*camPixelsBackup)[i - 1][j + 1].getPixelSpectralResp()[k] * mixRatioU * mixRatioR
                            + (*camPixelsBackup)[i - 1][j].getPixelSpectralResp()[k] * mixRatioU
                            + (*camPixelsBackup)[i][j + 1].getPixelSpectralResp()[k] * mixRatioR
                        );
                    else if (i == 0 && j != 0)
                        (*camera.getPixel2D())[i][j].getMutPixelSpectralResp()[k] += (
                            (*camPixelsBackup)[i + 1][j + 1].getPixelSpectralResp()[k] * mixRatioU * mixRatioR
                            + (*camPixelsBackup)[i + 1][j - 1].getPixelSpectralResp()[k] * mixRatioU * mixRatioL
                            + (*camPixelsBackup)[i + 1][j].getPixelSpectralResp()[k] * mixRatioU
                            + (*camPixelsBackup)[i][j + 1].getPixelSpectralResp()[k] * mixRatioR
                            + (*camPixelsBackup)[i][j - 1].getPixelSpectralResp()[k] * mixRatioL
                        );
                    else if (i != 0 && j == 0)
                        (*camera.getPixel2D())[i][j].getMutPixelSpectralResp()[k] += (
                            (*camPixelsBackup)[i + 1][j + 1].getPixelSpectralResp()[k] * mixRatioD * mixRatioR
                            + (*camPixelsBackup)[i - 1][j + 1].getPixelSpectralResp()[k] * mixRatioU * mixRatioR
                            + (*camPixelsBackup)[i][j + 1].getPixelSpectralResp()[k] * mixRatioR
                            + (*camPixelsBackup)[i + 1][j].getPixelSpectralResp()[k] * mixRatioD
                            + (*camPixelsBackup)[i - 1][j].getPixelSpectralResp()[k] * mixRatioU
                        );
                    else if (i == camera.getPixel2D()->size() - 1 && j != camera.getPixel2D()->at(0).size() - 1)
                        (*camera.getPixel2D())[i][j].getMutPixelSpectralResp()[k] += (
                            (*camPixelsBackup)[i - 1][j + 1].getPixelSpectralResp()[k] * mixRatioL * mixRatioU
                            + (*camPixelsBackup)[i - 1][j - 1].getPixelSpectralResp()[k] * mixRatioR * mixRatioU
                            + (*camPixelsBackup)[i - 1][j].getPixelSpectralResp()[k] * mixRatioU
                            + (*camPixelsBackup)[i][j + 1].getPixelSpectralResp()[k] * mixRatioR
                            + (*camPixelsBackup)[i][j - 1].getPixelSpectralResp()[k] * mixRatioL
                        );
                    else if (i != camera.getPixel2D()->size() - 1 && j == camera.getPixel2D()->at(0).size() - 1)
                        (*camera.getPixel2D())[i][j].getMutPixelSpectralResp()[k] += (
                            (*camPixelsBackup)[i + 1][j - 1].getPixelSpectralResp()[k] * mixRatioL * mixRatioU
                            + (*camPixelsBackup)[i - 1][j - 1].getPixelSpectralResp()[k] * mixRatioL * mixRatioD
                            + (*camPixelsBackup)[i][j - 1].getPixelSpectralResp()[k] * mixRatioD
                            + (*camPixelsBackup)[i + 1][j].getPixelSpectralResp()[k] * mixRatioU
                            + (*camPixelsBackup)[i - 1][j].getPixelSpectralResp()[k] * mixRatioL
                        );
                    else coutLogger->writeErrorEntry("Unknown case in main() at summing up, unbelievable");
                }

                if (i >= 2 && j >= 2 && i <= camera.getPixel2D()->size() - 3 && j <= camera.getPixel2D()->at(0).size() -
                    3)
                    (*camera.getPixel2D())[i][j].getMutPixelSpectralResp()[k] += (
                        (*camPixelsBackup)[i - 2][j].getPixelSpectralResp()[k] * mixRatioU * mixRatioU
                        + (*camPixelsBackup)[i + 2][j].getPixelSpectralResp()[k] * mixRatioD * mixRatioD
                        + (*camPixelsBackup)[i][j - 2].getPixelSpectralResp()[k] * mixRatioL * mixRatioL
                        + (*camPixelsBackup)[i][j + 2].getPixelSpectralResp()[k] * mixRatioR * mixRatioR
                    );
            }
        }
    }
    std::ostringstream s;
    s << "mixer thread " << std::this_thread::get_id() << " done " << left << " " << right;
#if VERTICES_CONFIG_CXX_STANDARD >= 20
    coutLogger->writeInfoEntry(s.view());
#elif VERTICES_CONFIG_CXX_STANDARD <= 17
    coutLogger->writeInfoEntry(s.str());
#endif
}

int main() {
    // ===== debug only start =====
    coutLogger->writeInfoEntry("Hello there!");

    stdoutLogger->writeInfoEntry("Hi there!");

    fsLogger->writeInfoEntry("hello hi!");

    Point x = {0.8, 0.6, 0};
    Point y = BigO;
    coordTransform->camToGnd(x, y);
    std::cout << y << std::endl;
    x = y;
    coordTransform->gndToCam(x, y);
    std::cout << y << std::endl;

    x = {10.0, 5.0, 0.3};
    coordTransform->gndToCam(x, y);
    std::cout << y << std::endl;
    x = y;
    coordTransform->camToGnd(x, y);
    std::cout << y << std::endl << std::endl;

    x = {0, 0, 0};
    coordTransform->camToImg(x, y);
    std::cout << y << std::endl;
    x = y;
    coordTransform->imgToCam(x, y);
    std::cout << y << std::endl;
    x = {
        CENTER_OF_CAMERA_IN_GND.at(0) + pixelSize * 1e-6 * resolutionX,
        CENTER_OF_CAMERA_IN_GND.at(1) + pixelSize * 1e-6 * resolutionY, 0.1
    };
    coordTransform->camToImg(x, y);
    std::cout << y << std::endl;
    x = y;
    coordTransform->imgToCam(x, y);
    std::cout << y << std::endl;
    // ====== debug only ends =====

    //std::cout << Triangle(Point(-0.5, 0.173092, 0.5), Point(-0.498325, 0.173198, 0.5), Point(-0.5, 0.173218, 0.498325)).
    //        getNormal().tail << std::endl;

    std::vector<std::string> objPaths;
    std::vector<std::string> mtlPaths;
    std::vector<std::pair<std::array<int, 2>, std::string> > BRDFPaths;
#ifdef _WIN32
    std::cout << "win32" << std::endl;

    objPaths.emplace_back(R"(C:\Users\root\3D Objects\mycube\mycube.obj)");
    objPaths.emplace_back(R"(C:\Users\root\3D Objects\hot_desert_biome_obj\source\CalidiousDesert_obj_-z_y.obj)");
    objPaths.emplace_back(R"(C:\Users\root\3D Objects\snow_apls_low_poly_obj\source\Mesher_-z_y.obj)");
    objPaths.emplace_back(R"(C:\Users\root\3D Objects\F22_blender\F22.obj)");

    mtlPaths.emplace_back(R"(C:\Users\root\3D Objects\mycube\mycube.mtl)");

    // BRDFPaths' sequence should follow your object's sequence!!!!!!!!!!
    // and blue should always be the first, then green, then red, then shortwave infrared if possible

    BRDFPaths.emplace_back(std::array<int, 2>{0, 0}, R"(C:\Users\root\Downloads\chrome-steel.binary)");
    BRDFPaths.emplace_back(std::array<int, 2>{BLUE_UPPER, BLUE_LOWER},
                           R"(C:\Users\root\Downloads\debug.mini.459.479.txt)");
    BRDFPaths.emplace_back(std::array<int, 2>{GREEN_UPPER, GREEN_LOWER},
                           R"(C:\Users\root\Downloads\debug.mini.545.565.txt)");
    BRDFPaths.emplace_back(std::array<int, 2>{RED_UPPER, RED_LOWER},
                           R"(C:\Users\root\Downloads\debug.mini.620.670.txt)");
    /*BRDFPaths.emplace_back(std::array<int, 2>{BLUE_UPPER, BLUE_LOWER},
                           R"(C:\Users\root\Downloads\MCD43A4.A2024074.h26v04.061.2024085221829.band3.459.479.txt)");
    BRDFPaths.emplace_back(std::array<int, 2>{GREEN_UPPER, GREEN_LOWER},
                           R"(C:\Users\root\Downloads\MCD43A4.A2024074.h26v04.061.2024085221829.band4.545.565.txt)");
    BRDFPaths.emplace_back(std::array<int, 2>{RED_UPPER, RED_LOWER},
                           R"(C:\Users\root\Downloads\MCD43A4.A2024074.h26v04.061.2024085221829.band1.620.670.txt)");
                           */
#elif __unix__ || __unix || __APPLE__ || __MACH__ || __linux__
    std::cout << "unix-like" << std::endl;

    objPaths.emplace_back(R"(/home/20009100240/3dmodel/mycube/mycube.obj)");
    objPaths.emplace_back(R"(/home/20009100240/3dmodel/hot_desert_biome_obj/source/CalidiousDesert_obj_-z_y.obj)");
    objPaths.emplace_back(R"(/home/20009100240/3dmodel/snow_apls_low_poly_obj/source/Mesher_-z_y.obj)");
    objPaths.emplace_back(R"(/home/20009100240/3dmodel/F22_blender/F22.obj)");

    mtlPaths.emplace_back(R"(/home/20009100240/3dmodel/mycube/mycube.mtl)");

    // BRDFPaths' sequence should follow your object's sequence!!!!!!!!!!
    // and blue should always be the first, then green, then red, then shortwave infrared if possible

    BRDFPaths.emplace_back(std::array<int, 2>{0, 0}, R"(/home/20009100240/3dmodel/BRDF/chrome-steel.binary)");
    BRDFPaths.emplace_back(std::array<int, 2>{BLUE_UPPER, BLUE_LOWER},
                           R"(/home/20009100240/3dmodel/BRDF/debug.mini.459.479.txt)");
    BRDFPaths.emplace_back(std::array<int, 2>{GREEN_UPPER, GREEN_LOWER},
                           R"(/home/20009100240/3dmodel/BRDF/debug.mini.545.565.txt)");
    BRDFPaths.emplace_back(std::array<int, 2>{RED_UPPER, RED_LOWER},
                           R"(/home/20009100240/3dmodel/BRDF/debug.mini.620.670.txt)");
#else
    std::cout << "other system, quitting" << std::endl;
    exit(0);
#endif

    std::cout << "Setting up field..." << std::endl;
    Field field = Field(
        Point(-60, -60, -1),
        Point(60, 60, 30 * 1.732)
    );

    field.newClosedObject()
            .setOBJPath(objPaths.at(0))
            .setMTLPath(mtlPaths.at(0))
            .setCenter({0, -3, 2})
            .setScaleFactor({3, 2, 2.5})
            .setForwardAxis(6)
            .setUpAxis(2)
            // innerPoints should be in cube {-1, -1, -1}--{1, 1, 1}
            .setInnerPoints({
                {-0.1, -0.1, -0.1},
                {0.1, 0.1, 0.1},
                {0, 0, 0}
            })
            .readFromOBJ()
            .readFromMTL()
            .inspectNormalVecForAllFaces();

#if VERTICES_CONFIG_CXX_STANDARD >= 20
    coutLogger->writeInfoEntry(std::format("Object #{} has been loaded", field.getObjects().size()));
    coutLogger->writeInfoEntry(std::format("Object #{} has {} faces", field.getObjects().size(),
                                           field.getObjects().back().getFaces().size()));
    coutLogger->writeInfoEntry(std::format("Object #{} has {} vertices", field.getObjects().size(),
                                           field.getObjects().back().getVertices().size()));
#elif VERTICES_CONFIG_CXX_STANDARD <= 17
    coutLogger->writeInfoEntry("Object #{" + std::to_string(field.getObjects().size()) + "} has been loaded");
    coutLogger->writeInfoEntry(
        "Object #{" + std::to_string(field.getObjects().size()) + "} has " + std::to_string(
            field.getObjects().back().getFaces().size()) + " faces");
    coutLogger->writeInfoEntry(
        "Object #{" + std::to_string(field.getObjects().size()) + "} has " + std::to_string(
            field.getObjects().back().getVertices().size()) + " vertices");
#endif

    field.newOpenObject()
            .setOBJPath(objPaths.at(1))
            .setMTLPath(mtlPaths.at(0))
            .setForwardAxis(6)
            .setUpAxis(2)
            .setCenter({0, 0, 0})
            .setScaleFactor({800, 800, 1})
            .setThatCorrectFaceVertices({598, 0, 1})
            .setThatCorrectFaceIndex(0)
            .readFromOBJ()
            .readFromMTL()
            .inspectNormalVecForAllFaces();

#if VERTICES_CONFIG_CXX_STANDARD >= 20
    coutLogger->writeInfoEntry(std::format("Object #{} has been loaded", field.getObjects().size()));
    coutLogger->writeInfoEntry(std::format("Object #{} has {} faces", field.getObjects().size(),
                                           field.getObjects().back().getFaces().size()));
    coutLogger->writeInfoEntry(std::format("Object #{} has {} vertices", field.getObjects().size(),
                                           field.getObjects().back().getVertices().size()));
#elif VERTICES_CONFIG_CXX_STANDARD <= 17
    coutLogger->writeInfoEntry("Object #{" + std::to_string(field.getObjects().size()) + "} has been loaded");
    coutLogger->writeInfoEntry(
        "Object #{" + std::to_string(field.getObjects().size()) + "} has " + std::to_string(
            field.getObjects().back().getFaces().size()) + " faces");
    coutLogger->writeInfoEntry(
        "Object #{" + std::to_string(field.getObjects().size()) + "} has " + std::to_string(
            field.getObjects().back().getVertices().size()) + " vertices");
#endif


    /*field.insertObject(
        objPaths[2],
        mtlPaths[0],
        {200, 400, 2},
        Point(25, 25, -0.1),
        {},
        0,
        {},
        true, 6, 2
    );
    field.insertObject(
        objPaths[3],
        mtlPaths[0],
        {9.5, 9.5, 3},
        Point(-15, 2, 0.5),
        {},
        0,
        {todo},
        false, 6, 2
    );*/

    int brdf_cnt_t = 0, b_cnt = 0, size = static_cast<int>(BRDFPaths.size());
    for (auto &fieldItem: field.getObjects()) {
        if (size == 0) {
            fprintf(stderr, "Error when initializing BRDFs. Reason: not enough BRDFs in the list.\a\n");
            return 6;
        }
        if (fieldItem.isOpenMesh) {
            field.brdfList.push_back(new OpenBRDF());
            auto &brdfRef = *field.brdfList.at(brdf_cnt_t);
            fieldItem.brdfIdx = b_cnt++;
            std::cout << "Handling a set of open BRDF" << std::endl;
            if (brdf_cnt_t + 3 <= size) {
                dynamic_cast<OpenBRDF &>(brdfRef).OpenBRDFInsert(BRDFPaths.at(brdf_cnt_t).second.c_str(),
                                                                 BRDFPaths.at(brdf_cnt_t).first);
                brdf_cnt_t++;
                dynamic_cast<OpenBRDF &>(brdfRef).OpenBRDFInsert(BRDFPaths.at(brdf_cnt_t).second.c_str(),
                                                                 BRDFPaths.at(brdf_cnt_t).first);
                brdf_cnt_t++;
                dynamic_cast<OpenBRDF &>(brdfRef).OpenBRDFInsert(BRDFPaths.at(brdf_cnt_t).second.c_str(),
                                                                 BRDFPaths.at(brdf_cnt_t).first);
                brdf_cnt_t++;
            } else {
                fprintf(stderr, "Error when initializing BRDFs. Reason: not enough open BRDFs in the list.\a\n");
                return 66;
            }
            std::cout << "Handling a set of open BRDF done" << std::endl;
        } else {
            try {
                //fieldItem.brdf = new ClosedBRDF();
                field.brdfList.push_back(new ClosedBRDF());
                auto &brdfRef = *field.brdfList.at(brdf_cnt_t);
                fieldItem.brdfIdx = b_cnt++;
                std::cout << "Handling a set of closed BRDF" << std::endl;
                if (brdf_cnt_t + 1 <= size) {
                    //dynamic_cast<ClosedBRDF &>(brdfRef).ClosedBRDFInsert(BRDFPaths.at(brdf_cnt_t).second.c_str());
                    dynamic_cast<ClosedBRDF &>(brdfRef).filename = BRDFPaths.at(brdf_cnt_t).second;
                } else {
                    fprintf(stderr, "Error when initializing BRDFs. Reason: not enough closed BRDFs in the list.\a\n");
                    return 67;
                }
                brdf_cnt_t++;
                std::cout << "Handling a set of closed BRDF done" << std::endl;
            } catch (const std::exception &e) {
                std::cerr << "\a[Debug] Caught exception in main(): " << e.what() << std::endl;
            } catch (...) {
                std::cerr << "[Debug] Caught unknown exception in main()\a\n";
            }
            /*
            // debug only
            auto thetaIn = *(dynamic_cast<ClosedBRDF*>(field.brdfList.back())->availThetaIn);
            std::cout << "availThetaIn:";
            for (auto & item: thetaIn) {
                std::cout << " " << item;
            }
            std::cout << std::endl;
            // debug only
            auto phiIn = *(dynamic_cast<ClosedBRDF*>(field.brdfList.back())->availPhiIn);
            std::cout << "availPhiIn:";
            for (auto & item: phiIn) {
                std::cout << " " << item;
            }
            std::cout << std::endl;
            // debug only
            auto thetaOut = *(dynamic_cast<ClosedBRDF*>(field.brdfList.back())->availThetaOut);
            std::cout << "availThetaOut:";
            for (auto & item: thetaOut) {
                std::cout << " " << item;
            }
            std::cout << std::endl;
            // debug only
            auto phiOut = *(dynamic_cast<ClosedBRDF*>(field.brdfList.back())->availPhiOut);
            std::cout << "availPhiOut:";
            for (auto & item: phiOut) {
                std::cout << " " << item;
            }
            std::cout << std::endl;
            */
        }
    }

    // since brdf has been initialized, now asign every face with a pointer to a brdf that the owner item has
    // it's such a stupid design pattern i know, but there's no way back
    std::cout << "face's brdf initializing only for BVH..." << std::endl;
    for (auto &obj: field.getObjects()) {
        for (auto &faceMut: obj.getMutFaces()) {
            faceMut.faceBRDF = obj.brdfIdx;
        }
    }

    field.buildBVHTree(); {
        const std::vector<std::shared_ptr<Node> > node_ptrs = field.generateNodeList();
        for (int nodeIndex = 0; nodeIndex < field.nodeCount; nodeIndex++) {
            auto &node = node_ptrs.at(nodeIndex);
            for (int faceIndex = 0; faceIndex < node->boxedFaces.size(); faceIndex++) {
                auto &face = node->boxedFaces.at(faceIndex);
                if (face->faceBRDF == -1)
                    std::cerr << "[Error] face #" << faceIndex << " of node #" << nodeIndex
                            << " has no brdfIdx in main()" << std::endl;
            }
        }
    }

    auto rays = new std::vector<Ray>();
    rays->emplace_back(Point(0, 1, 2), Vec(Point(0, -1, -1)));
    rays->emplace_back(Point(0, 0.5, 2), Vec(Point(0, 0, -1)));
    rays->emplace_back(Point(-0.5, -0.5, -0.5), Vec(Point(1, 1.1, 1.2)));

    std::cout << "-------Now----traversing----every----face--------" << std::endl;

    // only for timing
    // Get the starting timepoint
    auto start = std::chrono::high_resolution_clock::now();

    // Iterate over all rays
    for (int rayIndex = 0; rayIndex < rays->size(); rayIndex++) {
        auto &ray = rays->at(rayIndex);
        // Iterate over all objects
        for (int objIndex = 0; objIndex < field.getObjects().size(); objIndex++) {
            auto &obj = field.getObjects().at(objIndex);
            // Iterate over all faces of each object
            for (int faceIndex = 0; faceIndex < obj.getFaces().size(); faceIndex++) {
                auto &face = obj.getFaces().at(faceIndex);
                auto intersection = ray.mollerTrumboreIntersection(face);
                if (NO_INTERSECT != intersection) {
                    ray.setRayStopPoint(intersection);
                    std::cout << "The ray " << rayIndex + 1 << " intersects the face " << faceIndex + 1 << " of object "
                            << objIndex + 1 << " at " << intersection << std::endl;
                    const auto scatteredRays = ray.scatter(face, intersection, field.brdfList.at(obj.brdfIdx),
                                                           ray.getSourcePixel());
                    for (int j = 0; j < scatteredRays.size(); j++) {
                        bool flag = false;
                        for (int k = 0; k < scatteredRays.at(j).getIntensity_p().size(); k++)
                            if (scatteredRays.at(j).getIntensity_p().at(k) > 1e-10) flag = true;
                        if (flag) rays->push_back(scatteredRays.at(j));
                    }
                } else {
                    //std::cout << "The ray " << rayIndex+1 << " does not intersect the face " << faceIndex+1 << " of object " << objIndex+1 << "." << std::endl;
                }
            }
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
#if VERTICES_CONFIG_CXX_STANDARD >= 20
    std::cout << "Time taken: " << duration << "." << std::endl;
#elif VERTICES_CONFIG_CXX_STANDARD <= 17
    std::cout << "Time taken: " << duration.count() << "." << std::endl;
#endif
    std::cout << "Ray count: " << rays->size() << "." << std::endl;


    rays->clear();
    rays->emplace_back(Point(0, 1, 2), Vec(Point(0, -1, -1)));
    rays->emplace_back(Point(0, 0.5, 2), Vec(Point(0, 0, -1)));
    rays->emplace_back(Point(-0.5, -0.5, -0.5), Vec(Point(1, 1.1, 1.2)));

    std::cout << "-------Now----using----BVH----method----to-----accelerate--------" << std::endl;
    // generate a vector of all nodes' const shared pointers
    // so that I can conveiently traverse all nodes many times without visiting the multi-arch tree again and again
    // thanks to modern C++, RVO or NRVO is possible
    const std::vector<std::shared_ptr<Node> > node_ptrs = field.generateNodeList();

    // only for timing
    start = std::chrono::high_resolution_clock::now();

    // use BVH to accelerate the determination of whether the ray intersecting
    for (int rayIndex = 0; rayIndex < rays->size(); rayIndex++) {
        auto &ray = rays->at(rayIndex);
        // Iterate over all nodes(boxes) and using BVH algorithm
        for (int nodeIndex = 0; nodeIndex < field.nodeCount; nodeIndex++) {
            auto &node = node_ptrs.at(nodeIndex);
            // Check if the ray intersects with the node (box)
            if (ray.intersectsWithBox(node->bbox)) {
                // If intersects, iterate over all faces in this bounding box, the faces may come from diffrent objects
                for (int faceIndex = 0; faceIndex < node->boxedFaces.size(); faceIndex++) {
                    auto &face = node->boxedFaces.at(faceIndex);
                    if (auto intersection = ray.mollerTrumboreIntersection(*face); NO_INTERSECT != intersection) {
                        ray.setRayStopPoint(intersection);
                        std::cout << "The ray " << rayIndex + 1 << " intersects the face #" << faceIndex + 1 << " at "
                                << intersection << std::endl;
                        const auto scatteredRays = ray.scatter(*face, intersection,
                                                               field.brdfList.at(face->faceBRDF), ray.getSourcePixel());
                        for (const auto &ray_sp: scatteredRays) {
                            for (int j = 0; j < scatteredRays.size(); j++) {
                                bool flag = false;
                                for (int k = 0; k < scatteredRays.at(j).getIntensity_p().size(); k++)
                                    if (scatteredRays.at(j).getIntensity_p().at(k) > 1e-10) flag = true;
                                if (flag) rays->push_back(scatteredRays.at(j));
                            }
                        }
                    }
                }
            } else {
                // If not intersects, skip all children
                nodeIndex += node->boxedFaces.size();
            }
        }
    }

    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
#if VERTICES_CONFIG_CXX_STANDARD >= 20
    std::cout << "Time taken: " << duration << "." << std::endl;
#elif VERTICES_CONFIG_CXX_STANDARD <= 17
    std::cout << "Time taken: " << duration.count() << "." << std::endl;
#endif
    std::cout << "Ray count: " << rays->size() << "." << std::endl;


    // set up an camera
    // use default constructor for now
    auto camera = Camera();
    // commonly the height from ground is 200m
    //camera.spatialPosition = {Point(-0.008, -0.006, CAMERA_HEIGHT), Point(0.008, 0.006, CAMERA_HEIGHT)};
    rays->clear();
    //camera.buildSunlightSpectrum();

    std::cout << ">>>>>>>>>>the fovs are " << FOVx << " " << FOVy << std::endl;
    // after this there should be resolution X*Y rays
    //auto rays_r = camera.shootRaysRandom();

    auto rays_r = camera.shootRays(1);
    rays->insert(rays->end(), rays_r->begin(), rays_r->end());
    coutLogger->writeInfoEntry(
        "The rays_r's size is " + std::to_string(rays_r->size()) + " and teh rays's size is " + std::to_string(
            rays->size()));
    delete rays_r;
    rays_r = nullptr;
    if (rays->empty()) {
        std::cout << "trying to deref a nullptr in main() from camera.shootRays() call\a" << std::endl;
        return 8;
    }
    for (int i = 0; i < rays->size(); i++) {
        //std::cout << "ray #" << i << " ori:" << rays->at(i).getOrigin() << " direTail:" << rays->at(i).getDirection().getTail() << std::endl;
    }

    Ray tmp;
    //tmp.setOrigin({0, 0, 3}).setDirection(Vec({0, 0, -1})).setIntensity_p(camera.sunlightSpectrum).setAncestor({0, 0, 3}).setScatteredLevel(1).setSourcePixel(static_cast<void*>(&camera.getPixel2D()->at(camera.getPixel2D()->size()/2).at(camera.getPixel2D()->size()/2)));
    tmp.setOrigin({0, 0, 3}).setDirection(Vec({0, 0, -1})).setIntensity_p(camera.sunlightSpectrum).
            setAncestor({0, 0, 3}).setScatteredLevel(1).
            setSourcePixel(static_cast<void *>(&camera.getPixel2D()->at(0).at(0))).setSourcePixelPosInGnd(
                static_cast<Pixel *>(tmp.getSourcePixel())->getPosInGnd());
    //rays->push_back(tmp);

    coutLogger->writeInfoEntry(
        "camrea.pixel2d size:" + std::to_string(camera.getPixel2D()->size()) + " " + std::to_string(
            camera.getPixel2D()->at(0).size()));


    std::cout << "-------Camera----using----BVH----method----to-----accelerate--------" << std::endl;
    // only for timing
    start = std::chrono::high_resolution_clock::now();

    int cnt = 0;
    int goodCnt = 0, totCnt = 0;
    // use BVH to accelerate the determination of whether the ray intersecting
    coutLogger->writeInfoEntry(std::to_string(rays->size()));
    for (auto &ray: *rays) {
        //std::cout << "Ray " << cnt++ << ": " << ray.getOrigin() << " " << ray.getAncestor() << "\n";
    }
    cnt = 0;

    void checker(Field &field, const std::vector<std::shared_ptr<Node> > &node_ptrs, std::vector<Ray> *rays,
                 const std::pair<int, int> &constSubVec,
                 int idx, wrappedRays *ret);
#ifdef VERTICES_CONFIG_MULTI_THREAD_FOR_CAMRAYS
    //dp::thread_pool pool(std::max(std::thread::hardware_concurrency(), 1u));
    ThreadPool pool;
    //auto results = new std::vector<std::future<std::vector<Ray>*>>;
    std::vector<std::future<void> > futures;
    //for (int rayIndex = 0; rayIndex < rays->size() || flag; rayIndex++) {

    // divide rays into CPU cores amount of smaller rays
    const int numOfChunks = 1;//static_cast<int>(std::max(std::thread::hardware_concurrency(), 1u));
    std::vector<std::pair<int, int>> subVectors;
    int chunkSize = static_cast<int>(rays->size() / numOfChunks);
    for (int i = 0; i < numOfChunks; i++) {
        // [left_idx, right_idx)
        subVectors.emplace_back(i * chunkSize, (i + 1) * chunkSize);
    }
    // If vector size is not divisible by numOfChunks, remaining elements go to the last chunk
    if (rays->size() % numOfChunks != 0) {
        subVectors.back().second = static_cast<int>(rays->size());
    }
    coutLogger->writeInfoEntry("subVectors size:" + std::to_string(subVectors.size()));

    const auto &constSubVecs = subVectors;

    auto groupsOfWrappedRays = new std::vector<wrappedRays>(numOfChunks);
    for (int p = 0; p < numOfChunks; p++)
        groupsOfWrappedRays->emplace_back();

    for (int idx = 0; idx < constSubVecs.size(); idx++) {
        auto task = [&]() {
            return checker(std::ref(field), std::ref(node_ptrs), std::ref(rays), std::ref(constSubVecs.at(idx)), idx,
                           &groupsOfWrappedRays->at(idx));
        };
        futures.emplace_back(pool.submit(task));
    }
    coutLogger->writeInfoEntry("cool here yyy");

    auto goodRays_t = new std::vector<Ray>;
    for (auto &result: futures) {
        try {
            result.get();
        } catch (const std::exception& e) {
            coutLogger->writeErrorEntry("Exception caught when at futures .wait(): " + std::string(e.what()));
        } catch (...) {
            coutLogger->writeErrorEntry("thread pool futures .wait() unknown exception occurred");
        }
    }
    coutLogger->writeInfoEntry("not cool here ddd");

    bool allDone = false;
    while (!allDone) {
        allDone = true;
        for (auto &[rays, done]: *groupsOfWrappedRays)
            allDone &= done;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    coutLogger->writeInfoEntry("not not cool here ddddd");

    for (auto &[rays, done]: *groupsOfWrappedRays) {
        goodRays_t->insert(goodRays_t->end(), rays.begin(), rays.end());
        delete &rays;
    }
    coutLogger->writeInfoEntry("not cool here ddddddd");
#endif
#ifdef VERTICES_CONFIG_SINGLE_THREAD_FOR_CAMRAYS
    auto ret = new wrappedRays();
    //for (auto &ray: *rays) {
    checker(field, node_ptrs, rays, std::make_pair(0, rays->size()), 0, ret);
    //}
    auto *goodRays_t = std::move(&ret->rays);
    delete ret;

#endif
#ifdef VERTICES_CONFIG_MULTI_THREAD_FOR_CAMRAYS_WORKAROUND
    std::vector<std::thread> threads;
    std::vector<std::pair<int, int> > subVectors;
    auto threadAmount = HARDWARE_CONCURRENCY;
    int chunkSize = static_cast<int>(rays->size() / threadAmount) + 1;
    auto rets = new std::vector<wrappedRays>();
    for (int i = 0; i < rays->size(); i += chunkSize) {
        // [left_idx, right_idx)
        if (i >= rays->size()) {
            subVectors.emplace_back(-1, -1);
        } else if (i + chunkSize >= rays->size()) {
            subVectors.emplace_back(i, rays->size());
        } else {
            subVectors.emplace_back(i, i + chunkSize);
        }
        rets->emplace_back();
    }
    int i_ = 0;
    for (const auto &sub: subVectors) {
        threads.emplace_back(checker, std::ref(field), std::ref(node_ptrs), rays, std::ref(sub), 0, &rets->at(i_++));
    }
    for (auto &thread: threads) {
        try {
            thread.join();
        } catch (const std::exception &e) {
            coutLogger->writeErrorEntry("Exception caught when at threads.join() in main(): " + std::string(e.what()));
        } catch (...) {
            coutLogger->writeErrorEntry("threads.join() in main() encountered unknown exception");
        }
    }

    auto goodRays_tt = new std::vector<Ray>;
    // actually after .join(), all rets should be done==true
    for (auto &[rays, done]: *rets) {
        if (done) goodRays_tt->insert(goodRays_tt->end(), rays.begin(), rays.end());
    }
    delete rets;
    subVectors.clear();
    threads.clear();

    // again--------------------------------------------
    chunkSize = static_cast<int>(goodRays_tt->size() / threadAmount) + 1;
    rets = new std::vector<wrappedRays>();
    for (int i = 0; i < goodRays_tt->size(); i += chunkSize) {
        // [left_idx, right_idx)
        if (i >= goodRays_tt->size()) {
            subVectors.emplace_back(-1, -1);
        } else if (i + chunkSize >= goodRays_tt->size()) {
            subVectors.emplace_back(i, goodRays_tt->size());
        } else {
            subVectors.emplace_back(i, i + chunkSize);
        }
        rets->emplace_back();
    }
    i_ = 0;
    for (const auto &sub: subVectors) {
        threads.emplace_back(checker, std::ref(field), std::ref(node_ptrs), goodRays_tt, std::ref(sub), 0,
                             &rets->at(i_++));
    }
    for (auto &thread: threads) {
        try {
            thread.join();
        } catch (const std::exception &e) {
            coutLogger->writeErrorEntry("Exception caught when at threads.join() in main(): " + std::string(e.what()));
        } catch (...) {
            coutLogger->writeErrorEntry("threads.join() in main() encountered unknown exception");
        }
    }

    auto goodRays_t = new std::vector<Ray>;
    for (auto &[rays, done]: *rets) {
        if (done) goodRays_t->insert(goodRays_t->end(), rays.begin(), rays.end());
    }
    delete rets;
    delete goodRays_tt;
    subVectors.clear();
    threads.clear();
#endif

    // clean up rays that have no spectrum response
    // I've got no idea why they're there, but they are there, so yeah
    auto goodRays = new std::vector<Ray>();
    for (auto &ray: *goodRays_t) {
        bool flaga = false;
        for (auto &spectrumResp: ray.getIntensity_p()) {
            if (std::abs(spectrumResp) > 1e-10) {
                flaga = true;
            }
        }
        if (flaga) {
            goodRays->push_back(ray);
        }
    }
    delete goodRays_t;
    goodRays_t = nullptr;
    end = std::chrono::high_resolution_clock::now();
    std::cout << "GoodRays " << goodRays->size() << " " << goodRays->capacity() << std::endl;
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
#if VERTICES_CONFIG_CXX_STANDARD >= 20
    std::cout << "Time taken: " << duration << "." << std::endl;
#elif VERTICES_CONFIG_CXX_STANDARD <= 17
    std::cout << "Time taken: " << duration.count() << "." << std::endl;
#endif
    std::cout << "Ray count: " << rays->size() << "." << std::endl;

    // a "good ray" is a ray that successfully intersects with a face that
    // without any other face obstructing its path to the source point
    std::cout << "Good rays " << goodRays->size() << std::endl;
    delete rays;
    rays = nullptr;

    coutLogger->writeInfoEntry("Summing up every pixel's spectrum response...");
    // sum up every pixel's spectrum response
    for (auto &ray: *goodRays) {
        camera.addSingleRaySpectralRespToPixel(ray);
    }
    if (goodRays->size() > 2) {
        if (goodRays->at(0).getSourcePixel() == goodRays->at(1).getSourcePixel())
            coutLogger->writeErrorEntry("The first and second rays are the same pixel.");
    }
#ifdef VERTICES_CONFIG_SINGLE_THREAD_FOR_CAMRAYS
    // here we make a copy of the original pixel vectors as we want to mix them up
    auto camPixelsBackup = std::make_shared<std::vector<std::vector<Pixel>>>(*camera.getPixel2D());
    // then mix up neighboring pixel's intensity
    void mixer(const int left, const int right, Camera &camera, const std::shared_ptr<std::vector<std::vector<Pixel>>> &camPixelsBackup);
    mixer(0, spectralBands, camera, camPixelsBackup);
    camPixelsBackup.reset();
#endif
#ifdef VERTICES_CONFIG_MULTI_THREAD_FOR_CAMRAYS_WORKAROUND
    {
        std::vector<std::thread> threads;
        int threadAmount = HARDWARE_CONCURRENCY;
        std::vector<std::pair<int, int> > subVectors;
        int chunkSize = spectralBands / threadAmount;
        for (int i = 0; i < spectralBands; i += chunkSize) {
            // [left_idx, right_idx)
            if (i >= spectralBands) {
                subVectors.emplace_back(-1, -1);
            } else if (i + chunkSize >= spectralBands) {
                subVectors.emplace_back(i, spectralBands);
            } else {
                subVectors.emplace_back(i, i + chunkSize);
            }
        }
        coutLogger->writeInfoEntry("Here's normal");
        // here we make a copy of the original pixel vectors as we want to mix them up
        //threads.emplace_back(checker, std::ref(field), std::ref(node_ptrs), goodRays_tt, std::ref(sub), 0, &rets->at(i_++));
        auto camPixelsBackup = std::make_shared<std::vector<std::vector<Pixel> > >(*camera.getPixel2D());
        for (const auto &[fst, snd]: subVectors) {
            try {
                threads.emplace_back(mixer, fst, snd, std::ref(camera), std::ref(camPixelsBackup));
            } catch (std::exception &e) {
                coutLogger->writeErrorEntry(
                    "Exception caught when at threads.emplace_back() in main() at summing up: " + std::string(
                        e.what()));
            } catch (...) {
                coutLogger->writeErrorEntry(
                    "threads.emplace_back() in main() at summing up encountered unknown exception");
            }
        }
        for (auto &thread: threads) {
            try {
                thread.join();
            } catch (const std::exception &e) {
                coutLogger->writeErrorEntry(
                    "Exception caught when at threads.join() in main() at summing up: " + std::string(e.what()));
            } catch (...) {
                coutLogger->writeErrorEntry("threads.join() in main() at summing up encountered unknown exception");
            }
        }

        camPixelsBackup.reset();
        threads.clear();
    }
#endif

    for (auto &a: camera.sunlightSpectrum) {
        std::cout << a << " ";
    }
    std::cout << std::endl;


    // show the camera's spectrum response by every pixel
    /*for (int i = 0; i < spectralBands; i += 20) {
        std::cout << "For every pixel, spectrum response at " << i << "th band:" << std::endl;
        for (auto &row: *camera.getPixel2D()) {
            for (auto &col: row) {
                std::cout << std::setprecision(2) << col.getPixelSpectralResp()[i] << " ";
            }
            std::cout << std::endl;
        }
    }*/


    // save the camera's spectrum response to a bitmap
    double maxResp = -1.0f;
    constexpr int MUL = 8;
    auto testOutput = ToBitmap{camera.getPixel2D()->size() * MUL, camera.getPixel2D()->at(0).size() * MUL};
    //coutLogger->writeInfoEntry(testOutput.fillWithRandom().saveToTmpDir("", "random"));
    //coutLogger->writeInfoEntry(testOutput.fillWithZebra().saveToTmpDir("", "black_and_white"));
    //coutLogger->writeInfoEntry(testOutput.fillWithGray().saveToTmpDir("", "grayscale"));
    //coutLogger->writeInfoEntry(testOutput.fillWithAColor(0x5865f2).saveToTmpDir("", "blurple"));

    for (int i = 0; i < camera.getPixel2D()->size(); i++) {
        for (const auto &j: (*camera.getPixel2D())[i]) {
            if (j.getPixelSpectralResp()[20] > maxResp) {
                maxResp = j.getPixelSpectralResp()[20];
            }
        }
    }
    coutLogger->writeInfoEntry("Max resp: " + std::to_string(maxResp));
    for (int i = 0; i < testOutput.getResolutionX(); i += MUL) {
        for (int j = 0; j < testOutput.getResolutionY(); j += MUL) {
            //if ((*camera.getPixel2D())[i][j].getPixelSpectralResp()[20] > 1e-6) {

            //coutLogger->writeInfoEntry("val is: " + std::to_string((*camera.getPixel2D())[i][j].getPixelSpectralResp()[20]/maxResp) + " i,j " + std::to_string(i) + "," + std::to_string(j));

            //}

            //testOutput.setPixel(i, j, grayscaleToRGB(static_cast<std::uint8_t>(std::round((*camera.getPixel2D())[i][j].getPixelSpectralResp()[19] / maxResp * 255))));
            const auto val = static_cast<std::uint8_t>(std::round(
                (*camera.getPixel2D())[i / MUL][j / MUL].getPixelSpectralResp()[20] / maxResp *
                0xff));

            for (int ii = i; ii < i + MUL; ii++)
                for (int jj = j; jj < j + MUL; jj++)
                    testOutput.setPixel(ii, jj, grayscaleToRGB_int(val));
            //testOutput.setPixelByChannel(ii, jj, val & static_cast<std::uint8_t>(0xff0000), val & static_cast<std::uint8_t>(0x00ff00), val & static_cast<std::uint8_t>(0x0000ff));
            //testOutput.getMutImage().set(i, j, bmp::Pixel{val, val, val});
        }
    }
    auto outp = testOutput.saveToTmpDir("", "band20");
    // after the saveToTmpDir(), we obtained a path to newly generated file
    // we need to gather more info so we can attach to it
    testOutput.infoAppender->setIntInfo(InfoType::INT_BAND_COUNT, spectralBands)
                            .setIntInfo(InfoType::INT_BAND_OVERLAPPING, 0)
                            .setIntInfo(InfoType::INT_COUNT_X, camera.getPixel2D()->size())
                            .setIntInfo(InfoType::INT_COUNT_Y, camera.getPixel2D()->at(0).size())
                            .setIntInfo(InfoType::INT_FIELD_ITEM_COUNT, field.getObjects().size())
                            .setIntInfo(InfoType::INT_GOODRAYS_COUNT, goodRays->size())
                            .setIntInfo(InfoType::INT_FACES_IN_FIELD_COUNT, field.getAllFacesSize())
                            .setIntInfo(InfoType::INT_MULTITHREAD_COUNT, HARDWARE_CONCURRENCY)
                            .setIntInfo(InfoType::DOUBLE_FOV_X, FOVx)
                            .setIntInfo(InfoType::DOUBLE_FOV_Y, FOVy)
                            .setIntInfo(InfoType::INT_WAVELENGTH_LOW, UPPER_WAVELENGTH)
                            .setIntInfo(InfoType::INT_WAVELENGTH_HIGH, LOWER_WAVELENGTH)
                            .setIntInfo(InfoType::DOUBLE_MIXER_RATIO_U, mixRatioU)
                            .setIntInfo(InfoType::DOUBLE_MIXER_RATIO_D, mixRatioD)
                            .setIntInfo(InfoType::DOUBLE_MIXER_RATIO_L, mixRatioL)
                            .setIntInfo(InfoType::DOUBLE_MIXER_RATIO_R, mixRatioR)
                            .setIntInfo(InfoType::DOUBLE_CAMERA_HEIGHT, CAMERA_HEIGHT)
                            .setIntInfo(InfoType::DOUBLE_CAM_PLATE_HEIGHT, CAMERA_HEIGHT - CAM_IMG_DISTANCE)
                            .setIntInfo(InfoType::DOUBLE_CAM_PLATE_CENTER_X, camera.getImagePlaneCenter().getX())
                            .setIntInfo(InfoType::DOUBLE_CAM_PLATE_CENTER_Y, camera.getImagePlaneCenter().getY())
                            .setIntInfo(InfoType::DOUBLE_CAM_PLATE_CENTER_Z, camera.getImagePlaneCenter().getZ())
                            .setIntInfo(InfoType::DOUBLE_FIELD_BOX_MIN_X, field.getBoundsMin().getX())
                            .setIntInfo(InfoType::DOUBLE_FIELD_BOX_MIN_Y, field.getBoundsMin().getY())
                            .setIntInfo(InfoType::DOUBLE_FIELD_BOX_MIN_Z, field.getBoundsMin().getZ())
                            .setIntInfo(InfoType::DOUBLE_FIELD_BOX_MAX_X, field.getBoundsMax().getX())
                            .setIntInfo(InfoType::DOUBLE_FIELD_BOX_MAX_Y, field.getBoundsMax().getY())
                            .setIntInfo(InfoType::DOUBLE_FIELD_BOX_MAX_Z, field.getBoundsMax().getZ())
                            .tryAppend();

    coutLogger->writeInfoEntry(outp);
    std::thread([](const std::string &p) {
#ifdef _WIN32
        system(("start " + p).c_str());
#else
        system(("xdg-open " + p).c_str());
#endif
    }, outp).detach();

    coutLogger->writeInfoEntry("Goodbye!");

    stdoutLogger->writeInfoEntry("Bye!");

    fsLogger->writeInfoEntry("Bye-bye!");

    return 0;
}
