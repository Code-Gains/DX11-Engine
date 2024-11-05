#include "InputSystem.hpp"
#include <imgui_impl_glfw.h>

InputSystem::InputSystem(ECS* ecs) : _ecs(ecs), _mods(0)
{
    _window = _ecs->GetGlfwWindow();
    //ImGui_ImplGlfw_InitForOther(_window, false);
    glfwSetWindowUserPointer(_window, this);

    // Set GLFW callbacks
    glfwSetKeyCallback(_window, KeyCallback);
    glfwSetCharCallback(_window, CharCallback);
    glfwSetMouseButtonCallback(_window, MouseButtonCallback);
    glfwSetCursorPosCallback(_window, CursorPositionCallback);
    glfwSetScrollCallback(_window, ScrollCallback);
    glfwSetWindowSizeCallback(_window, WindowSizeCallback);

    // Initialize mouse positions
    glfwGetCursorPos(_window, &_mouseX, &_mouseY);
    _lastMouseX = _mouseX;
    _lastMouseY = _mouseY;
    _mouseDeltaX = _mouseDeltaY = 0.0;
}

void InputSystem::Update(float deltaTime)
{
    // Reset per-frame input states
    ClearInputStates();

    // Poll GLFW events to ensure callbacks are called
    //glfwPollEvents();
}

void InputSystem::ClearInputStates()
{
    _keysPressed.clear();
    _keysReleased.clear();
    _mouseButtonsPressed.clear();
    _mouseButtonsReleased.clear();
    _mouseDeltaX = _mouseDeltaY = 0.0;
}

void InputSystem::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_UNKNOWN)
        return;

    InputSystem* inputSystem = static_cast<InputSystem*>(glfwGetWindowUserPointer(window));
    inputSystem->_mods = mods;

    if (action == GLFW_PRESS)
    {
        inputSystem->_keysPressed.insert(key);
        inputSystem->_keysHeld.insert(key);
    }
    else if (action == GLFW_RELEASE)
    {
        inputSystem->_keysReleased.insert(key);
        inputSystem->_keysHeld.erase(key);
    }
    else if (action == GLFW_REPEAT)
    {
        // Handle key repeats if necessary
    }

    // Forward event to ImGui
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
}


void InputSystem::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    InputSystem* inputSystem = static_cast<InputSystem*>(glfwGetWindowUserPointer(window));
    inputSystem->_mods = mods;

    if (action == GLFW_PRESS)
    {
        inputSystem->_mouseButtonsPressed.insert(button);
        inputSystem->_mouseButtonsHeld.insert(button);
    }
    else if (action == GLFW_RELEASE)
    {
        inputSystem->_mouseButtonsReleased.insert(button);
        inputSystem->_mouseButtonsHeld.erase(button);
    }

    // Forward event to ImGui
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
}


void InputSystem::CursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    InputSystem* inputSystem = static_cast<InputSystem*>(glfwGetWindowUserPointer(window));

    // Calculate mouse delta
    inputSystem->_mouseDeltaX = xpos - inputSystem->_lastMouseX;
    inputSystem->_mouseDeltaY = ypos - inputSystem->_lastMouseY;

    // Update last mouse position
    inputSystem->_lastMouseX = xpos;
    inputSystem->_lastMouseY = ypos;

    // Update current mouse position
    inputSystem->_mouseX = xpos;
    inputSystem->_mouseY = ypos;

    // Forward event to ImGui
    ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
}


void InputSystem::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
}

void InputSystem::CharCallback(GLFWwindow* window, unsigned int codepoint)
{
    ImGui_ImplGlfw_CharCallback(window, codepoint);
}

void InputSystem::WindowSizeCallback(GLFWwindow* window, int width, int height)
{
    InputSystem* inputSystem = static_cast<InputSystem*>(glfwGetWindowUserPointer(window));
    auto renderingApplication = inputSystem->_ecs->GetRenderingApplication3D();
    renderingApplication->Resize(width, height);
}


bool InputSystem::IsKeyPressed(int key)
{
    return _keysPressed.find(key) != _keysPressed.end();
}

bool InputSystem::IsKeyHeld(int key)
{
    return _keysHeld.find(key) != _keysHeld.end();
}

bool InputSystem::IsKeyReleased(int key)
{
    return _keysReleased.find(key) != _keysReleased.end();
}

bool InputSystem::IsMouseButtonPressed(int button)
{
    return _mouseButtonsPressed.find(button) != _mouseButtonsPressed.end();
}

bool InputSystem::IsMouseButtonHeld(int button)
{
    return _mouseButtonsHeld.find(button) != _mouseButtonsHeld.end();
}

bool InputSystem::IsMouseButtonReleased(int button)
{
    return _mouseButtonsReleased.find(button) != _mouseButtonsReleased.end();
}

//void InputSystem::GetMousePosition(double& x, double& y)
//{
//    x = _mouseX;
//    y = _mouseY;
//}
//
//void InputSystem::GetMouseDelta(double& deltaX, double& deltaY)
//{
//    deltaX = _mouseDeltaX;
//    deltaY = _mouseDeltaY;
//}


double InputSystem::GetMouseX() const
{
    return _mouseX;
}

double InputSystem::GetMouseY() const
{
    return _mouseY;
}

double InputSystem::GetLastMouseX() const
{
    return _lastMouseX;
}

double InputSystem::GetLastMouseY() const
{
    return _lastMouseY;
}

double InputSystem::GetMouseDeltaX() const
{
    return _mouseDeltaX;
}

double InputSystem::GetMouseDeltaY() const
{
    return _mouseDeltaY;
}

