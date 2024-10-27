#pragma once
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include "ECS.hpp"

#include "ProfilerSystem.hpp"
#include "ECSDebugger.hpp"
#include "EntityMonitoringSystem.hpp"
// TODO Load windows from editor files (windows should not be hardcoded in code at some point likely)


class EditorUIManagerSystem : public ISystem
{
	ECS* _ecs = nullptr;
	bool _showWorldHierarchy = false;
	bool _showEcsDebugger = false;
	bool _showEMS = false;
	bool _showProfiler = false;

public:
	EditorUIManagerSystem();
	EditorUIManagerSystem(ECS* ecs);

	void Render() override;
	void Update(float deltaTime) override;
	void PeriodicUpdate(float deltaTime) override;
};