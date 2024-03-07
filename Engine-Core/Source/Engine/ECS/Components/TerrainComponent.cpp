#include "TerrainComponent.hpp"

TerrainComponent::TerrainComponent() : ComponentIdentifier(0)
{
}

TerrainComponent::TerrainComponent(int id) : ComponentIdentifier(id)
{
}

TerrainComponent::TerrainComponent(int id, const Heightmap& heightmap, MeshComponent* mesh)
	: ComponentIdentifier(id), _terrainChunkMesh(mesh)
{

	// non ECS mesh does not need identification or world references
	// mesh.SetId(geometry.GetId());
	//_world->AddComponent(geometry.GetId(), mesh);
}
