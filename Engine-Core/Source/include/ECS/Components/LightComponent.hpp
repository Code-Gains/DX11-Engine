#pragma once
#include <DirectXMath.h>
#include "ComponentIdentifier.hpp"

class LightComponent : public ComponentIdentifier
{
    DirectX::XMFLOAT4 _position;
    DirectX::XMFLOAT4 _ambient;
    DirectX::XMFLOAT4 _diffuse;
    DirectX::XMFLOAT4 _specular;

public:
    LightComponent();
    LightComponent(int id, const DirectX::XMFLOAT4& position, const DirectX::XMFLOAT4& ambient, const DirectX::XMFLOAT4& diffuse, const DirectX::XMFLOAT4& specular);

    DirectX::XMFLOAT4 GetPosition() const;
    DirectX::XMFLOAT4 GetAmbient() const;
    DirectX::XMFLOAT4 GetDiffuse() const;
    DirectX::XMFLOAT4 GetSpecular() const;

    // Serialization

    template<typename Archive>
    void save(Archive& archive) const
    {
    }

    template<typename Archive>
    void load(Archive& archive)
    {
    }
};

