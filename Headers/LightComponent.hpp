#pragma once
#include <DirectXMath.h>

#include "Component.hpp"

class LightComponent : public Component
{
    DirectX::XMFLOAT4 _position;
    DirectX::XMFLOAT4 _ambient;
    DirectX::XMFLOAT4 _diffuse;
    DirectX::XMFLOAT4 _specular;

public:
    LightComponent();
    LightComponent(const DirectX::XMFLOAT4& position, const DirectX::XMFLOAT4& ambient, const DirectX::XMFLOAT4& diffuse, const DirectX::XMFLOAT4& specular);

    DirectX::XMFLOAT4 GetPosition() const;
    DirectX::XMFLOAT4 GetAmbient() const;
    DirectX::XMFLOAT4 GetDiffuse() const;
    DirectX::XMFLOAT4 GetSpecular() const;
};

