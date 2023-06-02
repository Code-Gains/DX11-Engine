#pragma once
#include <Transform.hpp>
#include <d3d11_2.h>

class Object3D
{
public:
    Transform transform;

    virtual ~Object3D() {}

    virtual bool Initialize(ID3D11Device* device) = 0;

    virtual void Render(ID3D11DeviceContext* deviceContext) = 0;
};