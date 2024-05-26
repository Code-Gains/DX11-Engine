#pragma once
//#include <d3d11_2.h>
#include <DirectXMath.h>

#include "Component.hpp"
#include "ComponentIdentifier.hpp"


class CameraComponent : public IComponent
{
    DirectX::XMFLOAT3 _position;
    DirectX::XMFLOAT3 _rotation;

    float _moveSpeed;
    float _rotationSpeed;

public:
    CameraComponent();
    CameraComponent(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, float moveSpeed, float rotationSpeed);
    ~CameraComponent();
};

