#pragma once
//#include <imgui.h>
//#include <imgui_impl_glfw.h>
//#include <imgui_impl_win32.h>
//#include <imgui_impl_dx11.h>
//#include <Psapi.h>
#include <pdh.h>

#include "Component.hpp"


class ProfilerComponent : public Component
{
    HWND _win32Window;
    HANDLE _self;

    ULARGE_INTEGER _lastCPU, _lastSysCPU, _lastUserCPU;
    double _cpuUsage = 0;
    int _numProcessors;

    int _frameBufferSize = 100;
    float _frameDeltas[100] = {0};
    long int _frameIndex = 0;
    float _framerate = 0;
    int _framesPassed = 0;
    float _averageFramerate = 0;

    float _totalDeltaTime = 0;
    float _timeLeftUntilDebugInfo = 1;

    SIZE_T _virtualMemoryUsage = 0;

public:
    ProfilerComponent();

    void Initialize(HWND hwnd, HANDLE handle);

    // Setters and Getters
    int GetFramesPassed();

    float GetFramerate() const;
    void SetFramerate(float framerate);

    float GetAverageFramerate() const;
    void SetAverageFramerate(float averageFramerate);

    SIZE_T GetVirtualMemoryUsage() const;
    void SetVirtualMemoryUsage(SIZE_T virtualMemoryUsage);

    double GetCPUUsage() const;
    void SetCPUUsage(double cpuUsage);

    const int GetFrameBufferSize() const;

    float GetTimeUntilDebugInfo() const;
    void SetTimeUntilDebugInfo(float timeLeft);

    // Calculations
    float GetSumDeltaTimes() const;
    void UpdateFramerate();
    void UpdateAverageFramerate();
    void UpdateCPUValue();

    // Increments and Additions
    void IncrementFramesPassed();
    void AddTotalDeltaTime(float deltaTime);
    void AddTimeLeftUntilDebugInfo(float deltaTime);
    void SubtractTimeLeftUntilDebugInfo(float deltaTime);
    void SetFrameDeltas(float deltaTime);
    void IncrementFrameIndex();

    // Serialization

    template<typename Archive>
    void save(Archive& archive) const
    {
    }

    template<typename Archive>
    void load(Archive& archive)
    {
    }
};