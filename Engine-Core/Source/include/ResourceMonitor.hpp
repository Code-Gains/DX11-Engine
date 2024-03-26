#pragma once
//#include <Psapi.h>
//#include <pdh.h>
//#include <winnt.h>
//#include <imgui.h>
//#include <windows.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

#include <Psapi.h>
#include <pdh.h>


class ResourceMonitor
{
    HWND _win32Window;
    HANDLE _self;

	ULARGE_INTEGER _lastCPU, _lastSysCPU, _lastUserCPU;
    double _cpuUsage = 0;
	int _numProcessors;

    const int _frameBufferSize = 100;
    float _frameDeltas[100] = { 0 };
    long int _frameIndex = 0;
    float _framerate = 0;
    int _framesPassed = 0;
    float _averageFramerate = 0;

    float _totalDeltaTime = 0;
    float _timeLeftUntilDebugInfo = 1;

    SIZE_T _virtualMemoryUsage = 0;

public:
    ResourceMonitor() {};

    void Initialize(HWND hwnd, HANDLE handle)
    {
        _win32Window = hwnd;
        _self = handle;

        SYSTEM_INFO sysInfo;
        FILETIME ftime, fsys, fuser;

        GetSystemInfo(&sysInfo);
        _numProcessors = sysInfo.dwNumberOfProcessors;

        GetSystemTimeAsFileTime(&ftime);
        memcpy(&_lastCPU, &ftime, sizeof(FILETIME));

        GetProcessTimes(_self, &ftime, &ftime, &fsys, &fuser);
        memcpy(&_lastSysCPU, &fsys, sizeof(FILETIME));
        memcpy(&_lastUserCPU, &fuser, sizeof(FILETIME));
    }

    void Update(float deltaTime)
    {
        _framesPassed++;
        _totalDeltaTime += deltaTime;
        _timeLeftUntilDebugInfo -= deltaTime;

        _frameDeltas[_frameIndex] = deltaTime;
        _frameIndex = (_frameIndex + 1) % _frameBufferSize;

        if (_timeLeftUntilDebugInfo <= 0)
        {
            float sumDeltaTimes = 0;
            for (int i = 0; i < _frameBufferSize; ++i) {
                sumDeltaTimes += _frameDeltas[i];
            }
            _framerate = _frameBufferSize / sumDeltaTimes;
            _averageFramerate = _framesPassed / _totalDeltaTime;
            PROCESS_MEMORY_COUNTERS_EX pmc;
            if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc)))
            {
                _virtualMemoryUsage = pmc.PrivateUsage;
            }
            _cpuUsage = GetAndUpdateCpuValue();
            _timeLeftUntilDebugInfo = 1.0f;
        }
    }

    void Render() const
    {
        ImGui::Begin("Profiler");
        ImGui::Text("FPS [Last 100]: %.2f", _framerate);
        ImGui::Text("AVG FPS: %.2f", _averageFramerate);
        ImGui::Text("RAM: %.2f MB", _virtualMemoryUsage / (1024.0f * 1024.0f));
        if (_cpuUsage >= 0.0f)
            ImGui::Text("CPU Usage: %.2f%%", _cpuUsage);
        else
        {
            ImGui::Text("CPU Usage: N/A");
        }
        //ImGui::Text("Geometry instances: %d", _scene.GetOwnershipCount() + _instanceRenderer.GetOwnershipCount());
    }

    double GetAndUpdateCpuValue() {
        FILETIME ftime, fsys, fuser;
        ULARGE_INTEGER now, sys, user;
        double percent;

        GetSystemTimeAsFileTime(&ftime);
        memcpy(&now, &ftime, sizeof(FILETIME));

        GetProcessTimes(_self, &ftime, &ftime, &fsys, &fuser);
        memcpy(&sys, &fsys, sizeof(FILETIME));
        memcpy(&user, &fuser, sizeof(FILETIME));
        percent = (sys.QuadPart - _lastSysCPU.QuadPart) +
            (user.QuadPart - _lastUserCPU.QuadPart);
        percent /= (now.QuadPart - _lastCPU.QuadPart);
        percent /= _numProcessors;
        _lastCPU = now;
        _lastUserCPU = user;
        _lastSysCPU = sys;

        return percent * 100;
    }
};

