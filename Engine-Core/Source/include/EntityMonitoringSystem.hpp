#pragma once
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

#include "ECS.hpp";
#include "Constants.hpp"
#include "RenderingApplication3D.hpp"

class EntityMonitoringSystem : public ISystem
{
	ECS* _ecs = nullptr;
public:
	EntityMonitoringSystem();
	EntityMonitoringSystem(ECS* ecs);
	~EntityMonitoringSystem();

	void Render() override;
	void Update(float deltaTime) override;
	void PeriodicUpdate(float deltaTime) override;
};