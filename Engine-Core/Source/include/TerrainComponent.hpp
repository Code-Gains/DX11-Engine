#pragma once
#pragma once
#include <vector>
#include <memory>
#include <DirectXMath.h>
#include <cereal/cereal.hpp>

#include "Definitions.hpp"
#include "VertexType.hpp"
#include "ComponentIdentifier.hpp"
#include "Heightmap.hpp"
#include "Terrain.hpp"
#include <MeshComponent.hpp>

class TerrainComponent : public IComponent
{
	float _chunkWidth;
	float _chunkHeight;
	int _chunkDensityX;
	int _chunkDdensityY;
	
	// testing component relations
	MeshComponent* _terrainChunkMesh;

public:
	TerrainComponent();
	TerrainComponent(const Heightmap& heightmap, MeshComponent* mesh);

	template<typename Archive>
	void save(Archive& archive) const
	{
		//archive(cereal::make_nvp("_id", GetId()), CEREAL_NVP(_instancePoolIndex), CEREAL_NVP(_path));
	}

	template<typename Archive>
	void load(Archive& archive)
	{
		//archive(cereal::make_nvp("_id", GetId()), CEREAL_NVP(_instancePoolIndex), CEREAL_NVP(_path));
		//FinalizeLoading();
	}
};

