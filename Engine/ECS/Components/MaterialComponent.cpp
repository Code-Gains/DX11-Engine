#include "MaterialComponent.hpp"

MaterialComponent::MaterialComponent()
{
}

MaterialComponent::MaterialComponent(
	const DirectX::XMFLOAT4& ambient,
	const DirectX::XMFLOAT4& diffuse,
	const DirectX::XMFLOAT4& specular,
	float shininess) : _ambient(ambient), _diffuse(diffuse), _specular(specular), _shininess(shininess)
{
}

DirectX::XMFLOAT4 MaterialComponent::GetAmbient() const
{
	return _ambient;
}

DirectX::XMFLOAT4 MaterialComponent::GetDiffuse() const
{
	return _diffuse;
}

DirectX::XMFLOAT4 MaterialComponent::GetSpecular() const
{
	return _specular;
}

float MaterialComponent::GetShininess() const
{
	return _shininess;
}
