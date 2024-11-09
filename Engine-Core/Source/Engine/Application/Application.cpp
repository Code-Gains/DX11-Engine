#include <Application.hpp>

#include <GLFW/glfw3.h>

#include <iostream>
#include <ratio>
#include <imgui.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#include <thread>
#include <chrono>

Application::Application(const std::string& title)
    : _title(title)
{
}

Application::~Application()
{
    Application::Cleanup();
}

bool Application::Initialize()
{
    if (!glfwInit()) // #TODO move out to rendering application
    {
        std::cerr << "GLFW: Failed to initialize\n";
        return false;
    }

    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* videoMode = glfwGetVideoMode(primaryMonitor);
    _width = static_cast<int32_t>(videoMode->width * 0.5f);
    _height = static_cast<int32_t>(videoMode->height * 0.5f);

    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_FALSE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    _window = glfwCreateWindow(
        _width,
        _height,
        _title.data(),
        nullptr,
        nullptr);

    if (_window == nullptr)
    {
        std::cerr << "GLFW: Failed to create a window\n";
        Cleanup();
        return false;
    }

    const int32_t windowLeft = videoMode->width / 2 - _width / 2;
    const int32_t windowTop = videoMode->height / 2 - _height / 2;
    glfwSetWindowPos(_window, windowLeft, windowTop);

    //glfwSetWindowUserPointer(_window, this);
    glfwSetFramebufferSizeCallback(_window, HandleResize);

    _currentTime = std::chrono::high_resolution_clock::now();

    //ImGui::CreateContext();
    //ImGui::StyleColorsDark();
    //ImGui_ImplGlfw_InitForOther(_window, false);

    return true;
}

void Application::OnResize(
    const int32_t width,
    const int32_t height)
{
    _width = width;
    _height = height;
}

void Application::Cleanup()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(_window);
    glfwTerminate();
}

void Application::Run()
{
    constexpr auto targetMinimizedFrameDuration = std::chrono::milliseconds(50); // 20 FPS

    while (!glfwWindowShouldClose(_window))
    {
        auto frameStartTime = std::chrono::high_resolution_clock::now();

        glfwPollEvents();

        Update();
        PeriodicUpdate();

        if (_isMinimized)
        {
            auto frameEndTime = std::chrono::high_resolution_clock::now();
            auto frameDuration = frameEndTime - frameStartTime;

            auto sleepDuration = targetMinimizedFrameDuration - frameDuration;

            if (sleepDuration > std::chrono::milliseconds(0))
            {
                std::this_thread::sleep_for(sleepDuration);
            }

            continue;
        }

        Render();
    }
}

void Application::HandleResize(
    GLFWwindow* window,
    const int32_t width,
    const int32_t height)
{
    Application* application = static_cast<Application*>(glfwGetWindowUserPointer(window));
    application->OnResize(width, height);
}

GLFWwindow* Application::GetWindow() const
{
    return _window;
}

int32_t Application::GetWindowWidth() const
{
    return _width;
}

int32_t Application::GetWindowHeight() const
{
    return _height;
}

void Application::Update()
{
    auto oldTime = _currentTime;
    _currentTime = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> timeSpan = (_currentTime - oldTime);
    _deltaTime = static_cast<float>(timeSpan.count() / 1000.0);
    _periodicDeltaTime += _deltaTime;
}

void Application::PeriodicUpdate()
{

}


//#include "Application.hpp"
//#include <Windows.h>
//#include <d3d11.h>
//#include <iostream>
//#include <chrono>
//#include "imgui.h"
//#include "imgui_impl_win32.h"
//#include "imgui_impl_dx11.h"
//
//
//LRESULT CALLBACK Application::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
//{
//    Application* application = reinterpret_cast<Application*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
//
//    switch (uMsg)
//    {
//    case WM_CLOSE:
//        PostQuitMessage(0);
//        return 0;
//    case WM_SIZE:
//        if (application)
//        {
//            application->HandleResize(LOWORD(lParam), HIWORD(lParam));
//        }
//        return 0;
//    default:
//        return DefWindowProc(hwnd, uMsg, wParam, lParam);
//    }
//}
//
//Application::Application(const std::string& title)
//    : _title(title), _window(nullptr), _width(800), _height(600), _currentTime(), _deltaTime(0.0f)
//{
//}
//
//
//Application::~Application()
//{
//    Cleanup();
//}
//
//bool Application::Initialize()
//{
//    WNDCLASSEX wcex;
//    wcex.cbSize = sizeof(WNDCLASSEX);
//    wcex.style = CS_HREDRAW | CS_VREDRAW;
//    wcex.lpfnWndProc = WindowProc;
//    wcex.cbClsExtra = 0;
//    wcex.cbWndExtra = 0;
//    wcex.hInstance = GetModuleHandle(NULL);
//    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
//    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
//    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
//    wcex.lpszMenuName = NULL;
//    wcex.lpszClassName = TEXT("RawWin32App");
//    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
//
//    if (!RegisterClassEx(&wcex))
//    {
//        std::cerr << "Failed to register window class." << std::endl;
//        return false;
//    }
//
//    int32_t windowLeft = 0;
//    int32_t windowTop = 0;
//
//    int32_t screenWidth = GetSystemMetrics(SM_CXSCREEN);
//    int32_t screenHeight = GetSystemMetrics(SM_CYSCREEN);
//
//    windowLeft = screenWidth / 2 - _width / 2;
//    windowTop = screenHeight / 2 - _height / 2;
//
//    _width = static_cast<int32_t>(screenWidth * 0.5f);
//    _height = static_cast<int32_t>(screenHeight * 0.5f);
//
//    _window = CreateWindowEx(
//        0,
//        TEXT("RawWin32App"),
//        TEXT("Raw Win32 App"),
//        WS_OVERLAPPEDWINDOW,
//        windowLeft, windowTop,
//        _width, _height,
//        NULL, NULL,
//        GetModuleHandle(NULL),
//        NULL);
//
//    if (!_window)
//    {
//        std::cerr << "Failed to create window." << std::endl;
//        Cleanup();
//        return false;
//    }
//
//    ShowWindow(_window, SW_SHOWDEFAULT);
//
//    _currentTime = std::chrono::high_resolution_clock::now();
//    return true;
//}
//
//
//void Application::HandleResize(int32_t width, int32_t height)
//{
//    OnResize(width, height);
//}
//
//void Application::OnResize(int32_t width, int32_t height)
//{
//    _width = width;
//    _height = height;
//}
//
//HWND Application::GetWindow() const
//{
//    return _window;
//}
//
//int32_t Application::GetWindowWidth() const
//{
//    return _width;
//}
//
//int32_t Application::GetWindowHeight() const
//{
//    return _height;
//}
//
//void Application::Cleanup()
//{
//    ImGui_ImplDX11_Shutdown();
//    ImGui_ImplWin32_Shutdown();
//    ImGui::DestroyContext();
//
//    DestroyWindow(_window);
//    UnregisterClass(TEXT("RawWin32App"), GetModuleHandle(NULL));
//}
//
//void Application::Run()
//{
//    if (!Initialize())
//    {
//        return;
//    }
//
//    if (!Load())
//    {
//        Cleanup();
//        return;
//    }
//
//    MSG msg = {};
//    while (true)
//    {
//        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
//        {
//            if (msg.message == WM_QUIT)
//                break;
//
//            TranslateMessage(&msg);
//            DispatchMessage(&msg);
//        }
//        else
//        {
//            Update();
//            Render();
//        }
//    }
//    Cleanup();
//}
//
//void Application::Update()
//{
//    auto oldTime = _currentTime;
//    _currentTime = std::chrono::high_resolution_clock::now();
//
//    std::chrono::duration<double, std::milli> timeSpan = (_currentTime - oldTime);
//    _deltaTime = static_cast<float>(timeSpan.count() / 1000.0);
//}
