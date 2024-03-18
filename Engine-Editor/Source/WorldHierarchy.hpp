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

class World; // forward declaration

class WorldHierarchy
{
	World* _world = nullptr; // non owning
	std::unordered_map<Entity, std::string> _entityToName;

public:
	WorldHierarchy();
	WorldHierarchy(World* world);

	void Update(float deltaTime);
	void Render();

	void AddEntity(int entityId, std::string entityName);

	int CreatePrimitiveGeometry3D(PrimitiveGeometryType3D type, std::string name);

	void SetWorld(World* world);

	std::string GetEntityName(int entityId) const;

	void Clear();

	template <typename Archive>
	void serialize(Archive& archive)
	{
		archive(CEREAL_NVP(_entityToName));
	}
};

