// Using C++23 standard
//
// Created by root on 2024/3/13.
//

#include "main.h"

int main() {

    char filePath[512] = R"(C:\Users\root\CLionProjects\vertices\models\mycube.obj)";
    char mtlPath[512] = R"(C:\Users\root\CLionProjects\vertices\models\mycube.mtl)";

    std::vector<Item> objects = std::vector<Item>();
    objects.emplace_back();
    Item& object = objects.back();

    if (!readNewItem(filePath, object)) {
        std::cerr << "Error when doing readNewItem(filePath) in main()" << std::endl;
        return -1;
    }

    if (!readNewItem(mtlPath, object)) {
        std::cerr << "Error when doing readNewItem(mtlPath) in main()" << std::endl;
        return -1;
    }

    std::cout<<"Object has "<<object.getFaces().size()<<" faces."<<std::endl;
    std::cout<<"Object has "<<object.getVertices().size()<<" vertices."<<std::endl;
    std::cout<<"Object's refraction index is "<<object.Ni<<std::endl;
    std::cout<<"Object's Ka is "<<object.Ka[0]<<" "<<object.Ka[1]<<" "<<object.Ka[2]<<std::endl;
    std::cout<<"Object's Kd is "<<object.Kd[0]<<" "<<object.Kd[1]<<" "<<object.Kd[2]<<std::endl;
    std::cout<<"Object's Ks is "<<object.Ks[0]<<" "<<object.Ks[1]<<" "<<object.Ks[2]<<std::endl;

    std::vector<Ray> rays;
    rays.emplace_back(Point(2, 2, 2), Vec(Point(0,0,-1)));
    rays.emplace_back(Point(3, 3, 3), Vec(Point(-1,-1,-1)));

    // Iterate over all rays
    for (int rayIndex=0; rayIndex < rays.size(); rayIndex++) {
        auto & ray = rays[rayIndex];
        // Iterate over all objects
        for (int objIndex=0; objIndex < objects.size(); objIndex++) {
            auto & obj = objects[objIndex];
            // Iterate over all faces of each object
            for (int faceIndex=0; faceIndex < obj.getFaces().size(); faceIndex++) {
                auto & face = obj.getFaces()[faceIndex];
                if (NO_INTERSECT != ray.mollerTrumboreIntersection(face)) {
                    std::cout << "The ray " << rayIndex+1 << " intersects the face " << faceIndex+1 << " of object " << objIndex+1 << " at " << ray.mollerTrumboreIntersection(face) << std::endl;
                } else {
                    std::cout << "The ray " << rayIndex+1 << " does not intersect the face " << faceIndex+1 << " of object " << objIndex+1 << "." << std::endl;
                }
            }
        }
    }

    return 0;
}