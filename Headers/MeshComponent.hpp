#pragma once
#include <vector>
#include <DirectXMath.h>

#include "Component.hpp"
#include "Definitions.hpp"


class MeshComponent : public Component
{
	std::vector<DirectX::XMFLOAT3> _vertices;
	std::vector<UINT> _indices;
public:
	MeshComponent();
	MeshComponent(const std::vector<DirectX::XMFLOAT3>& vertices, const std::vector<UINT>& indices);

	std::vector<DirectX::XMFLOAT3> GetVertices() const;
	std::vector<UINT> GetIndices() const;
};

