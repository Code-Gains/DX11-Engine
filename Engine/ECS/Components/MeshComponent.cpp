#include "MeshComponent.hpp"

MeshComponent::MeshComponent()
{
}

MeshComponent::MeshComponent(
	const std::vector<DirectX::XMFLOAT3>& vertices,
	const std::vector<UINT>& indices) : _vertices(vertices), _indices(indices)
{
}

std::vector<DirectX::XMFLOAT3> MeshComponent::GetVertices() const
{
	return _vertices;
}

std::vector<UINT> MeshComponent::GetIndices() const
{
	return _indices;
}
