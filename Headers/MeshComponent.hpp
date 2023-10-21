#pragma once
#include <vector>
#include <DirectXMath.h>
#include <cereal/cereal.hpp>

#include "Definitions.hpp"
#include "VertexType.hpp"
#include "ComponentIdentifier.hpp"

#include "Cube.hpp"
#include "Sphere.hpp"
#include "Cylinder.hpp"


enum class PrimitiveGeometryType3D
{
	Cube,
	Sphere,
	Cylinder,
	Pipe
};

class MeshComponent : public ComponentIdentifier
{
	std::vector<VertexPositionNormalUv> _vertices;
	std::vector<UINT> _indices;
	int _instancePoolIndex;
	int _meshType;
public:
	MeshComponent();
	MeshComponent(const std::vector<VertexPositionNormalUv>& vertices, const std::vector<UINT>& indices);
	MeshComponent(int id, const std::vector<VertexPositionNormalUv>& vertices, const std::vector<UINT>& indices);
	MeshComponent(int id, const std::vector<VertexPositionNormalUv>& vertices, const std::vector<UINT>& indices, int _instancePoolIndex);

	void SetComponentIdentifier(int id);
	void SetInstancePoolIndex(int id);

	std::vector<VertexPositionNormalUv> GetVertices() const;
	std::vector<UINT> GetIndices() const;
	int GetInstancePoolIndex() const;

	static MeshComponent GetPrimitiveMeshComponent(PrimitiveGeometryType3D type);
	static MeshComponent GetPrimitiveMeshComponent(int id, PrimitiveGeometryType3D type);

	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::make_nvp("_id", GetId()), CEREAL_NVP(_vertices), CEREAL_NVP(_indices), CEREAL_NVP(_instancePoolIndex), CEREAL_NVP(_meshType));
	}
};

