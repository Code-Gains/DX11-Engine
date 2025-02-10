#pragma once

#include "RenderingApplication3D.hpp"
#include "World.hpp"
#include <memory>
#include <string>
#include <algorithm>
#include "ConstantBufferBinder.hpp"
#include "EditorUIManagerSystem.hpp"
#include "EntityMonitoringSystem.hpp"
#include "ProfilerSystem.hpp"
#include "WorldHierarchy.hpp"

class Editor
{
public:
	Editor();
	void Run();

	void LoadSystems(ECS* ecs, World& world, HWND hwnd, HANDLE handle);
};

