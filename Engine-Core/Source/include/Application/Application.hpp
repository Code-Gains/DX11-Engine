#pragma once

#include <cstdint>
#include <string>
#include <chrono>

// ReSharper disable once CppInconsistentNaming
struct GLFWwindow;


class Application
{
public:
    Application(const std::string& title);
    virtual ~Application();

    virtual bool Initialize();
    virtual bool Load() = 0;
    void Run();

    [[nodiscard]] int32_t GetWindowWidth() const;
    [[nodiscard]] int32_t GetWindowHeight() const;

protected:
    static void HandleResize(
        GLFWwindow* window,
        int32_t width,
        int32_t height);
    virtual void OnResize(
        int32_t width,
        int32_t height);

    virtual void Cleanup();
    virtual void Render() = 0;
    virtual void Update();
    virtual void PeriodicUpdate();

    [[nodiscard]] GLFWwindow* GetWindow() const;

    int32_t _width = 0;
    int32_t _height = 0;
    float _deltaTime = 0.016f;
    float _periodicDeltaTime = 0;
    const float _periodicUpdatePeriod = 0.016f;
    GLFWwindow* _window = nullptr;

    bool _isMinimized = false;
private:
    std::chrono::high_resolution_clock::time_point _currentTime;
    std::string _title;
};


//#pragma once
//
//#include <cstdint>
//#include <string>
//#include <chrono>
//
//// Include the necessary Win32 API headers
//#include <Windows.h>
//
//class Application
//{
//public:
//    Application(const std::string& title);
//    virtual ~Application();
//
//    void Run();
//
//protected:
//    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//
//    void HandleResize(int32_t width, int32_t height);
//    virtual void OnResize(int32_t width, int32_t height);
//
//    virtual bool Initialize();
//    virtual bool Load() = 0;
//    virtual void Cleanup();
//    virtual void Render() = 0;
//    virtual void Update();
//
//    HWND GetWindow() const;
//    int32_t GetWindowWidth() const;
//    int32_t GetWindowHeight() const;
//
//    int32_t _width = 0;
//    int32_t _height = 0;
//    float _deltaTime = 0.016f;
//
//private:
//    std::chrono::high_resolution_clock::time_point _currentTime;
//    HWND _window = nullptr;
//    std::string _title;
//};
