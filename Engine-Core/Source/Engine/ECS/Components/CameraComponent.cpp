#include "CameraComponent.hpp"

CameraComponent::CameraComponent()
{
}


CameraComponent::CameraComponent(ID3D11Device* device, DirectX::XMFLOAT3 rotation, float moveSpeed, float rotationSpeed)
	: _rotation(rotation), _moveSpeed(moveSpeed), _rotationSpeed(rotationSpeed)
{
    CreateConstantBuffers(device);
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

WRL::ComPtr<ID3D11Buffer> CameraComponent::GetCameraConstantBuffer()
{
	return _cameraConstantBuffer;
}

void CameraComponent::CreateConstantBuffers(ID3D11Device* device)
{
    D3D11_BUFFER_DESC desc{};
    desc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    desc.ByteWidth = sizeof(CameraConstantBuffer);
    device->CreateBuffer(&desc, nullptr, &_cameraConstantBuffer);

}

void CameraComponent::RenderMonitorUI() const
{

}
