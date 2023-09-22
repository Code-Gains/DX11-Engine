#pragma once
#include <DirectXMath.h>
#include <iostream>
#include "Component.hpp"

class TransformComponent : public Component
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 rotation;
	DirectX::XMFLOAT3 scale;

public:
	TransformComponent();

	TransformComponent(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& rot, const DirectX::XMFLOAT3& scale);

	DirectX::XMMATRIX GetWorldMatrix() const;

	friend std::ostream& operator<<(std::ostream& os, const TransformComponent& transform);
};

