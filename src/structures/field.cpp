//
// Created by root on 2024/3/16.
//

#include "field.h"

Field::Field() noexcept
    : bounds({BigO, BigO}), tree(std::make_unique<Box>()) {

}

Field::Field(Point min, Point max) noexcept
    : bounds({min, max}), tree(std::make_unique<Box>()) {

}

void Field::initPfaces(const std::vector<Triangle*> &p_faces_t) {
    p_faces = p_faces_t;
}



void Field::buildBVHTree() {
    // this "tree" should be the root node
    //initPfaces(p_faces);
    tree->build(objects);
    tree->split();
}

bool Field::insertObject(const std::string& objPath, const std::string& mtlPath, const std::array<double, 3>& scaleFactor, Point center) {
    objects.emplace_back();
    Item& object = objects.back();
    object.setScaleFactor(scaleFactor);
    object.setCenter(center);
    if (!readNewItem(objPath.c_str(), object)) {
        std::cerr << "Error when doing readNewItem(#" << objects.size() << ") in Field::insertObject(" << objPath << ")" << std::endl;
        return false;
    }

    if (!readNewItem(mtlPath.c_str(), object)) {
        std::cerr << "Error when doing readNewItem(#" << objects.size() << ") in Field::insertObject(" << mtlPath << ")" << std::endl;
        return false;
    }

    std::cout<<"Object #"<<objects.size()<<" has "<<object.getFaces().size()<<" faces."<<std::endl;
    std::cout<<"Object #"<<objects.size()<<" has "<<object.getVertices().size()<<" vertices."<<std::endl;
    std::cout<<"Object #"<<objects.size()<<"'s refraction index is "<<object.Ni<<std::endl;
    std::cout<<"Object #"<<objects.size()<<"'s Ka is "<<object.Ka[0]<<" "<<object.Ka[1]<<" "<<object.Ka[2]<<std::endl;
    std::cout<<"Object #"<<objects.size()<<"'s Kd is "<<object.Kd[0]<<" "<<object.Kd[1]<<" "<<object.Kd[2]<<std::endl;
    std::cout<<"Object #"<<objects.size()<<"'s Ks is "<<object.Ks[0]<<" "<<object.Ks[1]<<" "<<object.Ks[2]<<std::endl;
    std::cout<<std::endl;

    return true;
}

std::vector<Item>& Field::getObjects() {
    return objects;
}