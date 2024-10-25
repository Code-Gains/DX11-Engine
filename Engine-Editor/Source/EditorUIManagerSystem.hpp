#pragma once
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include "ECS.hpp"

#include "ProfilerSystem.hpp"
// TODO Load windows from editor files (windows should not be hardcoded in code at some point likely)


class EditorUIManagerSystem : public ISystem
{
	ECS* _ecs = nullptr;
	bool _showWorldHierarchy = true;
	bool _showEcsDebugger = true;
	bool _showEMS = true;
	bool _showProfiler = true;

public:
	EditorUIManagerSystem();
	EditorUIManagerSystem(ECS* ecs);

	void Render() override;
	void Update(float deltaTime) override;
	void PeriodicUpdate(float deltaTime) override;
};