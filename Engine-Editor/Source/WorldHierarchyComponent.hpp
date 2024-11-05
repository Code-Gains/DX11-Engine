#pragma once
#include "MeshComponent.hpp"

class WorldHierarchyComponent : public IComponent
{
	std::unordered_map<Entity, std::string> _entityToName;

public:

	WorldHierarchyComponent();
	int CreatePrimitiveGeometry3D(PrimitiveGeometryType3D type, std::string name);


	std::unordered_map<Entity, std::string>& GetEntityToNameRef();
	const std::unordered_map<Entity, std::string>& GetEntityToNameConstRef() const;
	std::string GetEntityName(int entityId) const;

	void Clear();
};