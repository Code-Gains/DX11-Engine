#include "TerrainComponent.hpp"


TerrainComponent::TerrainComponent()
{
}

TerrainComponent::TerrainComponent(const Heightmap& heightmap, MeshComponent* mesh)
	: _terrainChunkMesh(mesh)
{

	// non ECS mesh does not need identification or world references
	// mesh.SetId(geometry.GetId());
	//_world->AddComponent(geometry.GetId(), mesh);
}
