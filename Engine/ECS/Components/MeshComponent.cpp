#include "MeshComponent.hpp"

MeshComponent::MeshComponent() : ComponentIdentifier(0)
{
}

MeshComponent::MeshComponent(
	int id,
	const std::vector<VertexPositionNormalUv>& vertices,
	const std::vector<UINT>& indices, int instancePoolIndex) : 
	_vertices(vertices), _indices(indices), _instancePoolIndex(instancePoolIndex), ComponentIdentifier(id)
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
