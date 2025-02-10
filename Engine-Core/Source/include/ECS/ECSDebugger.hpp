#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <string>
#include <cereal/cereal.hpp>
#include "ECS.hpp"

/* Debugger class for Core ECS that provides a UI
   to visually diagnose problems, should be completely disabled in release config
*/
class ECSDebugger : public System
{
    ECS* _ecs;
    bool _enabled = false;

public:
    ECSDebugger();
    ECSDebugger(ECS* ecs);
    ~ECSDebugger();

    void Update(float deltaTime) override;
    void PeriodicUpdate(float deltaTime) override;
    void Render() override;

    void Toggle();

    template<typename Archive>
    void serialize(Archive& archive)
    {

    }
};

#include <cereal/types/polymorphic.hpp>
CEREAL_REGISTER_TYPE(ECSDebugger)
CEREAL_REGISTER_POLYMORPHIC_RELATION(System, ECSDebugger)