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
             const std::pair<int, int>& constSubVec, int idx, wrappedRays* ret, const Vec& sunlightVecR,
             const Vec& planeNormalVec) {
    // advanced return if empty
    if (constSubVec.first >= constSubVec.second) {
        ret->rays = {};
        ret->done = true;
        return;
    }

    auto goodRays_per_thread = new std::vector<Ray>();
    auto scatters_waiting_for_checking = new std::vector<Ray>();
    std::ostringstream s_;
    s_ << "checker thread " << std::this_thread::get_id() << " started ANd " << rays->size() << " " << constSubVec.first
            << " " << constSubVec.second;
#if VERTICES_CONFIG_CXX_STANDARD >= 20
    coutLogger->writeInfoEntry(s_.view());
#elif VERTICES_CONFIG_CXX_STANDARD <= 17
    coutLogger->writeInfoEntry(s_.str());
#endif

    const auto& allFaces = field.getAllFaces();
    for (auto it = constSubVec.first; it < constSubVec.second && idx == 0; ++it) {
        auto& ray = rays->at(it);
        ray.setAncestor(ray.getOrigin());

        for (const auto& face : allFaces)
            if (auto intersection = ray.mollerTrumboreIntersection(*face); NO_INTERSECT != intersection) {
                //if (face->getNormal().dot(ray.getDirection()) < 0.0) continue;
                if (face->isBorderWall) {
                    // reset the ray's info if it hitsthe wall and let it points to the refelcted direction
                    const Vec incidentDirection = ray.getDirection().getNormalized();
                    const Vec normal = face->getNormal().getNormalized();
                    const auto reflected = Vec(incidentDirection - (normal * normal.dot(incidentDirection)) * 2.0).
                        getNormalized();
                    ray.setOrigin(intersection).setDirection(reflected);
                    for (auto& val : ray.getMutIntensity_p()) val *= 0.99;
                    // iter back to current ray
                    --it;
                    continue;
                }

                auto noOtherInTheWay = true;
                auto theWay = Ray(intersection, planeNormalVec * -1.0);
                for (const auto& face_3 : field.getAllFaces()) {
                    if (face_3->isBorderWall) continue;
                    if (auto intersection_other = theWay.mollerTrumboreIntersection(*face_3); NO_INTERSECT !=
                        intersection_other && face_3 != face) {
                        //if (face_3->pointerToItemMTLDataset.d != 1.0) continue;
                        noOtherInTheWay = false;
                        break;
                    }
                }
                if (!noOtherInTheWay) continue;

                ray.setRayStopPoint(intersection);
                auto seeable = true;
                auto seenRay = Ray(intersection, sunlightVecR);
                for (const auto& face_2 : allFaces) {
                    if (face_2->isBorderWall) continue;
                    if (auto intersection_seen = seenRay.mollerTrumboreIntersection(*face_2); NO_INTERSECT !=
                        intersection_seen && face_2 != face) {
                        //if (face_2->pointerToItemMTLDataset.d != 1.0) continue;
                        seeable = false;
                        break;
                    }
                }
                if (seeable) {
                    goodRays_per_thread->push_back(ray);
                }
                else {
                    for (auto& val : ray.getMutIntensity_p()) val *= 0.1;
                    goodRays_per_thread->push_back(ray);
                }

                auto scatters = ray.scatter(*face, intersection, field.brdfList.at(face->faceBRDF),
                                            ray.getSourcePixel());
                for (auto& r : scatters) {
                    if (r.getSourcePixelPosInGnd() == BigO) {
                        r.setSourcePixelPosInGnd(ray.getSourcePixelPosInGnd());
                        r.setSourcePixel(ray.getSourcePixel());
                    }
                    r.setAncestor(ray.getAncestor());
                }
                goodRays_per_thread->insert(goodRays_per_thread->end(), scatters.begin(), scatters.end());
                scatters_waiting_for_checking->insert(scatters_waiting_for_checking->end(), scatters.begin(),
                                                      scatters.end());
            }
    }


    int ray_iter_step = 1;
    if (idx != 0) {
        delete scatters_waiting_for_checking;
        scatters_waiting_for_checking = rays;
    }
    else {
        coutLogger->writeInfoEntry(
            "checker thread " + std::to_string(scatters_waiting_for_checking->size()) + " generated");
    }
    if (static_cast<int>(std::log10(scatters_waiting_for_checking->size())) <= 4) ray_iter_step = 1;
    else ray_iter_step = static_cast<int>(std::floor(std::log10(scatters_waiting_for_checking->size())));
    for (auto it = 0; it < scatters_waiting_for_checking->size(); it += ray_iter_step) {
        auto& scat = scatters_waiting_for_checking->at(it);
        for (const auto& face : allFaces)
            if (auto intersection = scat.mollerTrumboreIntersection(*face); NO_INTERSECT != intersection) {
                //if (face->getNormal().dot(scat.getDirection()) < 0.0) continue;
                if (face->isBorderWall) {
                    const Vec incidentDirection = scat.getDirection().getNormalized();
                    const Vec normal = face->getNormal().getNormalized();
                    const auto reflected = Vec(incidentDirection - (normal * normal.dot(incidentDirection)) * 2.0).
                        getNormalized();
                    scat.setOrigin(intersection).setDirection(reflected);
                    for (auto& val : scat.getMutIntensity_p()) val *= 0.99;
                    --it;
                    continue;
                }

                auto noOtherInTheWay = true;
                auto theWay = Ray(intersection, planeNormalVec * -1.0);
                for (const auto& face_3 : allFaces) {
                    if (face_3->isBorderWall) continue;
                    if (auto intersection_other = theWay.mollerTrumboreIntersection(*face_3); NO_INTERSECT !=
                        intersection_other && face_3 != face) {
                        //if (face_3->pointerToItemMTLDataset.d != 1.0) continue;
                        noOtherInTheWay = false;
                        break;
                    }
                }
                if (!noOtherInTheWay) continue;

                scat.setRayStopPoint(intersection);

                auto seeable = true;
                auto seenRay = Ray(intersection, sunlightVecR);
                for (const auto& face_2 : allFaces) {
                    if (face_2->isBorderWall) continue;
                    if (auto intersection_seen = seenRay.mollerTrumboreIntersection(*face_2); NO_INTERSECT !=
                        intersection_seen && intersection_seen != intersection) {
                        //if (face_2->pointerToItemMTLDataset.d != 1.0) continue;
                        seeable = false;
                        break;
                    }
                }
                if (seeable) {
                    goodRays_per_thread->push_back(scat);
                }
                else {
                    for (auto& val : scat.getMutIntensity_p()) val *= 0.1;
                    goodRays_per_thread->push_back(scat);
                }
                //auto scatters = scat.scatter(*face, intersection, field.brdfList.at(face->faceBRDF), scat.getSourcePixel());
                //for (auto &r: scatters) {
                //    if (r.getSourcePixelPosInGnd() == BigO) {
                //        r.setSourcePixelPosInGnd(scat.getSourcePixelPosInGnd());
                //        r.setSourcePixel(scat.getSourcePixel());
                //    }
                //    r.setAncestor(scat.getAncestor());
                //}
                //scatters_waiting_for_checking_x2->insert(scatters_waiting_for_checking_x2->end(), scatters.begin(), scatters.end());
                //goodRays_per_thread->insert(goodRays_per_thread->end(), scatters.begin(), scatters.end());
            }
    }
    if (idx == 0) delete scatters_waiting_for_checking;
    scatters_waiting_for_checking = nullptr;

    //ret->rays.reserve(goodRays_per_thread->size());
    auto callerArg = new std::vector<Ray>();
    for (auto &rawR : *goodRays_per_thread) {
        bool f = false;
        for (auto &r : rawR.getMutIntensity_p()) {
            if (r > 1e-10) {
                f |= true;
                break;
            }
        }
        if (f) callerArg->push_back(rawR);
    }
    delete goodRays_per_thread;
    goodRays_per_thread = nullptr;
    if (idx == 0) {
        //ret->rays.insert(ret->rays.end(), callerArg->begin(), callerArg->end());
        checker(field, node_ptrs, callerArg, std::make_pair<int, int>(0, callerArg->size()), 1, ret, sunlightVecR,
                planeNormalVec);
    }
    else {
        ret->rays.insert(ret->rays.end(), callerArg->begin(), callerArg->end());
        ret->done = true;
    }
    delete callerArg;
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
                    ) / 4.0;

                    (*camera.getPixel2D())[i][j].getMutPixelSpectralResp()[k] += (
                        (*camPixelsBackup)[i - 1][j - 1].getPixelSpectralResp()[k] * mixRatioL * mixRatioU
                        + (*camPixelsBackup)[i - 1][j + 1].getPixelSpectralResp()[k] * mixRatioL * mixRatioD
                        + (*camPixelsBackup)[i + 1][j - 1].getPixelSpectralResp()[k] * mixRatioR * mixRatioU
                        + (*camPixelsBackup)[i + 1][j + 1].getPixelSpectralResp()[k] * mixRatioR * mixRatioD
                    ) / 4.0;
                } else {
                    if (i == 0 && j == 0)
                        (*camera.getPixel2D())[i][j].getMutPixelSpectralResp()[k] += (
                            (*camPixelsBackup)[i + 1][j + 1].getPixelSpectralResp()[k] * mixRatioD * mixRatioR
                            + (*camPixelsBackup)[i + 1][j].getPixelSpectralResp()[k] * mixRatioD
                            + (*camPixelsBackup)[i][j + 1].getPixelSpectralResp()[k] * mixRatioR
                        ) / 3.0;
                    else if (i == camera.getPixel2D()->size() - 1 && j == camera.getPixel2D()->at(0).size() - 1)
                        (*camera.getPixel2D())[i][j].getMutPixelSpectralResp()[k] += (
                            (*camPixelsBackup)[i - 1][j - 1].getPixelSpectralResp()[k] * mixRatioU * mixRatioL
                            + (*camPixelsBackup)[i - 1][j].getPixelSpectralResp()[k] * mixRatioU
                            + (*camPixelsBackup)[i][j - 1].getPixelSpectralResp()[k] * mixRatioL
                        ) / 3.0;
                    else if (i == 0 && j == camera.getPixel2D()->at(0).size() - 1)
                        (*camera.getPixel2D())[i][j].getMutPixelSpectralResp()[k] += (
                            (*camPixelsBackup)[i + 1][j - 1].getPixelSpectralResp()[k] * mixRatioD * mixRatioL
                            + (*camPixelsBackup)[i + 1][j].getPixelSpectralResp()[k] * mixRatioD
                            + (*camPixelsBackup)[i][j - 1].getPixelSpectralResp()[k] * mixRatioL
                        ) / 3.0;
                    else if (i == camera.getPixel2D()->size() - 1 && j == 0)
                        (*camera.getPixel2D())[i][j].getMutPixelSpectralResp()[k] += (
                            (*camPixelsBackup)[i - 1][j + 1].getPixelSpectralResp()[k] * mixRatioU * mixRatioR
                            + (*camPixelsBackup)[i - 1][j].getPixelSpectralResp()[k] * mixRatioU
                            + (*camPixelsBackup)[i][j + 1].getPixelSpectralResp()[k] * mixRatioR
                        ) / 3.0;
                    else if (i == 0 && j != 0)
                        (*camera.getPixel2D())[i][j].getMutPixelSpectralResp()[k] += (
                            (*camPixelsBackup)[i + 1][j + 1].getPixelSpectralResp()[k] * mixRatioU * mixRatioR
                            + (*camPixelsBackup)[i + 1][j - 1].getPixelSpectralResp()[k] * mixRatioU * mixRatioL
                            + (*camPixelsBackup)[i + 1][j].getPixelSpectralResp()[k] * mixRatioU
                            + (*camPixelsBackup)[i][j + 1].getPixelSpectralResp()[k] * mixRatioR
                            + (*camPixelsBackup)[i][j - 1].getPixelSpectralResp()[k] * mixRatioL
                        ) / 5.0;
                    else if (i != 0 && j == 0)
                        (*camera.getPixel2D())[i][j].getMutPixelSpectralResp()[k] += (
                            (*camPixelsBackup)[i + 1][j + 1].getPixelSpectralResp()[k] * mixRatioD * mixRatioR
                            + (*camPixelsBackup)[i - 1][j + 1].getPixelSpectralResp()[k] * mixRatioU * mixRatioR
                            + (*camPixelsBackup)[i][j + 1].getPixelSpectralResp()[k] * mixRatioR
                            + (*camPixelsBackup)[i + 1][j].getPixelSpectralResp()[k] * mixRatioD
                            + (*camPixelsBackup)[i - 1][j].getPixelSpectralResp()[k] * mixRatioU
                        ) / 5.0;
                    else if (i == camera.getPixel2D()->size() - 1 && j != camera.getPixel2D()->at(0).size() - 1)
                        (*camera.getPixel2D())[i][j].getMutPixelSpectralResp()[k] += (
                            (*camPixelsBackup)[i - 1][j + 1].getPixelSpectralResp()[k] * mixRatioL * mixRatioU
                            + (*camPixelsBackup)[i - 1][j - 1].getPixelSpectralResp()[k] * mixRatioR * mixRatioU
                            + (*camPixelsBackup)[i - 1][j].getPixelSpectralResp()[k] * mixRatioU
                            + (*camPixelsBackup)[i][j + 1].getPixelSpectralResp()[k] * mixRatioR
                            + (*camPixelsBackup)[i][j - 1].getPixelSpectralResp()[k] * mixRatioL
                        ) / 5.0;
                    else if (i != camera.getPixel2D()->size() - 1 && j == camera.getPixel2D()->at(0).size() - 1)
                        (*camera.getPixel2D())[i][j].getMutPixelSpectralResp()[k] += (
                            (*camPixelsBackup)[i + 1][j - 1].getPixelSpectralResp()[k] * mixRatioL * mixRatioU
                            + (*camPixelsBackup)[i - 1][j - 1].getPixelSpectralResp()[k] * mixRatioL * mixRatioD
                            + (*camPixelsBackup)[i][j - 1].getPixelSpectralResp()[k] * mixRatioD
                            + (*camPixelsBackup)[i + 1][j].getPixelSpectralResp()[k] * mixRatioU
                            + (*camPixelsBackup)[i - 1][j].getPixelSpectralResp()[k] * mixRatioL
                        ) / 5.0;
                    else coutLogger->writeErrorEntry("Unknown case in main() at summing up, unbelievable");
                }

                if (i >= 2 && j >= 2 && i <= camera.getPixel2D()->size() - 3 && j <= camera.getPixel2D()->at(0).size() -
                    3)
                    (*camera.getPixel2D())[i][j].getMutPixelSpectralResp()[k] += (
                        (*camPixelsBackup)[i - 2][j].getPixelSpectralResp()[k] * mixRatioU * mixRatioU
                        + (*camPixelsBackup)[i + 2][j].getPixelSpectralResp()[k] * mixRatioD * mixRatioD
                        + (*camPixelsBackup)[i][j - 2].getPixelSpectralResp()[k] * mixRatioL * mixRatioL
                        + (*camPixelsBackup)[i][j + 2].getPixelSpectralResp()[k] * mixRatioR * mixRatioR
                    ) / 4.0;
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

int main(int argc, char* argv[]) {
    if (argc == 2) {
        std::ifstream file(argv[1]);
        if (!file.is_open()) {
            coutLogger->writeErrorEntry(
                "The file u provided in the argument list cannot be opened: " + std::to_string(*argv[1]));
            return (26);
        }
        file.close();

        // now pass the filepath to the InfoAppender to read and print info
        auto infoAppender = InfoAppender(argv[1]);
        std::string data;
        try {
            infoAppender.tryRead(&data);
        }
        catch (std::exception& e) {
            coutLogger->writeErrorEntry("Error occurred in main() when tryRead(): " + std::to_string(*e.what()));
            return 27;
        } catch (...) {
            coutLogger->writeErrorEntry("Unknown error occurred in main() when tryRead()");
            return 28;
        }
        std::cout << data << std::endl;
        return 0;
    }

    coutLogger->writeInfoEntry("Hello there!");

    stdoutLogger->writeInfoEntry("Hi there!");

    fsLogger->writeInfoEntry("hello hi!");

    // quick abort if hardware don't support avx
    /*{
        constexpr int MUL = 2;
        auto outputs = new std::vector<ToBitmap>;

        const int bandLength = 20;
        for (int band = 0; band < spectralBands; band += bandLength)
            outputs->emplace_back(12, 12);

        const auto baseIntensityPercentage = 0.1;
        const auto maxRespOfAll = 2.1;
        const double newMaxRespOfAll = (1.0 + baseIntensityPercentage) * maxRespOfAll;
        for (int k = 0; k < spectralBands; k += bandLength) {
            auto testOutput = (*outputs)[k / bandLength];
            for (int i = 0; i < testOutput.getResolutionX(); i += MUL) {
                for (int j = 0; j < testOutput.getResolutionY(); j += MUL) {
                    const auto val = static_cast<std::uint8_t>(rand01() * 0xff);
                    for (int ii = i; ii < i + MUL; ii++)
                        for (int jj = j; jj < j + MUL; jj++)
                            testOutput.setPixel(ii, jj, grayscaleToRGB_int(val));
                }
            }
            //auto outpath = testOutput.saveToTmpDir("", "band" + std::to_string(k));
            // after the saveToTmpDir(), we obtained a path to newly generated file
            // we need to gather more info so we can attach to it

            //coutLogger->writeInfoEntry(outpath);
            testOutput.infoAppender->tryAppend();
        }
    }*/

    std::vector<std::string> objPaths;
    std::vector<std::string> mtlPaths;
    std::vector<std::pair<std::array<int, 2>, std::string> > BRDFPaths;
#ifdef _WIN32
    std::cout << "win32" << std::endl;

    objPaths.emplace_back(R"(C:\Users\root\3D Objects\mycube\mycube.obj)");
    objPaths.emplace_back(R"(C:\Users\root\3D Objects\mycube\mycube_x32.obj)");
    objPaths.emplace_back(R"(C:\Users\root\3D Objects\human-fbx\source\2a36_01.obj)");
    objPaths.emplace_back(R"(C:\Users\root\3D Objects\pile-of-old-tires-fbx\source\895e_01.obj)");
    for (int i = 1; i <= 41; i++)
        objPaths.emplace_back(
            R"(C:\Users\root\3D Objects\xiaomi_su7_fbx\objexport\groupByParts\21b8_)" + (i / 10 == 0
                    ? "0" + std::to_string(i)
                    : std::to_string(i)) + R"(.obj)");
    objPaths.emplace_back(R"(C:\Users\root\3D Objects\hot_desert_biome_obj\source\CalidiousDesert_obj_-z_y.obj)");

    mtlPaths.emplace_back(R"(C:\Users\root\3D Objects\mycube\mycube.mtl)");
    mtlPaths.emplace_back(R"(C:\Users\root\3D Objects\mycube\mycube_x32.mtl)");
    mtlPaths.emplace_back(R"(C:\Users\root\3D Objects\human-fbx\source\human.mtl)");
    mtlPaths.emplace_back(R"(C:\Users\root\3D Objects\pile-of-old-tires-fbx\source\tires_-z_y.mtl)");
    mtlPaths.emplace_back(R"(C:\Users\root\3D Objects\xiaomi_su7_fbx\objexport\groupByParts\su7_-z_yobj.mtl)");
    mtlPaths.emplace_back(R"(C:\Users\root\3D Objects\hot_desert_biome_obj\source\CalidiousDesert_obj_-z_y.obj)");

    // BRDFPaths' sequence should follow your object's sequence!!!!!!!!!!
    // and blue should always be the first, then green, then red, then shortwave infrared if possible

    BRDFPaths.emplace_back(std::array<int, 2>{0, 0}, R"(C:\Users\root\Downloads\empty_brdf_debug.binary)");
    BRDFPaths.emplace_back(std::array<int, 2>{0, 0}, R"(C:\Users\root\Downloads\empty_brdf_debug.binary)");
    BRDFPaths.emplace_back(std::array<int, 2>{0, 0}, R"(C:\Users\root\Downloads\empty_brdf_debug.binary)");
    BRDFPaths.emplace_back(std::array<int, 2>{0, 0}, R"(C:\Users\root\Downloads\empty_brdf_debug.binary)");
    for (int i = 1; i <= 41; i++)
        BRDFPaths.emplace_back(std::array<int, 2>{0, 0}, R"(C:\Users\root\Downloads\empty_brdf_debug.binary)");
    BRDFPaths.emplace_back(std::array<int, 2>{BLUE_UPPER, BLUE_LOWER},
                           R"(C:\Users\root\Downloads\debug.mini.459.479.txt)");
    BRDFPaths.emplace_back(std::array<int, 2>{GREEN_UPPER, GREEN_LOWER},
                           R"(C:\Users\root\Downloads\debug.mini.545.565.txt)");
    BRDFPaths.emplace_back(std::array<int, 2>{RED_UPPER, RED_LOWER},
                           R"(C:\Users\root\Downloads\debug.mini.620.670.txt)");

#elif __unix__ || __unix || __APPLE__ || __MACH__ || __linux__
    std::cout << "unix-like" << std::endl;

    objPaths.emplace_back(R"(/home/20009100240/3dmodel/mycube/mycube.obj)");
    objPaths.emplace_back(R"(/home/20009100240/3dmodel/mycube/mycube_x32.obj)");
    objPaths.emplace_back(R"(/home/20009100240/3dmodel/human-fbx/2a36_01.obj)");
    objPaths.emplace_back(R"(/home/20009100240/3dmodel/pile-of-old-tires-fbx/895e_01.obj)");
    for (int i = 1; i <= 41; i++)
        objPaths.emplace_back(R"(/home/20009100240/3dmodel/xiaomi_su7_towardsY/21b8_)" + (i/10 == 0 ? "0" + std::to_string(i) : std::to_string(i)) + R"(.obj)");
    objPaths.emplace_back(R"(/home/20009100240/3dmodel/hot_desert_biome_obj/source/CalidiousDesert_obj_-z_y.obj)");

    mtlPaths.emplace_back(R"(/home/20009100240/3dmodel/mycube/mycube.mtl)");
    mtlPaths.emplace_back(R"(/home/20009100240/3dmodel/mycube/mycube_x32.mtl)");
    mtlPaths.emplace_back(R"(/home/20009100240/3dmodel/human-fbx/human.mtl)");
    mtlPaths.emplace_back(R"(/home/20009100240/3dmodel/pile-of-old-tires-fbx/tires_-z_y.mtl)");
    mtlPaths.emplace_back(R"(/home/20009100240/3dmodel/xiaomi_su7_towardsY/su7_-z_yobj.mtl)");
    mtlPaths.emplace_back(R"(/home/20009100240/3dmodel/hot_desert_biome_obj/source/CalidiousDesert_obj_-z_y.mtl)");


    // BRDFPaths' sequence should follow your object's sequence!!!!!!!!!!
    // and blue should always be the first, then green, then red, then shortwave infrared if possible

    BRDFPaths.emplace_back(std::array<int, 2>{0, 0}, R"(/home/20009100240/3dmodel/BRDF/empty_brdf_debug.binary)");
    BRDFPaths.emplace_back(std::array<int, 2>{0, 0}, R"(/home/20009100240/3dmodel/BRDF/empty_brdf_debug.binary)");
    BRDFPaths.emplace_back(std::array<int, 2>{0, 0}, R"(/home/20009100240/3dmodel/BRDF/empty_brdf_debug.binary)");
    BRDFPaths.emplace_back(std::array<int, 2>{0, 0}, R"(/home/20009100240/3dmodel/BRDF/empty_brdf_debug.binary)");
    for (int i = 1; i <= 41; i++)
        BRDFPaths.emplace_back(std::array<int, 2>{0, 0}, R"(/home/20009100240/3dmodel/BRDF/empty_brdf_debug.binary)");
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
        Point(-FIELD_LENGTH_X / 2.0, -FIELD_LENGTH_Y / 2.0, -2),
        Point(FIELD_LENGTH_X / 2.0, FIELD_LENGTH_Y / 2.0, CAMERA_HEIGHT * 1.732)
    );
    // cube #1
    field.newClosedObject()
         .setOBJPath(objPaths.at(1))
         .setMTLPath(mtlPaths.at(1))
         .setCenter({11, 11, 4})
         .setScaleFactor({3, 2, 4})
         .setForwardAxis(6)
         .setUpAxis(2)
         .setNoNormalReqFromObjFile()
         // innerPoints should be in cube {-1, -1, -1}--{1, 1, 1}
         .setInnerPoints({
             {-0.1, -0.1, -0.1},
             {0.1, 0.1, 0.1},
             {0, 0, 0}
         })
         .readFromOBJ()
         .readFromMTL()
         .inspectNormalVecForAllFaces()
         .verbose(field.getObjects().size());

    // human
    field.newClosedObject()
         .setOBJPath(objPaths.at(2))
         .setMTLPath(mtlPaths.at(2))
         .setCenter({-14, -11, 0})
         .setScaleFactor({4.5, 4.5, 4.5}) // it's already 1.8m tall at the scale of 1.0
         .setForwardAxis(2)
         .setUpAxis(3)
         .readFromOBJ()
         .readFromMTL()
    .verbose(field.getObjects().size());
    //.inspectNormalVecForAllFaces();

    // piles of tires
    field.newClosedObject()
         .setOBJPath(objPaths.at(3))
         .setMTLPath(mtlPaths.at(3))
         .setCenter({-14, 11, 0})
         .setScaleFactor({4.5, 4.5, 4.5})
         .setForwardAxis(2)
         .setUpAxis(3)
         .readFromOBJ()
         .readFromMTL()
    .verbose(field.getObjects().size());
    //.inspectNormalVecForAllFaces();

    // xiaomi su7
    // its minX: -2.606214, maxX: 2.630939, minY: -0.029737, maxY: 1.435014, minZ: -1.100205, maxZ: 1.100205
    for (auto i = 4; i <= 41 + 3; i++) {
        field.newClosedObject()
             .setOBJPath(objPaths.at(i))
             .setMTLPath(mtlPaths.at(3))
             .setCenter({8, -8, 0})
             .setScaleFactor({4.5, 4.5, -4.5})
             .setForwardAxis(6)
             .setUpAxis(2)
             .readFromOBJ()
             .readFromMTL()
            .verbose(field.getObjects().size());
    }

    // cube #2
    /*field.newClosedObject()
         .setOBJPath(objPaths.at(0))
         .setMTLPath(mtlPaths.at(0))
         .setCenter({9, -9, 4})
         .setScaleFactor({3, 2, 4})
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
#endif    // cube #3
    field.newClosedObject()
         .setOBJPath(objPaths.at(0))
         .setMTLPath(mtlPaths.at(0))
         .setCenter({9, 9, 4})
         .setScaleFactor({2, 3, 4})
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
#endif    // cube #4
    field.newClosedObject()
         .setOBJPath(objPaths.at(0))
         .setMTLPath(mtlPaths.at(0))
         .setCenter({-9, -9, 4})
         .setScaleFactor({2, 3, 4})
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
*/
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


    // set up an camera
    // use default constructor for now
    auto camera = Camera();
    // field box's wall and ceiling
    field.newClosedObject()
         .setOBJPath(objPaths.front())
         .setMTLPath(mtlPaths.front())
         .setCenter({0.0, 0.0, (-2.0 + CAMERA_HEIGHT * 1.732) / 2.0})
         .setScaleFactor({FIELD_LENGTH_X * 0.98, FIELD_LENGTH_Y * 0.98, (2.0 + CAMERA_HEIGHT * 1.732) * 1.02 / 2.0})
         .setForwardAxis(6)
         .setUpAxis(2)
         .setNoNormalReqFromObjFile()
         // innerPoints should be in cube {-1, -1, -1}--{1, 1, 1}
         .setInnerPoints({
             {-3, -2, 2},
             {2, -3, -1},
             {2, 2, 2},
             {-3, 2, -2},
         })
         .readFromOBJ()
         .readFromMTL()
         .inspectNormalVecForAllFaces()
         .setSelfAsBorderWall()
    .verbose(field.getObjects().size());


    field.newOpenObject()
         .setOBJPath(objPaths.back())
         .setMTLPath(mtlPaths.at(0))
         .setForwardAxis(6)
         .setUpAxis(2)
         .setCenter({0, 0, 0})
         .setScaleFactor({800, 800, 1})
         .setNoNormalReqFromObjFile()
         .setThatCorrectFaceVertices({598, 0, 1})
         .setThatCorrectFaceIndex(0)
         .readFromOBJ()
         .readFromMTL()
         .inspectNormalVecForAllFaces()
    .verbose(field.getObjects().size());


    // only for timing
    // Get the starting timepoint
    auto start = std::chrono::high_resolution_clock::now();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    int brdf_cnt_t = 0, b_cnt = 0, size = static_cast<int>(BRDFPaths.size());
    for (auto& fieldItem : field.getObjects()) {
        if (size == 0) {
            fprintf(stderr, "Error when initializing BRDFs. Reason: not enough BRDFs in the list.\a\n");
            return 6;
        }
        if (fieldItem.isOpenMesh) {
            field.brdfList.push_back(new OpenBRDF());
            auto& brdfRef = *field.brdfList.at(brdf_cnt_t);
            fieldItem.brdfIdx = b_cnt++;
            std::cout << "Handling a set of open BRDF" << std::endl;
            if (brdf_cnt_t + 3 <= size) {
                dynamic_cast<OpenBRDF&>(brdfRef).OpenBRDFInsert(BRDFPaths.at(brdf_cnt_t).second.c_str(),
                                                                BRDFPaths.at(brdf_cnt_t).first);
                brdf_cnt_t++;
                dynamic_cast<OpenBRDF&>(brdfRef).OpenBRDFInsert(BRDFPaths.at(brdf_cnt_t).second.c_str(),
                                                                BRDFPaths.at(brdf_cnt_t).first);
                brdf_cnt_t++;
                dynamic_cast<OpenBRDF&>(brdfRef).OpenBRDFInsert(BRDFPaths.at(brdf_cnt_t).second.c_str(),
                                                                BRDFPaths.at(brdf_cnt_t).first);
                brdf_cnt_t++;
            }
            else {
                fprintf(stderr, "Error when initializing BRDFs. Reason: not enough open BRDFs in the list.\n\
                                        Required [%d][%d][%d], but BRDFPaths.size() is %llu\a\n", brdf_cnt_t + 1,
                        brdf_cnt_t + 2, brdf_cnt_t + 3, BRDFPaths.size());
                return 66;
            }
            std::cout << "Handling a set of open BRDF done" << std::endl;
        }
        else {
            try {
                //fieldItem.brdf = new ClosedBRDF();
                field.brdfList.push_back(new ClosedBRDF());
                auto& brdfRef = *field.brdfList.at(brdf_cnt_t);
                fieldItem.brdfIdx = b_cnt++;
                std::cout << "Handling a set of closed BRDF" << std::endl;
                if (brdf_cnt_t + 1 <= size) {
                    //dynamic_cast<ClosedBRDF &>(brdfRef).ClosedBRDFInsert(BRDFPaths.at(brdf_cnt_t).second.c_str());
                    dynamic_cast<ClosedBRDF&>(brdfRef).filename = BRDFPaths.at(brdf_cnt_t).second;
                }
                else {
                    fprintf(stderr, "Error when initializing BRDFs. Reason: not enough closed BRDFs in the list.\a\n");
                    return 67;
                }
                brdf_cnt_t++;
                std::cout << "Handling a set of closed BRDF done" << std::endl;
            }
            catch (const std::exception& e) {
                std::cerr << "\a[Debug] Caught exception in main(): " << e.what() << std::endl;
            } catch (...) {
                std::cerr << "[Debug] Caught unknown exception in main()\a\n";
            }
        }
    }

    int bcnt = 0;
    //cube, human, tire, xiaomisu7 * 41,cubewall, , ground
    dynamic_cast<BRDF*>(field.brdfList.at(bcnt++))->buildSpectrum("r").buildSpectrum("g").buildSpectrum("b");
    dynamic_cast<BRDF*>(field.brdfList.at(bcnt++))->buildSpectrum("r").buildSpectrum("g").buildSpectrum("b");
    dynamic_cast<BRDF*>(field.brdfList.at(bcnt++))->buildSpectrum("r").buildSpectrum("g").buildSpectrum("b");
    for (int iii = 0; iii < 41; iii++)
        dynamic_cast<BRDF*>(field.brdfList.at(bcnt++))->buildSpectrum("r").buildSpectrum("g").buildSpectrum("b");
    dynamic_cast<BRDF*>(field.brdfList.at(bcnt++))->none(1.0);
    dynamic_cast<BRDF*>(field.brdfList.at(bcnt++))->buildSpectrum("r").buildSpectrum("g").buildSpectrum("b");

    coutLogger->writeInfoEntry("aaa");
    if (field.brdfList.size() != b_cnt) {
        fprintf(stderr, "Error when initializing BRDFs. Reason: AAAAA not enough BRDFs in the list.\a\n");
        return 63;
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
    // generate a vector of all nodes' const shared pointers
    // so that I can conveiently traverse all nodes many times without visiting the multi-arch tree again and again
    // thanks to modern C++, RVO or NRVO is possible
    const std::vector<std::shared_ptr<Node>> node_ptrs = field.generateNodeList();
    for (int nodeIndex = 0; nodeIndex < field.nodeCount; nodeIndex++) {
        auto& node = node_ptrs.at(nodeIndex);
        for (int faceIndex = 0; faceIndex < node->boxedFaces.size(); faceIndex++) {
            auto &face = node->boxedFaces.at(faceIndex);
            if (face->faceBRDF == -1)
                std::cerr << "[Error] face #" << faceIndex << " of node #" << nodeIndex
                    << " has no brdfIdx in main()" << std::endl;
        }
    }

    std::cout << ">>>>>>>>>>the fovs are " << FOVx << " " << FOVy << std::endl;

    auto rays_r = camera.shootRays(1);
    auto rays = new std::vector<Ray>();
    rays->reserve(rays_r->size());
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

    coutLogger->writeInfoEntry(
        "camrea.pixel2d size:" + std::to_string(camera.getPixel2D()->size()) + " " + std::to_string(
            camera.getPixel2D()->at(0).size()));


    std::cout << "-------Camera----using----BVH----method----to-----accelerate--------" << std::endl;
    // only for timing
    start = std::chrono::high_resolution_clock::now();

    void checker(Field& field, const std::vector<std::shared_ptr<Node>>& node_ptrs, std::vector<Ray>* rays,
                 const std::pair<int, int>& constSubVec, int idx, wrappedRays* ret, const Vec& sunlightVecR,
                 const Vec& planeNormalVec);

#ifdef VERTICES_CONFIG_SINGLE_THREAD_FOR_CAMRAYS
    auto ret = new wrappedRays();
    /for (auto &ray: *rays) {
    checker(field, node_ptrs, rays, std::make_pair(0, rays->size()), 0, ret, std::ref(camera.sunlightDirectionReverse), std::ref(camera.getPlaneNormal()));
    /}
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
        }
        else {
            subVectors.emplace_back(i, i + chunkSize);
        }
        rets->emplace_back();
    }
    int i_ = 0;
    for (const auto &sub: subVectors) {
        threads.emplace_back(checker, std::ref(field), std::ref(node_ptrs), rays, std::ref(sub), 0,
                             &rets->at(i_++), std::ref(camera.sunlightDirectionReverse),
                             std::ref(camera.getImagePlaneOZ()));
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
    // actually after .join(), all rets should be done==true
    for (auto &[rays, done] : *rets) {
        if (done) goodRays_t->insert(goodRays_t->end(), rays.begin(), rays.end());
    }
    delete rets;
    subVectors.clear();
    threads.clear();

    /*
    // again--------------------------------------------
    chunkSize = static_cast<int>(goodRays_tt->size() / threadAmount) + 1;
    rets = new std::vector<wrappedRays>();
    for (int i = 0; i < goodRays_tt->size(); i += chunkSize) {
        // [left_idx, right_idx)
        if (i >= goodRays_tt->size()) {
            subVectors.emplace_back(-1, -1);
        } else if (i + chunkSize >= goodRays_tt->size()) {
            subVectors.emplace_back(i, goodRays_tt->size());
        }
        else {
            subVectors.emplace_back(i, i + chunkSize);
        }
        rets->emplace_back();
    }
    i_ = 0;
    for (const auto &sub: subVectors) {
        threads.emplace_back(checker, std::ref(field), std::ref(node_ptrs), goodRays_tt, std::ref(sub), 0,
                             &rets->at(i_++), std::ref(camera.sunlightDirectionReverse), std::ref(camera.getImagePlaneOZ()));
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
    threads.clear();*/

#endif

    // clean up rays that have no spectrum response
    // I've got no idea why they're there, but they are there, so yeah
    auto goodRays = new std::vector<Ray>();
    for (auto &ray: *goodRays_t) {
        bool flaga = false;
        for (auto &spectrumResp: ray.getIntensity_p()) {
            if (spectrumResp > 1e-10) {
                flaga = true;
                break;
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

    // here we make a copy of the original pixel vectors as we want to mix them up
    auto camPixelsBackup = std::make_shared<std::vector<std::vector<Pixel>>>(*camera.getPixel2D());
    // fetch max response per band

    auto maxRespPerBand = std::vector<double>(spectralBands, 0.0f);
    auto minRespPerBand = std::vector<double>(spectralBands, std::numeric_limits<double>::max());
    auto sumRespPerBand = std::vector<double>(spectralBands, 0.0f);
    double maxRespOfAll = -1.0f;
    double minRespOfAll = std::numeric_limits<double>::max();
    for (int band = 0; band < spectralBands; band += bandLength) {
        //outputs->emplace_back(camera.getPixel2D()->size() * MUL, camera.getPixel2D()->at(band).size() * MUL);
        for (int i = 0; i < camera.getPixel2D()->size(); i++)
            for (const auto& j : (*camera.getPixel2D())[i]) {
                if (j.getPixelSpectralResp()[band] > maxRespPerBand[band])
                    maxRespPerBand[band] = j.getPixelSpectralResp()[band];
                if (j.getPixelSpectralResp()[band] > maxRespOfAll)
                    maxRespOfAll = j.getPixelSpectralResp()[band];
                if (j.getPixelSpectralResp()[band] < minRespPerBand[band])
                    minRespPerBand[band] = j.getPixelSpectralResp()[band];
                if (j.getPixelSpectralResp()[band] < minRespOfAll)
                    minRespOfAll = j.getPixelSpectralResp()[band];
                //sumRespPerBand[band] += j.getPixelSpectralResp()[band];
            }
        coutLogger->writeInfoEntry("Max resp at band[" + std::to_string(band) + "] :" + std::to_string(maxRespPerBand[
            band]));
    }
    // and add some "initial background light intensity" to every pixel
    auto baseRespPerBand = std::vector<double>(spectralBands, 0.0f);
    for (int band = 0; band < spectralBands; band += bandLength)
        baseRespPerBand[band] = (maxRespPerBand[band] - minRespPerBand[band]) * 0.2f;

    maxRespPerBand = std::vector<double>(spectralBands, 0.0f);
    minRespPerBand = std::vector<double>(spectralBands, std::numeric_limits<double>::max());
    maxRespOfAll = -1.0f;
    minRespOfAll = std::numeric_limits<double>::max();
    for (auto& pixelRow : *camPixelsBackup)
        for (auto& pixel : pixelRow)
            for (int band = 0; band < spectralBands; band += bandLength) {
                pixel.getMutPixelSpectralResp()[band] += baseRespPerBand[band];
                if (pixel.getPixelSpectralResp()[band] > maxRespOfAll)
                    maxRespOfAll = pixel.getPixelSpectralResp()[band];
                if (pixel.getPixelSpectralResp()[band] < minRespOfAll)
                    minRespOfAll = pixel.getPixelSpectralResp()[band];
                if (pixel.getPixelSpectralResp()[band] > maxRespPerBand[band])
                    maxRespPerBand[band] = pixel.getPixelSpectralResp()[band];
                if (pixel.getPixelSpectralResp()[band] < minRespPerBand[band])
                    minRespPerBand[band] = pixel.getPixelSpectralResp()[band];
            }

    // here we make a copy of the original pixel vectors as we want to mix them up
    // then mix up neighboring pixel's intensity
    for (int i = 0; i < spectralBands; i++)
        if (i % bandLength == 0)
            mixer(i, i + 1, camera, camPixelsBackup);
    camPixelsBackup.reset();

    for (auto &a: camera.sunlightSpectrum) {
        std::cout << a << " ";
    }
    std::cout << std::endl;

    // save the camera's spectrum response to a bitmap
    constexpr int MUL = 2;
    auto outputs = new std::vector<ToBitmap>;

    for (int band = 0; band < spectralBands; band += bandLength)
        outputs->emplace_back(camera.getPixel2D()->size() * MUL, camera.getPixel2D()->at(0).size() * MUL);

    for (int k = 0; k < spectralBands; k += bandLength) {
        printf("band%d:\n", k);
        coutLogger->writeErrorEntry("band" + std::to_string(k) + ":");
        coutLogger->writeErrorEntry("InImg:(71,332) (192,44) (257,200) (374,194) || (26,158) (210,166) (312,162) (220,233)");
        auto testOutput = (*outputs)[k / bandLength];
        for (int i = 0; i < testOutput.getResolutionX(); i += MUL) {
            for (int j = 0; j < testOutput.getResolutionY(); j += MUL) {
                const auto val = static_cast<std::uint8_t>(std::min(255.0, std::round(6.35 * std::pow(
                                                                        ((*camera.getPixel2D())[i / MUL][j / MUL].
                                                                            getPixelSpectralResp()[k])
                                                                        / (maxRespPerBand[k]) * 0xff, 0.67))));

                if (i == 26*MUL && j == 158*MUL) coutLogger->writeErrorEntry(std::to_string(val) + ",");
                if (i == 71*MUL && j == 332*MUL) coutLogger->writeErrorEntry(std::to_string(val) + ",");
                if (i == 192*MUL && j == 44*MUL) coutLogger->writeErrorEntry(std::to_string(val) + ",");
                if (i == 210*MUL && j == 166*MUL) coutLogger->writeErrorEntry(std::to_string(val) + ",");
                if (i == 220*MUL && j == 233*MUL) coutLogger->writeErrorEntry(std::to_string(val) + ",");
                if (i == 257*MUL && j == 200*MUL) coutLogger->writeErrorEntry(std::to_string(val) + ",");
                if (i == 312*MUL && j == 162*MUL) coutLogger->writeErrorEntry(std::to_string(val) + ",");
                if (i == 374*MUL && j == 194*MUL) coutLogger->writeErrorEntry(std::to_string(val) + "\n");

                    /// ((1.0) * maxRespOfAll) * 0xff)));
                for (int ii = i; ii < i + MUL; ii++)
                    for (int jj = j; jj < j + MUL; jj++)
                        testOutput.setPixel(ii, jj, grayscaleToRGB_int(val));
                printf("%d%c", val, j >= testOutput.getResolutionY() - MUL ? '\n' : ',');
            }
        }
        puts("");
        auto outpath = testOutput.saveToTmpDir("", "band" + std::to_string(k));
        // after the saveToTmpDir(), we obtained a path to newly generated file
        // we need to gather more info so we can attach to it

        coutLogger->writeInfoEntry(outpath);
        testOutput.infoAppender->setIntInfo(InfoType::INT_BAND_COUNT, spectralBands)
                  .setIntInfo(InfoType::INT_BAND_OVERLAPPING, 0)
                  .setIntInfo(InfoType::INT_COUNT_X, camera.getPixel2D()->size())
                  .setIntInfo(InfoType::INT_COUNT_Y, camera.getPixel2D()->at(0).size())
                  .setIntInfo(InfoType::INT_FIELD_ITEM_COUNT, field.getObjects().size())
                  .setIntInfo(InfoType::INT_GOODRAYS_COUNT, goodRays->size())
                  .setIntInfo(InfoType::INT_FACES_IN_FIELD_COUNT, field.getAllFacesSize())
                  .setIntInfo(InfoType::INT_MULTITHREAD_COUNT, HARDWARE_CONCURRENCY)
                  .setDoubleInfo(InfoType::DOUBLE_FOV_X, FOVx)
                  .setDoubleInfo(InfoType::DOUBLE_FOV_Y, FOVy)
                  .setIntInfo(InfoType::INT_WAVELENGTH_LOW, UPPER_WAVELENGTH)
                  .setIntInfo(InfoType::INT_WAVELENGTH_HIGH, LOWER_WAVELENGTH)
                  .setIntInfo(InfoType::INT_THIS_BAND, k)
                  .setDoubleInfo(InfoType::DOUBLE_MIXER_RATIO_U, mixRatioU)
                  .setDoubleInfo(InfoType::DOUBLE_MIXER_RATIO_D, mixRatioD)
                  .setDoubleInfo(InfoType::DOUBLE_MIXER_RATIO_L, mixRatioL)
                  .setDoubleInfo(InfoType::DOUBLE_MIXER_RATIO_R, mixRatioR)
                  .setDoubleInfo(InfoType::DOUBLE_CAMERA_HEIGHT, CAMERA_HEIGHT)
                  .setDoubleInfo(InfoType::DOUBLE_CAM_PLATE_HEIGHT, CAMERA_HEIGHT - CAM_IMG_DISTANCE)
                  .setDoubleInfo(InfoType::DOUBLE_CAM_PLATE_CENTER_X, camera.getImagePlaneCenter().getX())
                  .setDoubleInfo(InfoType::DOUBLE_CAM_PLATE_CENTER_Y, camera.getImagePlaneCenter().getY())
                  .setDoubleInfo(InfoType::DOUBLE_CAM_PLATE_CENTER_Z, camera.getImagePlaneCenter().getZ())
                  .setDoubleInfo(InfoType::DOUBLE_FIELD_BOX_MIN_X, field.getBoundsMin().getX())
                  .setDoubleInfo(InfoType::DOUBLE_FIELD_BOX_MIN_Y, field.getBoundsMin().getY())
                  .setDoubleInfo(InfoType::DOUBLE_FIELD_BOX_MIN_Z, field.getBoundsMin().getZ())
                  .setDoubleInfo(InfoType::DOUBLE_FIELD_BOX_MAX_X, field.getBoundsMax().getX())
                  .setDoubleInfo(InfoType::DOUBLE_FIELD_BOX_MAX_Y, field.getBoundsMax().getY())
                  .setDoubleInfo(InfoType::DOUBLE_FIELD_BOX_MAX_Z, field.getBoundsMax().getZ())
                  .setDoubleInfo(InfoType::DOUBLE_CAM_OX_X, camera.getImagePlaneOX().getTail().getX())
                  .setDoubleInfo(InfoType::DOUBLE_CAM_OX_Y, camera.getImagePlaneOX().getTail().getY())
                  .setDoubleInfo(InfoType::DOUBLE_CAM_OX_Z, camera.getImagePlaneOX().getTail().getZ())
                  .setDoubleInfo(InfoType::DOUBLE_CAM_OY_X, camera.getImagePlaneOY().getTail().getX())
                  .setDoubleInfo(InfoType::DOUBLE_CAM_OY_Y, camera.getImagePlaneOY().getTail().getY())
                  .setDoubleInfo(InfoType::DOUBLE_CAM_OY_Z, camera.getImagePlaneOY().getTail().getZ())
                  .setDoubleInfo(InfoType::DOUBLE_SUNLIGHT_VEC_TO_UP_X, camera.sunlightDirectionReverse.getTail().getX())
                  .setDoubleInfo(InfoType::DOUBLE_SUNLIGHT_VEC_TO_UP_Y, camera.sunlightDirectionReverse.getTail().getY())
                  .setDoubleInfo(InfoType::DOUBLE_SUNLIGHT_VEC_TO_UP_Z, camera.sunlightDirectionReverse.getTail().getZ())
                  .tryAppend();

        std::thread([](const std::string& p) {
#ifdef _WIN32
            system(("start " + p).c_str());
#else
                system(("xdg-open " + p).c_str());
#endif
        }, outpath).detach();
    }

    coutLogger->writeInfoEntry("Goodbye!");

    stdoutLogger->writeInfoEntry("Bye!");

    fsLogger->writeInfoEntry("Bye-bye!");

    return 0;
}