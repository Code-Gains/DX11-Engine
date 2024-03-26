#pragma once
#include <vector>
#include <DirectXMath.h>
#include <cereal/cereal.hpp>

#include "Definitions.hpp"
#include "VertexType.hpp"
#include "ComponentIdentifier.hpp"
#include "Component.hpp"
#include <Heightmap.hpp>


enum class PrimitiveGeometryType3D
{
	Cube,
	Sphere,
	Cylinder,
	Pipe,
	TerrainChunk // TODO REMOVE FROM HERE ONLY FOR TESTING
};

class MeshComponent : public IComponent
{
	std::vector<VertexPositionNormalUv> _vertices;
	std::vector<UINT> _indices;
	int _instancePoolIndex;
	// Engine models use prefix Engine:: ?? maybe, I don't know yet what the json will look like
	std::string _path;
	bool _isDirty = true; // TODO find a common system for dirtyness
public:
	MeshComponent();
	MeshComponent(const std::vector<VertexPositionNormalUv>& vertices, const std::vector<UINT>& indices);
	MeshComponent(const std::vector<VertexPositionNormalUv>& vertices, const std::vector<UINT>& indices, int _instancePoolIndex);


	void SetVertices(const std::vector<VertexPositionNormalUv>& vertices);
	void SetIndices(const std::vector<UINT>& indices);
	void SetInstancePoolIndex(int id);
	void SetPath(std::string path);

	std::vector<VertexPositionNormalUv> GetVertices() const;
	std::vector<UINT> GetIndices() const;
	int GetInstancePoolIndex() const;

	bool IsDirty() const;
	void SetIsDirty(bool isDirty);

	static MeshComponent GeneratePrimitiveMeshComponent(PrimitiveGeometryType3D type);
	static std::vector<VertexPositionNormalUv> GetPrimitiveMeshVertices(PrimitiveGeometryType3D type);
	static std::vector<UINT> GetPrimitiveMeshIndices(PrimitiveGeometryType3D type);

	static MeshComponent GenerateTerrainMeshComponent(PrimitiveGeometryType3D type, const Heightmap* heightmap = nullptr);
	static std::vector<VertexPositionNormalUv> GetTerrainMeshVertices(const Heightmap* heightmap);
	static std::vector<UINT> GetTerrainMeshIndices();


	static std::vector<VertexPositionNormalUv> GetPrimitiveCubeVertices();
	static std::vector<VertexPositionNormalUv> GetPrimitiveSphereVertices(float radius, int numSlices, int numStacks);
	static std::vector<VertexPositionNormalUv> GetPrimitiveCylinderVertices(float radius, float height, int numSlices);
	static std::vector<VertexPositionNormalUv> GetPrimitivePipeVertices(float radius, float height, int numSlices);
	static std::vector<VertexPositionNormalUv> GetPrimitiveTerrainChunkVertices(float width, float height, int densityX, int densityY, const Heightmap* heightmap);

	static std::vector<UINT> GetPrimitiveCubeIndices();
	static std::vector<UINT> GetPrimitiveSphereIndices(int numSlices, int numStacks);
	static std::vector<UINT> GetPrimitiveCylinderIndices(int numSlices);
	static std::vector<UINT> GetPrimitivePipeIndices(int numSlices);
	static std::vector<UINT> GetPrimitiveTerrainChunkIndices(float width, float height, int densityX, int densityY);

	bool FinalizeLoading();


	template<typename Archive>
	void save(Archive& archive) const
	{
		archive(CEREAL_NVP(_instancePoolIndex), CEREAL_NVP(_path));
	}

	template<typename Archive>
	void load(Archive& archive)
	{
		archive(CEREAL_NVP(_instancePoolIndex), CEREAL_NVP(_path));
		FinalizeLoading();
	}
};

