#pragma once
#include <vector>
#include <DirectXMath.h>

#include "Definitions.hpp"
#include "VertexType.hpp"
#include "Component.hpp"

class MeshComponent : public Component
{
	std::vector<VertexPositionNormalUv> _vertices;
	std::vector<UINT> _indices;
public:
	MeshComponent();
	MeshComponent(const std::vector<VertexPositionNormalUv>& vertices, const std::vector<UINT>& indices);

	std::vector<VertexPositionNormalUv> GetVertices() const;
	std::vector<UINT> GetIndices() const;
};

