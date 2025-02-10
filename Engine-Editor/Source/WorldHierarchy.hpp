#pragma once
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

#include <unordered_map>
#include <memory>
#include <string>
#include <iostream>

#include <cereal/cereal.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/string.hpp>

#include "TransformComponent.hpp"
#include "TerrainComponent.hpp"
#include "WorldHierarchyComponent.hpp"

#include "ECS.hpp"
//#include "World.hpp"

class World; // forward declaration

class WorldHierarchy : public System
{
	ECS* _ecs;
	World* _world = nullptr; // non owning

	bool _enabled = false;

	Entity _selectedEntity;

public:
	WorldHierarchy();
	WorldHierarchy(ECS* ecs, World* world);

	void Render() override;
	void Update(float deltaTime) override;
	void PeriodicUpdate(float deltaTime) override;

	Entity GetSelectedEntity();

	int CreatePrimitiveGeometry3D(PrimitiveGeometryType3D type, std::string name, std::unordered_map<Entity, std::string>& entityToName);

	void SetWorld(World* world);

	void Toggle();

	template<typename Archive>
	void serialize(Archive& archive)
	{

	}
};

#include <cereal/types/polymorphic.hpp>
CEREAL_REGISTER_TYPE(WorldHierarchy)
CEREAL_REGISTER_POLYMORPHIC_RELATION(System, WorldHierarchy)

