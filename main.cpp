// Using C++23 standard
//
// Created by root on 2024/3/13.
//

#include "main.h"

int main() {

    std::array<char[512], 4> filePaths = {
            R"(C:\Users\root\3D Objects\hot_desert_biome_obj\source\CalidiousDesert_obj.obj)",
            R"(C:\Users\root\3D Objects\snow_apls_low_poly_obj\source\Mesher.obj)",
            R"(C:\Users\root\3D Objects\mycube\mycube.obj)",
            R"(C:\Users\root\3D Objects\F22_blender\F22.obj)",
    };
    std::array<char[512], 4> mtlPaths = {
            R"(C:\Users\root\3D Objects\mycube\mycube.mtl)",
            R"(C:\Users\root\3D Objects\mycube\mycube.mtl)",
            R"(C:\Users\root\3D Objects\mycube\mycube.mtl)",
            R"(C:\Users\root\3D Objects\mycube\mycube.mtl)",
    };
    std::array<std::array<double, 3>, 4> scaleFactors = {{
            {200, 400, 2},
            {200, 400, 2},
            {2, 2, 0.5},
            {9.5, 9.5, 9.5},
    }};
    std::array<Point, 4> centers = {
            Point(-50, -50, 0),
            Point(25, 25, -0.1),
            Point(0, 0, 0.15),
            Point(-15, 2, 0.5),
    };

    std::vector<Item> objects = std::vector<Item>();
    for (int i = 1; i <= filePaths.size(); i++, std::cout<<std::endl) {
        objects.emplace_back();
        Item& object = objects.back();
        object.setScaleFactor(scaleFactors[i-1]);
        object.setCenter(centers[i-1]);

        if (!readNewItem(filePaths[i-1], object)) {
            std::cerr << "Error when doing readNewItem(#" << i << ") in main()" << std::endl;
            return -1;
        }

        if (!readNewItem(mtlPaths[i-1], object)) {
            std::cerr << "Error when doing readNewItem(#" << i << ") in main()" << std::endl;
            return -1;
        }

        std::cout<<"Object #"<< i <<" has "<<object.getFaces().size()<<" faces."<<std::endl;
        std::cout<<"Object #"<< i <<" has "<<object.getVertices().size()<<" vertices."<<std::endl;
        std::cout<<"Object #"<< i <<"'s refraction index is "<<object.Ni<<std::endl;
        std::cout<<"Object #"<< i <<"'s Ka is "<<object.Ka[0]<<" "<<object.Ka[1]<<" "<<object.Ka[2]<<std::endl;
        std::cout<<"Object #"<< i <<"'s Kd is "<<object.Kd[0]<<" "<<object.Kd[1]<<" "<<object.Kd[2]<<std::endl;
        std::cout<<"Object #"<< i <<"'s Ks is "<<object.Ks[0]<<" "<<object.Ks[1]<<" "<<object.Ks[2]<<std::endl;
    }

    std::cout<<objects.size()<<std::endl;
    std::vector<Ray> rays;
    rays.emplace_back(Point(20, 20, 20), Vec(Point(-0.01,-0.015,-1)));
    rays.emplace_back(Point(3, 3, 3), Vec(Point(-1,-1,-1)));
    rays.emplace_back(Point(-0.5, -0.5, -0.5), Vec(Point(1,1.1,1.2)));

    // Iterate over all rays
    for (int rayIndex=0; rayIndex < rays.size(); rayIndex++) {
        auto & ray = rays[rayIndex];
        // Iterate over all objects
        for (int objIndex=0; objIndex < objects.size(); objIndex++) {
            auto & obj = objects[objIndex];
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