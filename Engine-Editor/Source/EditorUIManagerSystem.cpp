#include "EditorUIManagerSystem.hpp"

EditorUIManagerSystem::EditorUIManagerSystem()
{
}

EditorUIManagerSystem::EditorUIManagerSystem(ECS* ecs) : _ecs(ecs)
{
}


void EditorUIManagerSystem::Update(float deltaTime)
{
}

void EditorUIManagerSystem::Render()
{

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View"))
        {
            // On click get status from the global system registry and toggle enabled bool
            ImGui::MenuItem("World Hierarchy", nullptr, &_showWorldHierarchy);
            if (ImGui::MenuItem("ECS Debugger", nullptr, &_showEcsDebugger))
            {
                auto ecsDebugger = _ecs->GetSystem<ECSDebugger>();
                ecsDebugger->Toggle();
            }
            if (ImGui::MenuItem("EMS", nullptr, &_showEMS))
            {
                auto ems = _ecs->GetSystem<EntityMonitoringSystem>();
                ems->Toggle();
            }
            if (ImGui::MenuItem("Profiler", nullptr, &_showProfiler))
            {
                auto profilerSystem = _ecs->GetSystem<ProfilerSystem>();
                profilerSystem->Toggle();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Settings"))
        {
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void EditorUIManagerSystem::PeriodicUpdate(float deltaTime)
{
}
