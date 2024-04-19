// Using C++23 standard
//
// Created by root on 2024/3/13.
//

/* Copyright 2024 wtflmao. All Rights Reserved.
 *
 * Distributed under MIT license.
 * See file LICENSE/LICENSE.MIT.md or copy at https://opensource.org/license/mit
 */

#ifndef VERTICES_CONFIG_
#define VERTICES_CONFIG_

//#define VERTICES_CONFIG_SINGLE_THREAD_FOR_CAMRAYS
#define VERTICES_CONFIG_MULTI_THREAD_FOR_CAMRAYS_WORKAROUND

#endif

#include "main.h"

void checker(Field &field, const std::vector<std::shared_ptr<Node> > &node_ptrs, const std::vector<Ray> *rays,
             const std::pair<int, int> &constSubVec,
             int idx, wrappedRays *ret) {
    // advanced return if empty
    if (constSubVec.first == constSubVec.second) {
        ret->rays = {};
        ret->done = true;
        return;
    }

    auto goodRays_per_thread = new std::vector<Ray>();
    std::ostringstream s_;
    s_ << "checker thread " << std::this_thread::get_id() << " started ";
    coutLogger->writeInfoEntry(s_.view());

    s_ << "ANd " << rays->size() << " " << constSubVec.first << " " << constSubVec.second;
    coutLogger->writeInfoEntry(s_.view());
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
        auto ray = rays->at(it);
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
                        //std::cout << "(";
                        if (validity) {
                            goodRays_per_thread->push_back(std::move(ray));
                            //std::cout << goodCnt++;
                        } else {
                            //std::cout << "  ";
                            continue;
                        }
                        //std::cout << "," << totCnt++ << ")";
                        //std::cout << std::endl;

                        // here we handle the scattered rays
                        // the intensity for every scattered rays should be determined by BRDF(....)
                        for (const auto scatteredRays = ray.scatter(*face, intersection, field.brdfList.at(
                                                                        face->faceBRDF),
                                                                    ray.getSourcePixel()); const auto &ray_sp:
                             scatteredRays) {
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
                                                    intersection_tt != intersection_t)) {
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
                                        //printf("Good ray intensity [0]%lf, [20%%]%lf, [75%%]%lf, level%d\n", ray_tt.intensity_p.at(0), ray_tt.intensity_p.at(std::round(0.2*spectralBands)), ray_tt.intensity_p.at(0.75*spectralBands), ray_tt.scatteredLevel);
                                        //std::cout << goodCnt++;
                                    } else {
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
    s << "checker thread " << std::this_thread::get_id() << " done" << std::endl;
    coutLogger->writeInfoEntry(s.view());
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
    x = {CENTER_OF_CAMERA_IN_GND.at(0) + pixelSize * 1e-6 * resolutionX,
         CENTER_OF_CAMERA_IN_GND.at(1) + pixelSize * 1e-6 * resolutionY, 0.1};
    coordTransform->camToImg(x, y);
    std::cout << y << std::endl;
    x = y;
    coordTransform->imgToCam(x, y);
    std::cout << y << std::endl;
    // ====== debug only ends =====

    //std::cout << Triangle(Point(-0.5, 0.173092, 0.5), Point(-0.498325, 0.173198, 0.5), Point(-0.5, 0.173218, 0.498325)).
    //        getNormal().tail << std::endl;
#ifdef _WIN32
    std::cout << "win32" << std::endl;

    std::vector<std::string> objPaths;
    objPaths.emplace_back(R"(C:\Users\root\3D Objects\mycube\mycube.obj)");
    objPaths.emplace_back(R"(C:\Users\root\3D Objects\hot_desert_biome_obj\source\CalidiousDesert_obj_-z_y.obj)");
    objPaths.emplace_back(R"(C:\Users\root\3D Objects\snow_apls_low_poly_obj\source\Mesher_-z_y.obj)");
    objPaths.emplace_back(R"(C:\Users\root\3D Objects\F22_blender\F22.obj)");

    std::vector<std::string> mtlPaths;
    mtlPaths.emplace_back(R"(C:\Users\root\3D Objects\mycube\mycube.mtl)");

    // BRDFPaths' sequence should follow your object's sequence!!!!!!!!!!
    // and blue should always be the first, then green, then red, then shortwave infrared if possible
    std::vector<std::pair<std::array<int, 2>, std::string>> BRDFPaths;
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

    std::vector<std::string> objPaths;
    objPaths.emplace_back(R"(/home/20009100240/3dmodel/mycube/mycube.obj)");
    objPaths.emplace_back(R"(/home/20009100240/3dmodel/hot_desert_biome_obj/source/CalidiousDesert_obj_-z_y.obj)");
    objPaths.emplace_back(R"(/home/20009100240/3dmodel/snow_apls_low_poly_obj/source/Mesher_-z_y.obj)");
    objPaths.emplace_back(R"(/home/20009100240/3dmodel/F22_blender/F22.obj)");

    std::vector<std::string> mtlPaths;
    mtlPaths.emplace_back(R"(/home/20009100240/3dmodel/mycube/mycube.mtl)");
#else
    std::cout << "other system, quitting" << std::endl;
    exit(0);
#endif

    std::cout << "Setting up field..." << std::endl;
    Field field = Field(
        Point(-20, -20, -1),
        Point(20, 20, 30)
    );

    field.newClosedObject()
            .setOBJPath(objPaths.at(0))
            .setMTLPath(mtlPaths.at(0))
            .setCenter({0, 0, 0})
            .setScaleFactor({1, 1, 1})
            .setForwardAxis(6)
            .setUpAxis(2)
            .setInnerPoints({{-1, -1, -1},
                             {1,  1,  1},
                             {0,  0,  0}})
            .readFromOBJ()
            .readFromMTL()
            .inspectNormalVecForAllFaces();

    coutLogger->writeInfoEntry(std::format("Object #{} has been loaded", field.getObjects().size()));
    coutLogger->writeInfoEntry(std::format("Object #{} has {} faces", field.getObjects().size(),
                                           field.getObjects().back().getFaces().size()));
    coutLogger->writeInfoEntry(std::format("Object #{} has {} vertices", field.getObjects().size(),
                                           field.getObjects().back().getVertices().size()));


    field.newOpenObject()
            .setOBJPath(objPaths.at(1))
            .setMTLPath(mtlPaths.at(0))
            .setForwardAxis(6)
            .setUpAxis(2)
            .setCenter({0, 0, 0})
            .setScaleFactor({200, 200, 1})
            .setThatCorrectFaceVertices({598, 0, 1})
            .setThatCorrectFaceIndex(0)
            .readFromOBJ()
            .readFromMTL()
            .inspectNormalVecForAllFaces();

    coutLogger->writeInfoEntry(std::format("Object #{}  has been loaded", field.getObjects().size()));
    coutLogger->writeInfoEntry(std::format("Object #{} has {} faces", field.getObjects().size(),
                                           field.getObjects().back().getFaces().size()));
    coutLogger->writeInfoEntry(std::format("Object #{} has {} vertices", field.getObjects().size(),
                                           field.getObjects().back().getVertices().size()));


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

    field.buildBVHTree();

    {
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
            for (int faceIndex=0; faceIndex < obj.getFaces().size(); faceIndex++) {
                auto &face = obj.getFaces().at(faceIndex);
                auto intersection = ray.mollerTrumboreIntersection(face);
                if (NO_INTERSECT != intersection) {
                    ray.setRayStopPoint(intersection);
                    std::cout << "The ray " << rayIndex+1 << " intersects the face " << faceIndex+1 << " of object " << objIndex+1 << " at " << intersection << std::endl;
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
    std::cout << "Time taken: " << duration << "." << std::endl;
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
                        for (const auto scatteredRays = ray.scatter(*face,
                                                                    intersection,
                                                                    field.brdfList.at(face->faceBRDF),
                                                                    ray.getSourcePixel());
                             const auto &ray_sp:
                             scatteredRays) {
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
    std::cout << "Time taken: " << duration << "." << std::endl;
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

    void checker(Field &field, const std::vector<std::shared_ptr<Node> > &node_ptrs, const std::vector<Ray> *rays,
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
    for (auto &ray: *rays) {
        checker(field, node_ptrs, rays, std::make_pair(0, rays->size()), 0, ret);
    }
    auto *goodRays_t = &ret->rays;

#endif
#ifdef VERTICES_CONFIG_MULTI_THREAD_FOR_CAMRAYS_WORKAROUND
    std::vector<std::thread> threads;
    std::vector<std::pair<int, int> > subVectors;
    auto threadAmount = std::max(1u, std::thread::hardware_concurrency());
    int chunkSize = static_cast<int>(rays->size() / threadAmount);
    auto rets = new std::vector<wrappedRays>();
    for (int i = 0; i < rays->size(); i += chunkSize) {
        // [left_idx, right_idx)
        if (i >= rays->size()) { subVectors.emplace_back(-1, -1); } else if (i + chunkSize >= rays->size()) {
            subVectors.emplace_back(i, rays->size());
        } else { subVectors.emplace_back(i, i + chunkSize); }
        rets->emplace_back();
    }
    int i_ = 0;
    for (const auto &sub: subVectors) {
        threads.emplace_back(checker, std::ref(field), std::ref(node_ptrs), rays, std::ref(sub), 0, &rets->at(i_));
    }
    for (auto &thread: threads) {
        thread.join();
    }
    auto goodRays_t = new std::vector<Ray>;
    for (auto &[rays, done]: *rets) {
        if (done) goodRays_t->insert(goodRays_t->end(), rays.begin(), rays.end());
    }
    delete rets;
#endif

    // clean up rays that have no spectrum response
    // I've got no idea why they're there, but they are there, so yeah
    auto goodRays = new std::vector<Ray>();
    for (auto &ray: *goodRays_t) {
        bool flaga = false;
        for (auto &spectrumResp: ray.getIntensity_p()) {
            if (spectrumResp > 1e-10) {
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
    std::cout << "Time taken: " << duration << "." << std::endl;
    std::cout << "Ray count: " << rays->size() << "." << std::endl;



    for (int i = 0; auto &ray: *goodRays) {
        //printf("Good ray intensity [0]%lf, [20%%]%lf, [75%%]%lf, level%d\n", ray.intensity_p.at(0), ray.intensity_p.at(std::round(0.2*spectralBands)), ray.intensity_p.at(std::round(0.75*spectralBands)), ray.scatteredLevel);
    }

    // a "good ray" is a ray that successfully intersects with a face that
    // without any other face obstructing its path to the source point
    std::cout << "Good rays " << goodRays->size() << std::endl;
    delete rays;
    rays = nullptr;

    // sum up every pixel's spectrum response
    {
        int i = 0;
        for (auto &ray: *goodRays) {
            std::cout << "goodRays[" << i++ << "]"; //<< std::endl;
            //for (auto &r: ray) {
            std::cout << std::setprecision(4) << ray.getOrigin() << " " << ray.getAncestor() << " " << ray.
                    getSourcePixelPosInGnd() << "\n";
            //}
            //std::cout << std::endl;
            // calc the ray's spectrum response
            camera.addSingleRaySpectralRespToPixel(ray);
        }
    }

    for (auto &a: camera.sunlightSpectrum) {
        std::cout << a << " ";
    }
    std::cout << std::endl;


    // show the camera's spectrum response by every pixel
    for (int i = 0; i < spectralBands; i += 20) {
        std::cout << "For every pixel, spectrum response at " << i << "th band:" << std::endl;
        for (auto &row: *camera.getPixel2D()) {
            for (auto &col: row) {
                std::cout << std::setprecision(2) << col.getPixelSpectralResp()[i] << " ";
            }
            std::cout << std::endl;
        }
    }


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
                                                     (*camera.getPixel2D())[i/MUL][j/MUL].getPixelSpectralResp()[20] / maxResp *
                                                     255));

            for (int ii = i; ii < i + MUL; ii++)
                for (int jj = j; jj < j + MUL; jj++)
                    testOutput.setPixelByChannel(ii, jj, val, val, val);
                //testOutput.getMutImage().set(i, j, bmp::Pixel{val, val, val});

        }
    }
    auto outp = testOutput.saveToTmpDir("", "band20");
    coutLogger->writeInfoEntry(outp);
    system(("start " + outp).c_str());


    coutLogger->writeInfoEntry("Goodbye!");

    stdoutLogger->writeInfoEntry("Bye!");

    fsLogger->writeInfoEntry("Bye-bye!");

    return 0;
}