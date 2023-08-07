#pragma once
#include <d3d11_2.h>
#include <DirectXMath.h>

struct PhongMaterial
{
    DirectX::XMFLOAT4 ambient;
    DirectX::XMFLOAT4 diffuse;
    DirectX::XMFLOAT4 specular;
    float shininess;
    float padding[3];

    PhongMaterial();
};

struct MaterialConstantBuffer : PhongMaterial
{};
