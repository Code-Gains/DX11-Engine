#include "CameraComponent.hpp"

CameraComponent::CameraComponent()
{
}


CameraComponent::CameraComponent(DirectX::XMFLOAT3 rotation, float moveSpeed, float rotationSpeed)
	: _rotation(rotation), _moveSpeed(moveSpeed), _rotationSpeed(rotationSpeed)
{
}
CameraComponent::~CameraComponent()
{
}

DirectX::XMFLOAT3& CameraComponent::GetRotationByRef()
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

void CameraComponent::RenderMonitorUI() const
{

}
