#pragma once
#include <vector>
#include <DirectXMath.h>

#include "Definitions.hpp"
#include "VertexType.hpp"
#include "Component.hpp"

class MeshComponent
{
	std::vector<VertexPositionNormalUv> _vertices;
	std::vector<UINT> _indices;
	int _instancePoolIndex;
public:
	MeshComponent();
	MeshComponent(const std::vector<VertexPositionNormalUv>& vertices, const std::vector<UINT>& indices, int _instancePoolIndex);

	std::vector<VertexPositionNormalUv> GetVertices() const;
	std::vector<UINT> GetIndices() const;
	int GetInstancePoolIndex() const;
};

