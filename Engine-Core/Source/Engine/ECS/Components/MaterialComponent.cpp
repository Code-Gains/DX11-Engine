#include "MaterialComponent.hpp"

MaterialComponent::MaterialComponent()
{
}

MaterialComponent::MaterialComponent(
	const DirectX::XMFLOAT4& ambient,
	const DirectX::XMFLOAT4& diffuse,
	const DirectX::XMFLOAT4& specular,
	float shininess)
{
	_materialConstantBuffer = MaterialConstantBuffer(ambient, diffuse, specular, shininess);
	ComponentRegistry::RegisterComponentUI<MaterialComponent, MaterialComponentUI>();
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

DirectX::XMFLOAT4& MaterialComponent::GetAmbientByRef()
{
	return _materialConstantBuffer.ambient;
}

DirectX::XMFLOAT4& MaterialComponent::GetDiffuseByRef()
{
	return _materialConstantBuffer.diffuse;
}

DirectX::XMFLOAT4& MaterialComponent::GetSpecularByRef()
{
	return _materialConstantBuffer.specular;
}

float& MaterialComponent::GetShininessByRef()
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

MaterialComponent MaterialComponent::GetDefaultMaterialComponent()
{
	DirectX::XMFLOAT4 ambient{ 1.0f, 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT4 diffuse{ 1.0f, 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT4 specular{ 1.0f, 1.0f, 1.0f, 1.0f };
	float shininess = 15.0f;
	auto material = MaterialComponent(ambient, diffuse, specular, shininess);
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

void MaterialComponentUI::RenderUI(Component& component)
{
	// Cast to actual component type
	auto& material = static_cast<MaterialComponent&>(component);

	auto& selectedAmbient = material.GetAmbientByRef();
	auto& selectedDiffuse = material.GetDiffuseByRef();
	auto& selectedSpecular = material.GetSpecularByRef();
	auto& selectedShininess = material.GetShininessByRef();

	if (ImGui::DragFloat3("Ambient", &selectedAmbient.x, 0.01, 0.0f, 1.0f))
	{
		selectedAmbient.x = std::clamp(selectedAmbient.x, 0.0f, 1.0f);
		selectedAmbient.y = std::clamp(selectedAmbient.y, 0.0f, 1.0f);
		selectedAmbient.z = std::clamp(selectedAmbient.z, 0.0f, 1.0f);
		material.SetAmbient(selectedAmbient);
	}
	if (ImGui::DragFloat3("Diffuse", &selectedDiffuse.x, 0.01, 0.0f, 1.0f))
	{
		selectedDiffuse.x = std::clamp(selectedDiffuse.x, 0.0f, 1.0f);
		selectedDiffuse.y = std::clamp(selectedDiffuse.y, 0.0f, 1.0f);
		selectedDiffuse.z = std::clamp(selectedDiffuse.z, 0.0f, 1.0f);
		material.SetDiffuse(selectedDiffuse);
	}
	if (ImGui::DragFloat3("Specular", &selectedSpecular.x, 0.01, 0.0f, 1.0f))
	{
		selectedSpecular.x = std::clamp(selectedSpecular.x, 0.0f, 1.0f);
		selectedSpecular.y = std::clamp(selectedSpecular.y, 0.0f, 1.0f);
		selectedSpecular.z = std::clamp(selectedSpecular.z, 0.0f, 1.0f);
		material.SetSpecular(selectedSpecular);
	}
	if (ImGui::DragFloat("Shininess", &selectedShininess, 0.1f, 0.0f, 100.0f))
	{
		selectedShininess = std::clamp(selectedShininess, 0.0f, 1.0f);
		material.SetShininess(selectedShininess);
	}
}
