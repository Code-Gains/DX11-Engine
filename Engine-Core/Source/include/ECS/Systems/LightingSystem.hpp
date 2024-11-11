#pragma once
#include <d3d11_2.h>

#include "ECS.hpp";
#include "DirectionalLightComponent.hpp"
#include "ConfigManager.hpp"
#include "ConstantBufferBinder.hpp"

enum ConstantBuffers
{
	PerFrame,
	Camera,
	DirectionalLight,
	PerObject
};

class LightingSystem : public System
{
	WRL::ComPtr<ID3D11Device> _device = nullptr;
	WRL::ComPtr<ID3D11DeviceContext> _deviceContext = nullptr;
	ECS* _ecs;

	WRL::ComPtr<ID3D11Buffer> _directionalLightConstantBuffer = nullptr;
public:
	LightingSystem();
	LightingSystem(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ECS* ecs);

	void InitializeBuffers();

	void Render() override;
	void Update(float deltaTime) override;
	void PeriodicUpdate(float deltaTime) override;

	void UpdateDirectionalLightBuffer(const DirectionalLightConstantBuffer& directionalLightBuffer);

};
