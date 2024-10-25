#include "ProfilerComponent.hpp"

ProfilerComponent::ProfilerComponent()
    : _win32Window(nullptr), _self(nullptr) {}
    //, _cpuUsage(0), _numProcessors(0),
    //_frameIndex(0), _framerate(0), _framesPassed(0), _averageFramerate(0),
    //_totalDeltaTime(0), _timeLeftUntilDebugInfo(1), _virtualMemoryUsage(0) {}

void ProfilerComponent::Initialize(HWND hwnd, HANDLE handle)
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

// Getters and Setters
float ProfilerComponent::GetFramerate() const
{
    return _framerate;
}

int ProfilerComponent::GetFramesPassed()
{
    return _framesPassed;
}

void ProfilerComponent::SetFramerate(float framerate)
{
    _framerate = framerate;
}

float ProfilerComponent::GetAverageFramerate() const
{
    return _averageFramerate;
}

void ProfilerComponent::SetAverageFramerate(float averageFramerate)
{
    _averageFramerate = averageFramerate;
}

SIZE_T ProfilerComponent::GetVirtualMemoryUsage() const
{
    return _virtualMemoryUsage;
}

void ProfilerComponent::SetVirtualMemoryUsage(SIZE_T virtualMemoryUsage)
{
    _virtualMemoryUsage = virtualMemoryUsage;
}

double ProfilerComponent::GetCPUUsage() const
{
    return _cpuUsage;
}

void ProfilerComponent::SetCPUUsage(double cpuUsage)
{
    _cpuUsage = cpuUsage;
}

const int ProfilerComponent::GetFrameBufferSize() const
{
    return _frameBufferSize;
}

float ProfilerComponent::GetSumDeltaTimes() const
{
    float sumDeltaTimes = 0;
    for (int i = 0; i < _frameBufferSize; ++i) {
        sumDeltaTimes += _frameDeltas[i];
    }
    return sumDeltaTimes;
}

void ProfilerComponent::UpdateFramerate()
{
    _framerate = _frameBufferSize / GetSumDeltaTimes();
}

void ProfilerComponent::UpdateAverageFramerate()
{
    _averageFramerate = _framesPassed / _totalDeltaTime;
}

void ProfilerComponent::UpdateCPUValue()
{
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
    _cpuUsage = percent * 100;
}


float ProfilerComponent::GetTimeUntilDebugInfo() const
{
    return _timeLeftUntilDebugInfo;
}

void ProfilerComponent::SetTimeUntilDebugInfo(float timeLeft)
{
    _timeLeftUntilDebugInfo = timeLeft;
}

void ProfilerComponent::IncrementFramesPassed()
{
    _framesPassed++;
}

void ProfilerComponent::AddTotalDeltaTime(float deltaTime)
{
    _totalDeltaTime += deltaTime;
}

void ProfilerComponent::AddTimeLeftUntilDebugInfo(float deltaTime)
{
    _timeLeftUntilDebugInfo += deltaTime;
}

void ProfilerComponent::SubtractTimeLeftUntilDebugInfo(float deltaTime)
{
    _timeLeftUntilDebugInfo -= deltaTime;
}

void ProfilerComponent::SetFrameDeltas(float deltaTime)
{
    _frameDeltas[_frameIndex] = deltaTime;
}

void ProfilerComponent::IncrementFrameIndex()
{
    _frameIndex = (_frameIndex + 1) % _frameBufferSize;
}
