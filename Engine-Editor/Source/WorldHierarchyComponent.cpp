#include "WorldHierarchyComponent.hpp"

WorldHierarchyComponent::WorldHierarchyComponent()
{
}

int WorldHierarchyComponent::CreatePrimitiveGeometry3D(PrimitiveGeometryType3D type, std::string name)
{
	//auto geometry = _world->CreateEntity();
	//auto transform = TransformComponent();
	//_world->AddComponent(geometry, transform);

	////temporary to check system TODO SEPARATE

	//if (type == PrimitiveGeometryType3D::TerrainChunk)
	//{
	//	std::vector<std::vector<float>> heights;
	//	heights.resize(256);
	//	for (unsigned int i = 0; i < 256; ++i) {
	//		heights[i].resize(256, 0);
	//	}
	//	Heightmap heightmap = Heightmap(heights);
	//	auto mesh = MeshComponent<VertexPositionNormalUvHeight>::GenerateTerrainMeshComponent(type, heightmap);
	//	auto terrain = TerrainComponent(heightmap, &mesh);
	//	_world->AddComponent(geometry, mesh);
	//	_world->AddComponent(geometry, terrain);
	//}
	//else
	//{
	//	auto mesh = MeshComponent<VertexPositionNormalUv>::GeneratePrimitiveMeshComponent(type);
	//	_world->AddComponent(geometry, mesh);
	//}

	//auto material = MaterialComponent::GetDefaultMaterialComponent();
	//_world->AddComponent(geometry, material);

	//_entityToName[geometry] = name;

	//return geometry;
	return 0;
}

std::unordered_map<Entity, std::string>& WorldHierarchyComponent::GetEntityToNameRef()
{
	return _entityToName;
}

const std::unordered_map<Entity, std::string>& WorldHierarchyComponent::GetEntityToNameConstRef() const
{
	return _entityToName;
}

std::string WorldHierarchyComponent::GetEntityName(int entityId) const
{
    return std::string();
}

void WorldHierarchyComponent::Clear()
{
	_entityToName.clear();
}
