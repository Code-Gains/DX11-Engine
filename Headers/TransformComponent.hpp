#pragma once
#include <DirectXMath.h>
#include <iostream>
#include "ComponentIdentifier.hpp"

class TransformComponent : public ComponentIdentifier
{
	DirectX::XMFLOAT3 _position;
	DirectX::XMFLOAT3 _rotation;
	DirectX::XMFLOAT3 _scale;
	bool _isDirty = true; // TODO consider using flag systems (&)

public:
	TransformComponent();
	TransformComponent(int id);
	TransformComponent(int id, const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& rot, const DirectX::XMFLOAT3& scale);

	void SetPosition(const DirectX::XMFLOAT3& position);
	void SetRotation(const DirectX::XMFLOAT3& rotation);
	void SetScale(const DirectX::XMFLOAT3& scale);

	DirectX::XMFLOAT3 GetPosition() const;
	DirectX::XMFLOAT3 GetRotation() const;
	DirectX::XMFLOAT3 GetScale() const;
	DirectX::XMMATRIX GetWorldMatrix() const;

	bool IsDirty() const;
	void SetIsDirty(bool isDirty);

	friend std::ostream& operator<<(std::ostream& os, const TransformComponent& transform);
};

