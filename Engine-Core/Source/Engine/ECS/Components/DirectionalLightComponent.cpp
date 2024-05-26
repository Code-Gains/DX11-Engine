#include "DirectionalLightComponent.hpp"

DirectionalLightConstantBuffer::DirectionalLightConstantBuffer()
{
}

DirectionalLightConstantBuffer::DirectionalLightConstantBuffer(
	const DirectX::XMFLOAT4& direction,
	const DirectX::XMFLOAT4& ambient,
	const DirectX::XMFLOAT4& diffuse,
	const DirectX::XMFLOAT4& specular)
	: direction(direction), ambient(ambient), diffuse(diffuse), specular(specular)
{
}

DirectionalLightComponent::DirectionalLightComponent()
{
}

DirectionalLightComponent::DirectionalLightComponent(const DirectX::XMFLOAT4& direction, const DirectX::XMFLOAT4& ambient, const DirectX::XMFLOAT4& diffuse, const DirectX::XMFLOAT4& specular)
{
	_directionalLightConstantBuffer.direction = direction;
	_directionalLightConstantBuffer.ambient = ambient;
	_directionalLightConstantBuffer.diffuse = diffuse;
	_directionalLightConstantBuffer.specular = specular;
}

DirectX::XMFLOAT4 DirectionalLightComponent::GetDirection() const
{
	return _directionalLightConstantBuffer.direction;
}

DirectX::XMFLOAT4 DirectionalLightComponent::GetAmbient() const
{
	return _directionalLightConstantBuffer.ambient;
}

DirectX::XMFLOAT4 DirectionalLightComponent::GetDiffuse() const
{
	return _directionalLightConstantBuffer.diffuse;
}

DirectX::XMFLOAT4 DirectionalLightComponent::GetSpecular() const
{
	return _directionalLightConstantBuffer.specular;
}

DirectionalLightConstantBuffer DirectionalLightComponent::GetConstantBuffer()
{
	return _directionalLightConstantBuffer;
}

void DirectionalLightComponent::SetDirection(const DirectX::XMFLOAT4& direction)
{
	_directionalLightConstantBuffer.direction = direction;
}

void DirectionalLightComponent::SetAmbient(const DirectX::XMFLOAT4& ambient)
{
	_directionalLightConstantBuffer.ambient = ambient;
}

void DirectionalLightComponent::SetDiffuse(const DirectX::XMFLOAT4& diffuse)
{
	_directionalLightConstantBuffer.diffuse = diffuse;
}

void DirectionalLightComponent::SetSpecular(const DirectX::XMFLOAT4& specular)
{
	_directionalLightConstantBuffer.specular = specular;
}
