#include "Component.hpp"

#include <iostream>
#include <memory>
#include <unordered_map>
#include <string>
#include <fstream>

// 1) Cereal headers
#include <cereal/types/memory.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/archives/json.hpp>

#include "IComponentVector.hpp"
#include "ComponentVector.hpp"

#include "CameraComponent.hpp"
#include "DirectionalLightComponent.hpp"
#include "LightComponent.hpp"
#include "MaterialComponent.hpp"
#include "MeshComponent.hpp"
#include "ProfilerComponent.hpp"
#include "TerrainComponent.hpp"
#include "TransformComponent.hpp"

#include "VertexType.hpp" //

#include <iostream>
//#include <cereal/types/base_class.hpp>
//#include <cereal/types/polymorphic.hpp>
//#ifndef CEREAL_REGISTER_ABSTRACT
//// A simple approach is to treat "register abstract" the same
//// as "register type with name."
//#define CEREAL_REGISTER_ABSTRACT(T) \
//    CEREAL_REGISTER_TYPE_WITH_NAME(T, #T)
//#endif
//
////CEREAL_REGISTER_ABSTRACT(IComponentVector)
//CEREAL_REGISTER_ABSTRACT(IComponentVector);

//CEREAL_REGISTER_TYPE(Component)
//CEREAL_REGISTER_TYPE(IComponentVector)
//
//CEREAL_REGISTER_TYPE(CameraComponent);
//CEREAL_REGISTER_TYPE(DirectionalLightComponent);
//CEREAL_REGISTER_TYPE(LightComponent);
//CEREAL_REGISTER_TYPE(MaterialComponent);
//CEREAL_REGISTER_TYPE(MeshComponent<VertexPositionNormalUv>);
//CEREAL_REGISTER_TYPE(ProfilerComponent);
//CEREAL_REGISTER_TYPE(TerrainComponent);
//CEREAL_REGISTER_TYPE(TransformComponent);

//CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, CameraComponent);
//CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, DirectionalLightComponent);
//CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, LightComponent);
//CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, MaterialComponent);
//CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, MeshComponent<VertexPositionNormalUv>);
//CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, ProfilerComponent);
//CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, TerrainComponent);
//CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, TransformComponent);

#define CEREAL_REGISTER_ABSTRACT(T) \
  CEREAL_REGISTER_TYPE_WITH_NAME(T, #T)
CEREAL_REGISTER_ABSTRACT(IComponentVector);

CEREAL_REGISTER_TYPE(ComponentVector<CameraComponent>);
CEREAL_REGISTER_TYPE(ComponentVector<DirectionalLightComponent>);
CEREAL_REGISTER_TYPE(ComponentVector<LightComponent>);
CEREAL_REGISTER_TYPE(ComponentVector<MaterialComponent>);
CEREAL_REGISTER_TYPE(ComponentVector<MeshComponent<VertexPositionNormalUv>>);
CEREAL_REGISTER_TYPE(ComponentVector<ProfilerComponent>);
CEREAL_REGISTER_TYPE(ComponentVector<TerrainComponent>);
CEREAL_REGISTER_TYPE(ComponentVector<TransformComponent>);

CEREAL_REGISTER_POLYMORPHIC_RELATION(IComponentVector, ComponentVector<CameraComponent>)
CEREAL_REGISTER_POLYMORPHIC_RELATION(IComponentVector, ComponentVector<DirectionalLightComponent>)
CEREAL_REGISTER_POLYMORPHIC_RELATION(IComponentVector, ComponentVector<LightComponent>)
CEREAL_REGISTER_POLYMORPHIC_RELATION(IComponentVector, ComponentVector<MaterialComponent>)
CEREAL_REGISTER_POLYMORPHIC_RELATION(IComponentVector, ComponentVector<MeshComponent<VertexPositionNormalUv>>)
CEREAL_REGISTER_POLYMORPHIC_RELATION(IComponentVector, ComponentVector<ProfilerComponent>)
CEREAL_REGISTER_POLYMORPHIC_RELATION(IComponentVector, ComponentVector<TerrainComponent>)
CEREAL_REGISTER_POLYMORPHIC_RELATION(IComponentVector, ComponentVector<TransformComponent>)


void LogCerealRegistration()
{
    std::cout << "[Debug] Cereal registrations loaded in SerializationRegistration.cpp 2\n";
}

// -- Force the function to run at program startup:
//static int s_dummyVar = (LogCerealRegistration(), 0);