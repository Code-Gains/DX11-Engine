#include "ProfilerSystem.hpp"
#include <Psapi.h>

ProfilerSystem::ProfilerSystem()
{
}

ProfilerSystem::ProfilerSystem(ECS* ecs) : _ecs(ecs)
{
}

ProfilerSystem::~ProfilerSystem()
{
}

// TODO Consider combining update and render loops for some editor systems
void ProfilerSystem::Update(float deltaTime)
{
    if (!_enabled)
        return;


    // querry returns a vector of tuples that contain component vectors
    auto componentQueryResult = _ecs->QueryComponentVectors<ProfilerComponent>();
    // iterate
    for (auto& tuple : componentQueryResult)
    {
        auto& profilers = std::get<0>(tuple);

        // We want to work with the std::vector
        auto& rawProfilers = *profilers->GetRawVector();

        for (auto& entityToComponent : profilers->GetEntityToIndex())
        {
            auto id = entityToComponent.first;
            auto idx = entityToComponent.second;

            auto& profiler = rawProfilers[idx];

            profiler.IncrementFramesPassed();
            profiler.AddTotalDeltaTime(deltaTime);
            profiler.SubtractTimeLeftUntilDebugInfo(deltaTime);

            profiler.SetFrameDeltas(deltaTime);
            profiler.IncrementFrameIndex();
            auto timeLeftUntilDebugInfo = profiler.GetTimeUntilDebugInfo();

            if (timeLeftUntilDebugInfo <= 0)
            {
                // we have sum delta and everything else is internal
                profiler.UpdateFramerate();
                profiler.UpdateAverageFramerate();
                PROCESS_MEMORY_COUNTERS_EX pmc;
                if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc)))
                {
                    profiler.SetVirtualMemoryUsage(pmc.PrivateUsage);
                }
                profiler.UpdateCPUValue();
                profiler.SetTimeUntilDebugInfo(1.0f);
            }
        }
    }
}

void ProfilerSystem::PeriodicUpdate(float deltaTime)
{
}

void ProfilerSystem::Render()
{
    if (!_enabled)
        return;

    // querry returns a vector of tuples that contain component vectors
    auto componentQueryResult = _ecs->QueryComponentVectors<ProfilerComponent>();
    // iterate
    for (auto& tuple : componentQueryResult)
    {
        auto& profilers = std::get<0>(tuple);

        // We want to work with the std::vector
        auto& rawProfilers = *profilers->GetRawVector();

        for (auto& entityToComponent : profilers->GetEntityToIndex())
        {
            auto id = entityToComponent.first;
            auto idx = entityToComponent.second;

            auto& profiler = rawProfilers[idx];
            int passed = profiler.GetFramesPassed();
            ImGui::Begin("Profiler");
            ImGui::Text("FPS [Last 100]: %.2f", profiler.GetFramerate());
            ImGui::Text("AVG FPS: %.2f", profiler.GetAverageFramerate());
            ImGui::Text("RAM: %.2f MB", profiler.GetVirtualMemoryUsage() / (1024.0f * 1024.0f));
            double cpuUsage = profiler.GetCPUUsage();
            ImGui::Text("CPU Usage: %.4f%%", profiler.GetCPUUsage());
            ImGui::End();
        }
    }
}

void ProfilerSystem::Toggle()
{
    _enabled = !_enabled;
}