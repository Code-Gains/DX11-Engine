#pragma once
#include <d3d11_2.h>
#include <DirectXMath.h>


struct PerFrameConstantBuffer
{
    DirectX::XMFLOAT4X4 viewProjectionMatrix;
};

struct PerObjectConstantBuffer
{
    DirectX::XMFLOAT4X4 modelMatrix;
    DirectX::XMFLOAT4X4 normalMatrix;
};

struct CameraConstantBuffer
{
    DirectX::XMFLOAT3 cameraPosition;
    float padding;
};

struct LightConstantBuffer
{
    DirectX::XMFLOAT4 Position;
    DirectX::XMFLOAT4 Ambient;
    DirectX::XMFLOAT4 Diffuse;
    DirectX::XMFLOAT4 Specular;
};