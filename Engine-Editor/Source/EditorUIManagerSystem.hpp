#pragma once
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include "ECS.hpp"

#include "ProfilerSystem.hpp"
#include "ECSDebugger.hpp"
#include "EntityMonitoringSystem.hpp"
#include "WorldHierarchy.hpp"
#include "EntityEditor.hpp"

#include "RenderingApplication3D.hpp"
// TODO Load windows from editor files (windows should not be hardcoded in code at some point likely)


class EditorUIManagerSystem : public System
{
	ECS* _ecs = nullptr;
	bool _showWorldHierarchy = false;
	bool _showEcsDebugger = false;
	bool _showEMS = false;
	bool _showProfiler = false;
	bool _showEntityEditor = false;

	bool _isDraggingWindow = false;
	double _dragStartMouseX = 0.0, _dragStartMouseY = 0.0;
	int _dragStartWindowX = 0, _dragStartWindowY = 0;
	double _dragOffsetX = 0.0, _dragOffsetY = 0.0;

public:
	EditorUIManagerSystem();
	EditorUIManagerSystem(ECS* ecs);

	void Render() override;
	void Update(float deltaTime) override;
	void PeriodicUpdate(float deltaTime) override;

	void RenderMenuBar();
	void RenderMenuBarMenuButtons();
	void RenderMenuBarControlButtons(int controlButtonStackWidth);
	void RenderMenuBarInvisibleDragButton(int controlButtonStackWidth);
	void RenderWindowInvisibleResizePanels();

	bool Splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2);

	template<typename Archive>
	void serialize(Archive& archive)
	{

	}
};

#include <cereal/types/polymorphic.hpp>
CEREAL_REGISTER_TYPE(EditorUIManagerSystem)
CEREAL_REGISTER_POLYMORPHIC_RELATION(System, EditorUIManagerSystem)