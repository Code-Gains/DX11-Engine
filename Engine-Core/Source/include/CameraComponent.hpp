#pragma once
#include <d3d11_2.h>
#include <DirectXMath.h>

#include "Component.hpp"
#include "ComponentIdentifier.hpp"
#include "ConstantBufferDefinitions.hpp"
#include "Definitions.hpp"

// Debug tools
#include "IMonitorable.hpp"


class CameraComponent : public IComponent, public IMonitorable
{
    DirectX::XMFLOAT3 _rotation;

    float _moveSpeed;
    float _rotationSpeed;

    WRL::ComPtr<ID3D11Buffer> _cameraConstantBuffer = nullptr;

    // TODO ADD FOV AND OTHER STUFF LIKE RENDER MASKS, MODES, FLAGS

public:
    CameraComponent();
    CameraComponent(ID3D11Device* device, DirectX::XMFLOAT3 rotation, float moveSpeed, float rotationSpeed);
    ~CameraComponent();

    DirectX::XMFLOAT3& GetRotationByRef();
    float GetMoveSpeed() const;
    float GetRotationSpeed() const;

    WRL::ComPtr<ID3D11Buffer> GetCameraConstantBuffer();
    void CreateConstantBuffers(ID3D11Device* device);

    // Debug tools
    void RenderMonitorUI() const override;
};