#pragma once
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

#include "ECS.hpp";
#include "ProfilerComponent.hpp"

class ProfilerSystem : public ISystem
{
	ECS* _ecs = nullptr;
	bool _enabled = false;

public:
	ProfilerSystem();
	ProfilerSystem(ECS* ecs);
	~ProfilerSystem();

	void Render() override;
	void Update(float deltaTime) override;
	void PeriodicUpdate(float deltaTime) override;

	void Toggle();
};