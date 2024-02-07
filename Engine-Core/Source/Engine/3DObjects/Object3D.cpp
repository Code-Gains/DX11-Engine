#include "Object3D.hpp"

//std::vector<Object3D*> Object3D::GetAllObjects()
//{
//    return { this };
//}
//
//std::vector<Object3D*> CompositeObject3D::GetAllObjects() {
//    std::vector<Object3D*> allObjects = { this };
//
//    for (auto& child : _children) {
//        auto childObjects = child->GetAllObjects();
//        allObjects.insert(allObjects.end(), childObjects.begin(), childObjects.end());
//    }
//
//    return allObjects;
//}