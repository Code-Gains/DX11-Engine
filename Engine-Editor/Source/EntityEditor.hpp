#pragma once
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

#include "ECS.hpp"
#include "WorldHierarchy.hpp"

//#include "ProfilerSystem.hpp"
//#include "ECSDebugger.hpp"
//#include "EntityMonitoringSystem.hpp"
//#include "WorldHierarchy.hpp"

//#include "RenderingApplication3D.hpp"
// TODO Load windows from editor files (windows should not be hardcoded in code at some point likely)


class EntityEditor : public System
{
	ECS* _ecs = nullptr;
	bool _enabled = false;
	Entity _selected;

public:
	EntityEditor();
	EntityEditor(ECS* ecs);

	void Render() override;
	void Update(float deltaTime) override;
	void PeriodicUpdate(float deltaTime) override;

	void Toggle();
};