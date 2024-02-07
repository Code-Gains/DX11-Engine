#include "LightComponent.hpp"

LightComponent::LightComponent() : ComponentIdentifier(0)
{
}

LightComponent::LightComponent(
	int id,
	const DirectX::XMFLOAT4& position,
	const DirectX::XMFLOAT4& ambient,
	const DirectX::XMFLOAT4& diffuse,
	const DirectX::XMFLOAT4& specular) : _position(position), _ambient(ambient), _diffuse(diffuse), _specular(specular), ComponentIdentifier(id)
{
}

DirectX::XMFLOAT4 LightComponent::GetPosition() const
{
	return _position;
}


DirectX::XMFLOAT4 LightComponent::GetAmbient() const
{
	return _ambient;
}

DirectX::XMFLOAT4 LightComponent::GetDiffuse() const
{
	return _diffuse;
}

DirectX::XMFLOAT4 LightComponent::GetSpecular() const
{
	return _specular;
}

