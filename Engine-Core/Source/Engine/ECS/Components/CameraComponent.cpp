#include "CameraComponent.hpp"

CameraComponent::CameraComponent()
{
}


CameraComponent::CameraComponent(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, float moveSpeed, float rotationSpeed)
	: _position(position), _rotation(rotation), _moveSpeed(moveSpeed), _rotationSpeed(rotationSpeed)
{
}
CameraComponent::~CameraComponent()
{
}

DirectX::XMFLOAT3 CameraComponent::GetPosition() const
{
	return _position;
}

DirectX::XMFLOAT3 CameraComponent::GetRotation() const
{
	return _rotation;
}

float CameraComponent::GetMoveSpeed() const
{
	return _moveSpeed;
}

float CameraComponent::GetRotationSpeed() const
{
	return _rotationSpeed;
}

void CameraComponent::SetPosition(const DirectX::XMFLOAT3& position)
{
	_position = position;
}

void CameraComponent::SetRotation(const DirectX::XMFLOAT3& rotation)
{
	_rotation = rotation;
}
