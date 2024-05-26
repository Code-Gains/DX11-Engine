#pragma once
#include "CameraComponent.hpp"
#include "ECS.hpp";
#include "Constants.hpp"
#include "RenderingApplication3D.hpp"

class CameraSystem : public ISystem
{
	RenderingApplication3D* _renderingApplication = nullptr;
	ECS* _ecs = nullptr;
public:
	CameraSystem();
	CameraSystem(RenderingApplication3D* renderingApplication, ECS* ecs);
	~CameraSystem();

	void Render() override;
	void Update(float deltaTime) override;
	void PeriodicUpdate(float deltaTime) override;
};