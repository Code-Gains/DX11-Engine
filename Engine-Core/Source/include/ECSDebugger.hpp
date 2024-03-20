#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

#include "ECS.hpp"

/* Debugger class for Core ECS that provides a UI
   to visually diagnose problems, should be completely disabled in release config
*/
class ECSDebugger : public ISystem
{
    ECS* _ecs;


public:
    ECSDebugger();
    ECSDebugger(ECS* ecs);
    ~ECSDebugger();

    void Update(float deltaTime) override;
    void PeriodicUpdate(float deltaTime) override;
    void Render() override;
};

