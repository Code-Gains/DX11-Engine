#include "MeshComponent.hpp"

MeshComponent::MeshComponent()
{
}

MeshComponent::MeshComponent(
	const std::vector<VertexPositionNormalUv>& vertices,
	const std::vector<UINT>& indices, int instancePoolIndex) : 
	_vertices(vertices), _indices(indices), _instancePoolIndex(instancePoolIndex)
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

int MeshComponent::GetInstancePoolIndex() const
{
	return _instancePoolIndex;
}
