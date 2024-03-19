// Using C++23 standard
//
// Created by root on 2024/3/13.
//

#include "main.h"

int main() {
    Field field = Field(
            Point(-200, -200, 0),
            Point(200, 200, 30)
            );
    field.insertObject(
        std::string(R"(C:\Users\root\3D Objects\mycube\mycube.obj)"),
        std::string(R"(C:\Users\root\3D Objects\mycube\mycube.mtl)"),
        {2, 2, 0.5},
        Point(0, 0, 0.15)
    );
    field.insertObject(
            std::string(R"(C:\Users\root\3D Objects\hot_desert_biome_obj\source\CalidiousDesert_obj.obj)"),
            std::string(R"(C:\Users\root\3D Objects\mycube\mycube.mtl)"),
            {200, 400, 2},
            Point(-50, -50, 0)
            );
    field.insertObject(
            std::string(R"(C:\Users\root\3D Objects\snow_apls_low_poly_obj\source\Mesher.obj)"),
            std::string(R"(C:\Users\root\3D Objects\mycube\mycube.mtl)"),
            {200, 400, 2},
            Point(25, 25, -0.1)
            );
    field.insertObject(
            std::string(R"(C:\Users\root\3D Objects\F22_blender\F22.obj)"),
            std::string(R"(C:\Users\root\3D Objects\mycube\mycube.mtl)"),
            {9.5, 9.5, 3},
            Point(-15, 2, 0.5)
            );

    field.buildBVHTree();

    std::vector<Ray> rays;
    rays.emplace_back(Point(20, 20, 20), Vec(Point(-0.01,-0.015,-1)));
    rays.emplace_back(Point(3, 3, 3), Vec(Point(-1,-1,-1)));
    rays.emplace_back(Point(-0.5, -0.5, -0.5), Vec(Point(1,1.1,1.2)));


    std::cout << "-------Now----traversing----every----face--------" << std::endl;

    // only for timing
    // Get the starting timepoint
    auto start = std::chrono::high_resolution_clock::now();

    // Iterate over all rays
    for (int rayIndex=0; rayIndex < rays.size(); rayIndex++) {
        auto & ray = rays[rayIndex];
        // Iterate over all objects
        for (int objIndex = 0; objIndex < field.getObjects().size(); objIndex++) {
            auto &obj = field.getObjects()[objIndex];
            // Iterate over all faces of each object
            for (int faceIndex=0; faceIndex < obj.getFaces().size(); faceIndex++) {
                auto & face = obj.getFaces()[faceIndex];
                auto intersection = ray.mollerTrumboreIntersection(face);
                if (NO_INTERSECT != intersection) {
                    std::cout << "The ray " << rayIndex+1 << " intersects the face " << faceIndex+1 << " of object " << objIndex+1 << " at " << intersection << std::endl;
                } else {
                    //std::cout << "The ray " << rayIndex+1 << " does not intersect the face " << faceIndex+1 << " of object " << objIndex+1 << "." << std::endl;
                }
            }
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Time taken: " << duration << " seconds." << std::endl;


    std::cout << "-------Now----using----BVH----method----to-----accelerate--------" << std::endl;
    // generate an vector of all nodes' const shared pointers
    // so that I can conveiently traverse all nodes many times without visiting the multi-arch tree again and again
    // thanks to modern C++, RVO or NRVO is possible
    const std::vector<std::shared_ptr<Node>> node_ptrs = field.generateNodeList();

    // only for timing
    start = std::chrono::high_resolution_clock::now();

    // use BVH to accelerate the determination of whether the ray intersecting
    for (int rayIndex=0; rayIndex < rays.size(); rayIndex++) {
        auto & ray = rays[rayIndex];
        // Iterate over all nodes(boxes) and using BVH algorithm
        for (int nodeIndex = 0; nodeIndex < field.nodeCount; nodeIndex++) {
            auto & node = node_ptrs[nodeIndex];
            // Check if the ray intersects with the node (box)
            if (ray.intersectsWithBox(node->bbox)) {
                // If intersects, iterate over all faces in this bounding box, the faces may come from diffrent objects
                for (int faceIndex = 0; faceIndex < node->boxedFaces.size(); faceIndex++) {
                    auto & face = node->boxedFaces[faceIndex];
                    if (auto intersection = ray.mollerTrumboreIntersection(*face); NO_INTERSECT != intersection) {
                        std::cout << "The ray " << rayIndex+1 << " intersects the face #" << faceIndex+1 << " at " << intersection << std::endl;
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
    std::cout << "Time taken: " << duration << " seconds." << std::endl;

    return 0;
}