#pragma once
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

#include <unordered_map>
#include <memory>
#include <string>
#include <iostream>

#include "TransformComponent.hpp"

class World; // forward declaration

class WorldHierarchy
{
	World* _world = nullptr; // non owning
	std::unordered_map<int, std::string> _entityToName;
public:
	WorldHierarchy();
	WorldHierarchy(World* world);

	void Update(float deltaTime);
	void Render();

	void AddEntity(int entityId, std::string entityName);
	void LinkEntities(std::vector<Entity> entities);

	int CreatePrimitiveGeometry3D(PrimitiveGeometryType3D type, std::string name);

	std::string GetEntityName(int entityId) const;
};

