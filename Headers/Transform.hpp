#pragma once
#include <DirectXMath.h>
#include <iostream>

class Transform
{
public:
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 rotation;
	DirectX::XMFLOAT3 scale;

	Transform();

	Transform(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& rot, const DirectX::XMFLOAT3& scale);

	DirectX::XMMATRIX GetWorldMatrix() const;

	friend std::ostream& operator<<(std::ostream& os, const Transform& transform);

};

