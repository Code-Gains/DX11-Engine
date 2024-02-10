#pragma once
#include <vector>
#include <DirectXMath.h>
#include <cereal/cereal.hpp>

#include "Definitions.hpp"
#include "VertexType.hpp"
#include "ComponentIdentifier.hpp"


enum class PrimitiveGeometryType3D
{
	Cube,
	Sphere,
	Cylinder,
	Pipe,
	TerrainChunk
};

class MeshComponent : public ComponentIdentifier
{
	std::vector<VertexPositionNormalUv> _vertices;
	std::vector<UINT> _indices;
	int _instancePoolIndex;
	// Engine models use prefix Engine::
	std::string _path;
public:
	MeshComponent();
	MeshComponent(const std::vector<VertexPositionNormalUv>& vertices, const std::vector<UINT>& indices);
	MeshComponent(int id, const std::vector<VertexPositionNormalUv>& vertices, const std::vector<UINT>& indices);
	MeshComponent(int id, const std::vector<VertexPositionNormalUv>& vertices, const std::vector<UINT>& indices, int _instancePoolIndex);


	void SetVertices(const std::vector<VertexPositionNormalUv>& vertices);
	void SetIndices(const std::vector<UINT>& indices);
	void SetComponentIdentifier(int id);
	void SetInstancePoolIndex(int id);
	void SetPath(std::string path);

	std::vector<VertexPositionNormalUv> GetVertices() const;
	std::vector<UINT> GetIndices() const;
	int GetInstancePoolIndex() const;

	static MeshComponent GeneratePrimitiveMeshComponent(PrimitiveGeometryType3D type);
	static std::vector<VertexPositionNormalUv> GetVertices(PrimitiveGeometryType3D type);
	static std::vector<UINT> GetIndices(PrimitiveGeometryType3D type);

	static std::vector<VertexPositionNormalUv> GetPrimitiveCubeVertices();
	static std::vector<VertexPositionNormalUv> GetPrimitiveSphereVertices(float radius, int numSlices, int numStacks);
	static std::vector<VertexPositionNormalUv> GetPrimitiveCylinderVertices(float radius, float height, int numSlices);
	static std::vector<VertexPositionNormalUv> GetPrimitivePipeVertices(float radius, float height, int numSlices);
	static std::vector<VertexPositionNormalUv> GetPrimitiveTerrainChunkVertices(float width, float height, int densityX, int densityY);

	static std::vector<UINT> GetPrimitiveCubeIndices();
	static std::vector<UINT> GetPrimitiveSphereIndices(int numSlices, int numStacks);
	static std::vector<UINT> GetPrimitiveCylinderIndices(int numSlices);
	static std::vector<UINT> GetPrimitivePipeIndices(int numSlices);
	static std::vector<UINT> GetPrimitiveTerrainChunkIndices(float width, float height, int densityX, int densityY);

	bool FinalizeLoading();


	template<typename Archive>
	void save(Archive& archive) const
	{
		archive(cereal::make_nvp("_id", GetId()), CEREAL_NVP(_instancePoolIndex), CEREAL_NVP(_path));
	}

	template<typename Archive>
	void load(Archive& archive)
	{
		archive(cereal::make_nvp("_id", GetId()), CEREAL_NVP(_instancePoolIndex), CEREAL_NVP(_path));
		FinalizeLoading();
	}
};

