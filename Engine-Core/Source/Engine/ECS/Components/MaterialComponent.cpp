#include "MaterialComponent.hpp"

MaterialComponent::MaterialComponent() : ComponentIdentifier(0)
{
}

MaterialComponent::MaterialComponent(
	const DirectX::XMFLOAT4& ambient,
	const DirectX::XMFLOAT4& diffuse,
	const DirectX::XMFLOAT4& specular,
	float shininess) : ComponentIdentifier(0)
{
	_materialConstantBuffer = MaterialConstantBuffer(ambient, diffuse, specular, shininess);
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

void MaterialComponent::SetAmbient(DirectX::XMFLOAT4 ambient)
{
	_materialConstantBuffer.ambient = ambient;
	_isDirty = true;
}

void MaterialComponent::SetDiffuse(DirectX::XMFLOAT4 diffuse)
{
	_materialConstantBuffer.diffuse = diffuse;
	_isDirty = true;
}

void MaterialComponent::SetSpecular(DirectX::XMFLOAT4 specular)
{
	_materialConstantBuffer.specular = specular;
	_isDirty = true;
}

void MaterialComponent::SetShininess(float shininess)
{
	_materialConstantBuffer.shininess = shininess;
	_isDirty = true;
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

bool MaterialComponent::IsDirty() const
{
	return _isDirty;
}

void MaterialComponent::SetIsDirty(bool isDirty)
{
	_isDirty = isDirty;
}

MaterialConstantBuffer MaterialComponent::GetMaterialConstantBuffer() const
{
	return _materialConstantBuffer;
}

MaterialComponent MaterialComponent::GetDefaultMaterialComponent(int id)
{
	DirectX::XMFLOAT4 ambient{ 1.0f, 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT4 diffuse{ 1.0f, 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT4 specular{ 1.0f, 1.0f, 1.0f, 1.0f };
	float shininess = 3.0f;
	auto material = MaterialComponent(id, ambient, diffuse, specular, shininess);
	return material;
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
