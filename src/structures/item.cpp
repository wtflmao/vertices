//
// Created by root on 2024/3/15.
//

/* Copyright 2024 wtflmao. All Rights Reserved.
 *
 * Distributed under MIT license.
 * See file LICENSE/LICENSE.MIT.md or copy at https://opensource.org/license/mit
 */

#include "item.h"

// make a vector of raw pointers for every Triangle from the Item
std::vector<std::shared_ptr<Triangle> > Item::getFaceRefs() noexcept {
    std::vector<std::shared_ptr<Triangle> > refs;
    for (auto face: faces) {
        refs.emplace_back(std::make_shared<Triangle>(face));
    }
    std::cout << "oooo" << refs.size() << std::endl;
    return refs;
}

const std::vector<Triangle>& Item::getFaces() const noexcept {
    return faces;
}

const std::vector<Point>& Item::getVertices() const noexcept {
    return vertices;
}

std::vector<Triangle>& Item::getMutFaces() noexcept {
    return faces;
}

std::vector<Point>& Item::getMutVertices() noexcept {
    return vertices;
}

Item &Item::setScaleFactor(const std::array<double, 3> factor) noexcept {
    scaleFactor = factor;
    return *this;
}

const std::array<double, 3>& Item::getScaleFactor() const noexcept {
    return scaleFactor;
}

Item &Item::setCenter(Point pos) noexcept {
    center = pos;
    return *this;
}

const Point& Item::getCenter() const noexcept {
    return center;
}

Item::Item() = default;

Item &Item::setOBJPath(const std::string &objPath_t) noexcept {
    objPath = objPath_t;
    return *this;
}

Item &Item::setMTLPath(const std::string &mtlPath_t) noexcept {
    mtlPath = mtlPath_t;
    return *this;
}

// open mesh only
Item &Item::setThatCorrectFaceVertices(const std::array<int, 3> &correctFaceVertices_t) noexcept {
    thatCorrectFaceVertices = correctFaceVertices_t;
    isOpenMesh = true;
    return *this;
}

// open mesh only
Item &Item::setThatCorrectFaceIndex(const int tCFI) noexcept {
    thatCorrectFaceIndex = tCFI;
    isOpenMesh = true;
    return *this;
}

// closed mesh only
Item &Item::setInnerPoints(const std::vector<Point> &innerPoints_t) noexcept {
    innerPoints.assign(innerPoints_t.begin(), innerPoints_t.end());
    isOpenMesh = false;
    return *this;
}

Item &Item::setForwardAxis(const int axis) noexcept {
    forwardAxis = axis;
    return *this;
}

Item &Item::setUpAxis(const int axis) noexcept {
    upAxis = axis;
    return *this;
}

// open mesh only
const std::array<int, 3> &Item::getThatCorrectFaceVertices() const {
    if (!isOpenMesh) {
        std::cerr << "\a" << std::endl;
        throw std::runtime_error("Error when attempting to run Item::getThatCorrectFaceVertices() for open mesh");
    }
    return thatCorrectFaceVertices;
}

// open mesh only
int Item::getThatCorrectFaceIndex() const {
    if (!isOpenMesh) {
        std::cerr << "\a" << std::endl;
        throw std::runtime_error("Error when attempting to run Item::getThatCorrectFaceIndex() for open mesh");
    }
    return thatCorrectFaceIndex;
}

// closed mesh only
const std::vector<Point> &Item::getInnerPoints() const {
    if (isOpenMesh) {
        std::cerr << "\a" << std::endl;
        throw std::runtime_error("Error when attempting to run Item::getInnerPoints() for closed mesh");
    }
    return innerPoints;
}

Item &Item::readFromOBJ() {
    if (!readNewItem(objPath.c_str(), *this)) {
        std::cerr << "\aError when attempting to run Item::readOBJ() for " << objPath << std::endl;
        throw std::runtime_error("Error when attempting to run Item::readOBJ() for " + objPath);
    }
    return *this;
}

Item &Item::readFromMTL() {
    if (!readNewItem(mtlPath.c_str(), *this)) {
        std::cerr << "Error when attempting to run Item::readMTL() for " << mtlPath << std::endl;
        throw std::runtime_error("Error when attempting to run Item::readMTL() for " + mtlPath);
    }
    return *this;
}

// I've decided that one item only have one set of BRDF data
// if you want to add more, please divide the item into two different parts along with the boundary of different BRDF faces
// waveLength in namometer. i and j are normalized to 2000, belong to [0, 1]
float Item::getBRDFOpen(int waveLength, double i, double j, BRDF &b_ori) const noexcept {
    auto &b = dynamic_cast<OpenBRDF &>(b_ori);
    if (waveLength < UPPER_WAVELENGTH || waveLength > LOWER_WAVELENGTH) {
        return 0.0;
    } else if (waveLength >= BLUE_UPPER && waveLength < BLUE_LOWER) {
        return b.valMap->at({BLUE_UPPER, BLUE_LOWER}).at(static_cast<std::size_t>(std::round(i * 2000))).at(
                static_cast<short>(std::round(j * 2000)));
    } else if (waveLength >= GREEN_UPPER && waveLength < GREEN_LOWER) {
        return b.valMap->at({GREEN_UPPER, GREEN_LOWER}).at(static_cast<std::size_t>(std::round(i * 2000))).at(
                static_cast<short>(std::round(j * 2000)));
    } else if (waveLength >= RED_UPPER && waveLength <= RED_LOWER) {
        return b.valMap->at({RED_UPPER, RED_LOWER}).at(static_cast<std::size_t>(std::round(i * 2000))).at(
                static_cast<short>(std::round(j * 2000)));
    }
    // actually if u don't mess up with my settings in src/basic/BRDF.h, this condition should never be reached
    return 1.0;
}

/*
double Item::getBRDFClosed(int waveLength, BRDF &b_ori, double theta_i = 0.0, double phi_i = 0.0, double theta_r = 0.0,
                           double phi_r = 0.0) const noexcept {
    auto &b = dynamic_cast<ClosedBRDF &>(b_ori);
    auto tup = b.getBRDF(theta_i, phi_i, theta_r, phi_r);
    double R = std::get<0>(tup);
    double G = std::get<1>(tup);
    double B = std::get<2>(tup);
    if (waveLength < UPPER_WAVELENGTH || waveLength > LOWER_WAVELENGTH) {
        return 0.0;
    } else if (waveLength >= BLUE_UPPER && waveLength < BLUE_LOWER) {
        return B;
    } else if (waveLength >= GREEN_UPPER && waveLength < GREEN_LOWER) {
        return G;
    } else if (waveLength >= RED_UPPER && waveLength <= RED_LOWER) {
        return R;
    } else {
        // actually if u don't mess up with my settings in src/basic/BRDF.h, this condition should never be reached
        return 1.0;
    }
}
*/

std::vector<std::array<int, 3> > &Item::getMutFWVR() noexcept {
    return facesWithVertexRefs;
}

const std::vector<std::array<int, 3> > &Item::getFWVR() const noexcept {
    return facesWithVertexRefs;
}

inline double votingPower(const double x, const double y, const double z) noexcept {
    return std::abs(x + y + z) < 1e-10
               ? 1
               : (2.0 / (1.0 + -4.0 * x / (x + y + z) + std::exp(-4.0 * x / (x + y + z))) + 2.0 / (
                      1.0 + -4.0 * y / (x + y + z) + std::exp(-4.0 * y / (x + y + z))) + 2.0 / (
                      1.0 + -4.0 * z / (x + y + z) + std::exp(-4.0 * z / (x + y + z)))) / 3.0;
}

Item &Item::inspectNormalVecForAllFaces() noexcept {
    if (faces.empty()) {
        return *this;
    }

    if (isOpenMesh) {
        // open mesh has its unique way to propagate correct normal vector direction
        // it needs a pre-given face with its CORRECT normal vector direction
        // and propagate to its neighboring faces in the space

        // create a vertices list to record which vertex is shared by which face
        // vector is generally faster than map when traversing and inserting one by one

        std::cout << "openmesh normal here" << std::endl;
        auto vertexAdjList_p = std::make_shared<std::vector<std::vector<int> > >(vertices.size());
        //vertexAdjList.reserve(vertices.size());
        auto &vertexAdjList = *vertexAdjList_p;
        for (int m = 0; m < vertices.size(); m++) {
            vertexAdjList[m] = std::vector<int>({});
        }

        //std::cout << "openmesh normal hererrrrrrrrr" << std::endl;

        const auto &fwvr = facesWithVertexRefs;
        // traverse the face list to update the adj list
        for (int i = 0; i < fwvr.size(); i++) {
            if (fwvr[i][0] >= vertexAdjList.size() ||
                fwvr[i][1] >= vertexAdjList.size() ||
                fwvr[i][2] >= vertexAdjList.size()) {
                throw std::out_of_range("Index out of range in vertexAdjList");
            } else {
                vertexAdjList[fwvr[i][0]].push_back(i);
                vertexAdjList[fwvr[i][1]].push_back(i);
                vertexAdjList[fwvr[i][2]].push_back(i);
            }
        }
        //std::cout << "openmesh normal herecccc" << std::endl;

        // remember `thatCorrectedFaceVertices` and `that...Index` and `vertexAdjList` start from 0

        // the checked faces
        std::set<int> checkedFaces;
        checkedFaces.insert(thatCorrectFaceIndex);
        //std::cout << "openmesh normal heresssssssssss" << std::endl;

        // vertices of unchecked faces
        std::queue<int> verticesOfUncheckedFaces;
        std::set<int> verticesOfUncheckedFacesSet; // for non-duplicate pushing back
        verticesOfUncheckedFaces.push(thatCorrectFaceVertices[0]);
        verticesOfUncheckedFaces.push(thatCorrectFaceVertices[1]);
        verticesOfUncheckedFaces.push(thatCorrectFaceVertices[2]);
        verticesOfUncheckedFacesSet.insert(thatCorrectFaceVertices[0]);
        verticesOfUncheckedFacesSet.insert(thatCorrectFaceVertices[1]);
        verticesOfUncheckedFacesSet.insert(thatCorrectFaceVertices[2]);
        //std::cout << "openmesh normal here ddd" << std::endl;

        // traverse adjacent list
        // i and l is vertex index, j and k are face index figures
        // sometimes not every face is reachable from the correct face, but idc
        while (!verticesOfUncheckedFaces.empty()) {
            const int i = verticesOfUncheckedFaces.front();
            for (auto j: vertexAdjList[i]) {
                if (!checkedFaces.contains(j)) {
                    // not found, inspect the face
                    // find the neighboring face that is correct
                    for (int k = 0; k < vertexAdjList[i].size(); k++) {
                        if (checkedFaces.contains(k)) {
                            if (faces[k].getNormal().dot(faces[j].getNormal()) < 0) {
                                // j's normal is wrong, revise it
                                faces[j].setNormalVec(Vec(faces[j].getNormal()) *= -1);
                            }
                            checkedFaces.insert(j);
                            // find face j's all vertices
                            for (auto l: fwvr[j]) {
                                if (!verticesOfUncheckedFacesSet.contains(l)) {
                                    verticesOfUncheckedFaces.push(l);
                                    verticesOfUncheckedFacesSet.insert(l);
                                }
                            }
                        } else {
                            // not found, what the duck???
                        }
                    }
                } else {
                    // face found, the normal vec is correct
                }
            }
            verticesOfUncheckedFaces.pop();
            // do not remove the element from the set, it is for non-duplicate pushing back
        }
        // clean up memory
        vertexAdjList_p.reset();
    } else {
        // closed mesh needs some pre-given points that INSIDE the object
        // and use Vec(insider point A, centroid of a face) cross face's normal vector to
        // determine if the normal vec direction held by this face is correct or not
        // and then traverse every insider points, let them vote, use the vote result to determine
        // the farther insider point is, the less voting power it has at this face will be

        std::cout << "closedmesh normal here" << std::endl;
        for (auto &face: faces) {
            double sumCorrect = 0.0;
            double sumWrong = 0.0;
            // traverse every inner point
            for (auto &point: innerPoints) {
                if (face.getNormal().dot(Vec(point, face.getCentroid())) < 0) {
                    sumWrong += votingPower(std::abs(point.getX() - face.getCentroid().getX()),
                                            std::abs(point.getY() - face.getCentroid().getY()),
                                            std::abs(point.getZ() - face.getCentroid().getZ()));
                } else {
                    sumCorrect += votingPower(std::abs(point.getX() - face.getCentroid().getX()),
                                              std::abs(point.getY() - face.getCentroid().getY()),
                                              std::abs(point.getZ() - face.getCentroid().getZ()));
                }
            }

            if (sumWrong > sumCorrect) {
                face.setNormalVec(Vec(face.getNormal()) *= -1);
            }
        }
    }
    // free the memory it consumed, as I shouldn't use it anymore
    facesWithVertexRefs.clear();
    return *this;
}
