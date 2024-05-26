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
