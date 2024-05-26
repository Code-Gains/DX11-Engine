#pragma once
//#include <d3d11_2.h>
#include <DirectXMath.h>

#include "Component.hpp"
#include "ComponentIdentifier.hpp"


class CameraComponent : public IComponent
{
    DirectX::XMFLOAT3 _rotation;

    float _moveSpeed;
    float _rotationSpeed;

    // TODO ADD FOV AND OTHER STUFF LIKE RENDER MASKS, MODES, FLAGS

public:
    CameraComponent();
    CameraComponent(DirectX::XMFLOAT3 rotation, float moveSpeed, float rotationSpeed);
    ~CameraComponent();

    //DirectX::XMFLOAT3& GetPositionByRef();
    DirectX::XMFLOAT3& GetRotationByRef();
    float GetMoveSpeed() const;
    float GetRotationSpeed() const;

    /*void SetPosition(const DirectX::XMFLOAT3& position);
    void SetRotation(const DirectX::XMFLOAT3& rotation);*/
};

