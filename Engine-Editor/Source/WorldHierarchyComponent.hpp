#pragma once
#include "MeshComponent.hpp"
#include <cereal/cereal.hpp>

class WorldHierarchyComponent : public Component
{
	std::unordered_map<Entity, std::string> _entityToName;

public:

	WorldHierarchyComponent();
	int CreatePrimitiveGeometry3D(PrimitiveGeometryType3D type, std::string name);


	std::unordered_map<Entity, std::string>& GetEntityToNameRef();
	const std::unordered_map<Entity, std::string>& GetEntityToNameConstRef() const;
	std::string GetEntityName(int entityId) const;

	void Clear();

	template<typename Archive>
	void save(Archive& archive) const
	{
	}

	template<typename Archive>
	void load(Archive& archive)
	{
	}
};