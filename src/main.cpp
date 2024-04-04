// Using C++23 standard
//
// Created by root on 2024/3/13.
//

/* Copyright 2024 wtflmao. All Rights Reserved.
 *
 * Distributed under MIT license.
 * See file LICENSE/LICENSE.MIT.md or copy at https://opensource.org/license/mit
 */

#include "main.h"

int main() {

    // ===== debug only start =====
    coutLogger->writeInfoEntry("Hello there!");

    stdoutLogger->writeInfoEntry("Hi there!");

    fsLogger->writeInfoEntry("hello hi!");

    auto a = CoordTransform();
    Point x = {0.8, 0.6, 0};
    Point y = BigO;
    a.camToGnd(x, y);
    std::cout << y << std::endl;
    x = y;
    a.gndToCam(x, y);
    std::cout << y << std::endl;

    x = {10.0, 5.0, 0.3};
    a.gndToCam(x, y);
    std::cout << y << std::endl;
    x = y;
    a.camToGnd(x, y);
    std::cout << y << std::endl << std::endl;

    x = {0, 0, 0};
    a.camToImg(x, y);
    std::cout << y << std::endl;
    x = y;
    a.imgToCam(x, y);
    std::cout << y << std::endl;
    x = {CENTER_OF_CAMERA_IN_GND.at(0) + pixelSize * 1e-6 * resolutionX,
         CENTER_OF_CAMERA_IN_GND.at(1) + pixelSize * 1e-6 * resolutionY, 0.1};
    a.camToImg(x, y);
    std::cout << y << std::endl;
    x = y;
    a.imgToCam(x, y);
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
            Point(-200, -200, 0),
            Point(200, 200, 30)
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
            .setCenter({-50, -50, 0})
            .setScaleFactor({200, 400, 1})
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
                    dynamic_cast<ClosedBRDF &>(brdfRef).ClosedBRDFInsert(BRDFPaths.at(brdf_cnt_t).second.c_str());

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
                    const auto scatteredRays = ray.scatter(face, intersection, field.brdfList.at(obj.brdfIdx));
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
                                                                    field.brdfList.at(face->faceBRDF));
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
    camera.spatialPosition = {Point(-0.008, -0.006, CAMERA_HEIGHT), Point(0.008, 0.006, CAMERA_HEIGHT)};
    rays->clear();
    camera.buildSunlightSpectrum();

    std::cout << ">>>>>>>>>>the fovs are " << FOVx << " " << FOVy << std::endl;
    // after this there should be resolution X*Y rays
    auto rays_r = camera.shootRaysRandom(1);
    rays->insert(rays->end(), rays_r.begin(), rays_r.end());
    //delete rays_r;
    if (rays->empty()) {
        std::cout << "trying to deref a nullptr in main() from camera.shootRaysRandom() call\a" << std::endl;
        return 8;
    }
    auto goodRays_t = new std::vector<Ray>();

    std::cout << "-------Camera----using----BVH----method----to-----accelerate--------" << std::endl;
    // only for timing
    start = std::chrono::high_resolution_clock::now();

    int cnt = 0;
    bool flag = true;
    int goodCnt = 0, totCnt = 0;
    // use BVH to accelerate the determination of whether the ray intersecting
    for (int rayIndex = 0; rayIndex < rays->size() || flag; rayIndex++) {
        // get one ray at a time to avoid memory overhead
        auto ray = Ray();
        //if (cnt < resolutionX * resolutionY) {
        //ray = camera.shootRayRandom(cnt++);
        //ray.ancestor = ray.getOrigin();
        //rayIndex--;
        //} else {
        // now every raw ray from camera has been investigated, traverse the rays vector
        flag = false;
        ray = rays->at(rayIndex);
        //}
        // Iterate over all nodes(boxes) and using BVH algorithm
        for (int nodeIndex = 0; nodeIndex < field.nodeCount; nodeIndex++) {
            auto &node = node_ptrs[nodeIndex];
            // Check if the ray intersects with the node (box)
            if (ray.intersectsWithBox(node->bbox)) {
                // If intersects, iterate over all faces in this bounding box, the faces may come from diffrent objects
                for (int faceIndex = 0; faceIndex < node->boxedFaces.size(); faceIndex++) {
                    auto &face = node->boxedFaces[faceIndex];
                    if (auto intersection = ray.mollerTrumboreIntersection(*face); NO_INTERSECT != intersection) {
                        ray.setRayStopPoint(intersection);
                        //std::cout << "The ray " << rayIndex + 1 << " intersects the face #" << faceIndex + 1 << " at "
                        //        << intersection << " with intensity[0] " << ray.intensity_p[0] << std::endl;
                        // check if this ray is valid by checking if there's no any faces in the way from the intersection, in the direction of the REAL sunlight's direction
                        Ray ray_t = Ray(intersection, field.sunlightDirectionToGround * -1);
                        ray_t.setAncestor(ray.getAncestor());
                        bool validity = true;
                        Point intersection_t = BigO;
                        for (int nodeIndex_t = 0; nodeIndex_t < field.nodeCount && validity; nodeIndex_t++) {
                            auto &node_t = node_ptrs.at(nodeIndex_t);
                            // Check if the ray intersects with the node (box)
                            if (ray_t.intersectsWithBox(node_t->bbox)) {
                                // If intersects, iterate over all faces in this bounding box, the faces may come from diffrent objects
                                for (int faceIndex_t = 0;
                                     faceIndex_t < node_t->boxedFaces.size() && validity; faceIndex_t++) {
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
                            goodRays_t->push_back(ray);
                            //std::cout << goodCnt++;
                        } else {
                            //std::cout << "  ";
                        }
                        //std::cout << "," << totCnt++ << ")";
                        //std::cout << std::endl;

                        // here we handle the scattered rays
                        // the intensity for every scattered rays should be determined by BRDF(....)
                        // todo: use BRDF
                        for (const auto scatteredRays = ray.scatter(*face, intersection, field.brdfList.at(
                                face->faceBRDF)); const auto &ray_sp:
                             scatteredRays) {
                            for (int j = 0; j < scatteredRays.size(); j++) {
                                bool flag_tt = false;
                                for (int k = 0; k < scatteredRays.at(j).getIntensity_p().size(); k++)
                                    if (scatteredRays.at(j).getIntensity_p().at(k) > 1e-10) flag_tt = true;
                                if (flag_tt) {//rays->push_back(scatteredRays[j]);
                                    // for 2+ scattered rays, the source of them is not THE SUN but the original ray
                                    // so, eh, yeah, IDK how to write this, whatever, just see the code below
                                    Ray ray_tt = scatteredRays.at(j);
                                    ray_tt.setAncestor(ray_t.getAncestor());
                                    bool validity_tt = true;
                                    for (int nodeIndex_tt = 0;
                                         nodeIndex_tt < field.nodeCount && validity_tt; nodeIndex_tt++) {
                                        auto &node_tt = node_ptrs.at(nodeIndex_tt);
                                        // Check if the ray intersects with the node (box)
                                        if (ray_tt.intersectsWithBox(node_tt->bbox)) {
                                            // If intersects, iterate over all faces in this bounding box, the faces may come from diffrent objects
                                            for (int faceIndex_tt = 0;
                                                 faceIndex_tt < node_tt->boxedFaces.size() &&
                                                 validity_tt; faceIndex_tt++) {
                                                auto &face_tt = node_tt->boxedFaces.at(faceIndex_tt);
                                                if (auto intersection_tt = ray_tt.mollerTrumboreIntersection(
                                                            *face_tt); (
                                                        NO_INTERSECT != intersection_tt &&
                                                        intersection_tt != intersection_t)) {
                                                    // bad, has intersection, the original ray is not valid
                                                    validity_tt = false;
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
                                        goodRays_t->push_back(ray_tt);
                                        //printf("Good ray intensity [0]%lf, [20%%]%lf, [75%%]%lf, level%d\n", ray_tt.intensity_p.at(0), ray_tt.intensity_p.at(std::round(0.2*spectralBands)), ray_tt.intensity_p.at(0.75*spectralBands), ray_tt.scatteredLevel);
                                        //std::cout << goodCnt++;
                                    } else {
                                        //std::cout << "  ";
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
    std::cout << "Rays " << rays->size() << " " << rays->capacity() << std::endl;

    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Time taken: " << duration << "." << std::endl;
    std::cout << "Ray count: " << rays->size() << "." << std::endl;

    // clean up rays that have no spectrum response
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

    for (int i = 0; auto &ray: *goodRays) {
        //printf("Good ray intensity [0]%lf, [20%%]%lf, [75%%]%lf, level%d\n", ray.intensity_p.at(0), ray.intensity_p.at(std::round(0.2*spectralBands)), ray.intensity_p.at(std::round(0.75*spectralBands)), ray.scatteredLevel);
    }

    // a "good ray" is a ray that successfully intersects with a face that
    // without any other face obstructing its path to the source point
    std::cout << "Good rays " << goodRays->size() << std::endl;
    delete rays;
    rays = nullptr;

    // sum up every pixel's spectrum response
    for (auto &ray: *goodRays) {
        // calc the ray's spectrum response
        camera.addRaySpectrumResp(ray);
    }

    // show the camera's spectrum response by every pixel
    for (int i = 0; i < spectralBands; i += 5) {
        std::cout << "Spectrum response at " << i << "th band:" << std::endl;
        for (auto &row: *camera.spectralResp_p) {
            for (auto &col: row) {
                std::cout << col.getPixelSpectralResp().at(i) << " ";
            }
            std::cout << std::endl;
        }
    }


    coutLogger->writeInfoEntry("Goodbye!");

    stdoutLogger->writeInfoEntry("Bye!");

    fsLogger->writeInfoEntry("Bye-bye!");

    return 0;
}