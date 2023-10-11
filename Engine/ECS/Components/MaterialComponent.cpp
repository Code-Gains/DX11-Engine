#include "MaterialComponent.hpp"

MaterialComponent::MaterialComponent() : ComponentIdentifier(0)
{
}

MaterialComponent::MaterialComponent(
	int id,
	const DirectX::XMFLOAT4& ambient,
	const DirectX::XMFLOAT4& diffuse,
	const DirectX::XMFLOAT4& specular,
	float shininess) : ComponentIdentifier(id)
{
	_materialConstantBuffer = MaterialConstantBuffer(ambient, diffuse, specular, shininess);
}

DirectX::XMFLOAT4 MaterialComponent::GetAmbient() const
{
	return _materialConstantBuffer.ambient;
}

DirectX::XMFLOAT4 MaterialComponent::GetDiffuse() const
{
	return _materialConstantBuffer.diffuse;
}

DirectX::XMFLOAT4 MaterialComponent::GetSpecular() const
{
	return _materialConstantBuffer.specular;
}

float MaterialComponent::GetShininess() const
{
	return _materialConstantBuffer.shininess;
}

MaterialConstantBuffer MaterialComponent::GetMaterialConstantBuffer() const
{
	return _materialConstantBuffer;
}

MaterialConstantBuffer::MaterialConstantBuffer()
{
}

MaterialConstantBuffer::MaterialConstantBuffer(
	const DirectX::XMFLOAT4& ambient,
	const DirectX::XMFLOAT4& diffuse,
	const DirectX::XMFLOAT4& specular,
	float shininess) : ambient(ambient), diffuse(diffuse), specular(specular), shininess(shininess)
{
}
