#pragma once

#include "Component.hpp"
#include <cereal/types/polymorphic.hpp>

#include "CameraComponent.hpp"
#include "DirectionalLightComponent.hpp"
#include "LightComponent.hpp"
#include "MaterialComponent.hpp"
#include "MeshComponent.hpp"
#include "ProfilerComponent.hpp"
#include "TerrainComponent.hpp"
#include "TransformComponent.hpp"

#include "VertexType.hpp"

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