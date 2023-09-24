#pragma once
#include <DirectXMath.h>
#include "Component.hpp"

class MaterialComponent : public Component
{
    DirectX::XMFLOAT4 _ambient;
    DirectX::XMFLOAT4 _diffuse;
    DirectX::XMFLOAT4 _specular;
    float _shininess;
    float _padding[3];

    // Maps when needed

public:
    MaterialComponent();
	MaterialComponent(const DirectX::XMFLOAT4& ambient, const DirectX::XMFLOAT4& diffuse, const DirectX::XMFLOAT4& specular, float shininess);

    DirectX::XMFLOAT4 GetAmbient() const;
    DirectX::XMFLOAT4 GetDiffuse() const;
    DirectX::XMFLOAT4 GetSpecular() const;
    float GetShininess() const;
};

