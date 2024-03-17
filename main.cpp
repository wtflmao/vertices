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

    return 0;
}