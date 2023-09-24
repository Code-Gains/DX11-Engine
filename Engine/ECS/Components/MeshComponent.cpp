#include "MeshComponent.hpp"

MeshComponent::MeshComponent()
{
}

MeshComponent::MeshComponent(
	const std::vector<VertexPositionNormalUv>& vertices,
	const std::vector<UINT>& indices) : _vertices(vertices), _indices(indices)
{
}

std::vector<VertexPositionNormalUv> MeshComponent::GetVertices() const
{
	return _vertices;
}

std::vector<UINT> MeshComponent::GetIndices() const
{
	return _indices;
}
