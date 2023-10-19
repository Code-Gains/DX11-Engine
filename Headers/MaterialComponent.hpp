#pragma once
#include <DirectXMath.h>
#include "ComponentIdentifier.hpp"


struct MaterialConstantBuffer
{
    DirectX::XMFLOAT4 ambient;
    DirectX::XMFLOAT4 diffuse;
    DirectX::XMFLOAT4 specular;
    float shininess;
    float padding[3] = { 0, 0, 0 };

    MaterialConstantBuffer();
    MaterialConstantBuffer(const DirectX::XMFLOAT4& ambient, const DirectX::XMFLOAT4& diffuse, const DirectX::XMFLOAT4& specular, float shininess);
};

class MaterialComponent : public ComponentIdentifier
{
    MaterialConstantBuffer _materialConstantBuffer;
public:
    MaterialComponent();
	MaterialComponent(const DirectX::XMFLOAT4& ambient, const DirectX::XMFLOAT4& diffuse, const DirectX::XMFLOAT4& specular, float shininess);
	MaterialComponent(int id, const DirectX::XMFLOAT4& ambient, const DirectX::XMFLOAT4& diffuse, const DirectX::XMFLOAT4& specular, float shininess);

    DirectX::XMFLOAT4 GetAmbient() const;
    DirectX::XMFLOAT4 GetDiffuse() const;
    DirectX::XMFLOAT4 GetSpecular() const;
    float GetShininess() const;

    MaterialConstantBuffer GetMaterialConstantBuffer() const;

    static MaterialComponent GetDefaultMaterialComponent(int id);
};

