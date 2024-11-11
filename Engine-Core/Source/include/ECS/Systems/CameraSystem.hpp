#pragma once
#include "CameraComponent.hpp"
#include "ECS.hpp";
#include "Constants.hpp"
#include "RenderingApplication3D.hpp"
#include "ConstantBufferBinder.hpp"

class CameraSystem : public System
{
	RenderingApplication3D* _renderingApplication = nullptr;
	ECS* _ecs = nullptr;

	void BindCameraConstantBuffer(const WRL::ComPtr<ID3D11Buffer>& cameraConstantBuffer, const DirectX::XMFLOAT3& cameraPosition) const;
public:
	CameraSystem();
	CameraSystem(RenderingApplication3D* renderingApplication, ECS* ecs);
	~CameraSystem();

	void Render() override;
	void Update(float deltaTime) override;
	void PeriodicUpdate(float deltaTime) override;
};