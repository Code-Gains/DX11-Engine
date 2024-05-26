#pragma once
#include <DirectXMath.h>
#include <iostream>
#include <cereal/cereal.hpp>
#include "ComponentIdentifier.hpp"
#include "DirectXSerialization.hpp"
#include "Component.hpp"

class TransformComponent : public IComponent
{
	DirectX::XMFLOAT3 _position;
	DirectX::XMFLOAT3 _rotation;
	DirectX::XMFLOAT3 _scale;
	bool _isDirty = true; // TODO consider using flag systems (&)

public:
	TransformComponent();
	TransformComponent(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& rot, const DirectX::XMFLOAT3& scale);

	void SetPosition(const DirectX::XMFLOAT3& position);
	void SetRotation(const DirectX::XMFLOAT3& rotation);
	void SetScale(const DirectX::XMFLOAT3& scale);

	DirectX::XMFLOAT3 GetPosition() const;
	DirectX::XMFLOAT3 GetRotation() const;
	DirectX::XMFLOAT3 GetScale() const;

	//TODO CONSIDER USING REFS IS GOOD OR BAD
	DirectX::XMFLOAT3& GetPositionByRef();
	DirectX::XMFLOAT3& GetRotationByRef();
	DirectX::XMFLOAT3& GetScaleByRef();

	DirectX::XMMATRIX GetWorldMatrix() const;

	bool IsDirty() const;
	void SetIsDirty(bool isDirty);

	friend std::ostream& operator<<(std::ostream& os, const TransformComponent& transform);

	template <typename Archive>
	void serialize(Archive& archive)
	{
		archive(CEREAL_NVP(_isDirty), CEREAL_NVP(_position), CEREAL_NVP(_rotation), CEREAL_NVP(_scale));
	}
};

