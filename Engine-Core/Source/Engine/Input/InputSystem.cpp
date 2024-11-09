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

InputSystem::~InputSystem()
{
    if (_currentCursor)
    {
        glfwDestroyCursor(_currentCursor);
        _currentCursor = nullptr;
    }
}

void InputSystem::Update(float deltaTime)
{
    // Reset per-frame input states
    ClearInputStates();
}

void InputSystem::PeriodicUpdate(float deltaTime)
{
    //UpdateCursorShape();
    //PerformWindowResize();
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

//void InputSystem::UpdateCursorShape()
//{
//    int windowWidth, windowHeight;
//    glfwGetWindowSize(_window, &windowWidth, &windowHeight);
//
//    ResizeEdge edge = ResizeEdge::None;
//
//    if (_mouseX <= _resizeEdgeSize && _mouseY <= _resizeEdgeSize)
//        edge = ResizeEdge::TopLeft;
//    else if (_mouseX >= windowWidth - _resizeEdgeSize && _mouseY <= _resizeEdgeSize)
//        edge = ResizeEdge::TopRight;
//    else if (_mouseX <= _resizeEdgeSize && _mouseY >= windowHeight - _resizeEdgeSize)
//        edge = ResizeEdge::BottomLeft;
//    else if (_mouseX >= windowWidth - _resizeEdgeSize && _mouseY >= windowHeight - _resizeEdgeSize)
//        edge = ResizeEdge::BottomRight;
//    else if (_mouseX <= _resizeEdgeSize)
//        edge = ResizeEdge::Left;
//    else if (_mouseX >= windowWidth - _resizeEdgeSize)
//        edge = ResizeEdge::Right;
//    else if (_mouseY <= _resizeEdgeSize)
//        edge = ResizeEdge::Top;
//    else if (_mouseY >= windowHeight - _resizeEdgeSize)
//        edge = ResizeEdge::Bottom;
//
//    if (edge != _resizeEdge)
//    {
//        _resizeEdge = edge;
//        UpdateCursorIcon();
//    }
//}
//
//void InputSystem::UpdateCursorIcon()
//{
//    GLFWcursor* cursor = nullptr;
//
//    // Destroy the previous cursor if it exists
//    if (_currentCursor)
//    {
//        glfwDestroyCursor(_currentCursor);
//        _currentCursor = nullptr;
//    }
//
//    switch (_resizeEdge)
//    {
//    case ResizeEdge::Left:
//    case ResizeEdge::Right:
//        cursor = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
//        break;
//    case ResizeEdge::Top:
//    case ResizeEdge::Bottom:
//        cursor = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
//        break;
//    case ResizeEdge::TopLeft:
//    case ResizeEdge::TopRight:
//    case ResizeEdge::BottomLeft:
//    case ResizeEdge::BottomRight:
//        cursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
//        break;
//    default:
//        cursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
//        break;
//    }
//
//    // Set the new cursor
//    if (cursor)
//    {
//        _currentCursor = cursor;
//        glfwSetCursor(_window, _currentCursor);
//    }
//}
//
//void InputSystem::PerformWindowResize()
//{
//    int windowX, windowY, windowWidth, windowHeight;
//    glfwGetWindowPos(_window, &windowX, &windowY);
//    glfwGetWindowSize(_window, &windowWidth, &windowHeight);
//
//    // Calculate mouse movement delta
//    double deltaX = _mouseDeltaX;
//    double deltaY = _mouseDeltaY;
//
//    // Initialize new window size and position
//    int newWidth = windowWidth;
//    int newHeight = windowHeight;
//    int newX = windowX;
//    int newY = windowY;
//
//    switch (_resizeEdge)
//    {
//    case ResizeEdge::Left:
//        newWidth -= static_cast<int>(deltaX);
//        newX += static_cast<int>(deltaX);
//        break;
//    case ResizeEdge::Right:
//        newWidth += static_cast<int>(deltaX);
//        break;
//    case ResizeEdge::Top:
//        newHeight -= static_cast<int>(deltaY);
//        newY += static_cast<int>(deltaY);
//        break;
//    case ResizeEdge::Bottom:
//        newHeight += static_cast<int>(deltaY);
//        break;
//    case ResizeEdge::TopLeft:
//        newWidth -= static_cast<int>(deltaX);
//        newX += static_cast<int>(deltaX);
//        newHeight -= static_cast<int>(deltaY);
//        newY += static_cast<int>(deltaY);
//        break;
//    case ResizeEdge::TopRight:
//        newWidth += static_cast<int>(deltaX);
//        newHeight -= static_cast<int>(deltaY);
//        newY += static_cast<int>(deltaY);
//        break;
//    case ResizeEdge::BottomLeft:
//        newWidth -= static_cast<int>(deltaX);
//        newX += static_cast<int>(deltaX);
//        newHeight += static_cast<int>(deltaY);
//        break;
//    case ResizeEdge::BottomRight:
//        newWidth += static_cast<int>(deltaX);
//        newHeight += static_cast<int>(deltaY);
//        break;
//    default:
//        break;
//    }
//
//    // Enforce minimum window size
//    if (newWidth < _minWidth)
//    {
//        // Adjust X position if resizing from left
//        if (_resizeEdge == ResizeEdge::Left ||
//            _resizeEdge == ResizeEdge::TopLeft ||
//            _resizeEdge == ResizeEdge::BottomLeft)
//        {
//            newX -= (_minWidth - newWidth);
//        }
//        newWidth = _minWidth;
//    }
//
//    if (newHeight < _minHeight)
//    {
//        // Adjust Y position if resizing from top
//        if (_resizeEdge == ResizeEdge::Top ||
//            _resizeEdge == ResizeEdge::TopLeft ||
//            _resizeEdge == ResizeEdge::TopRight)
//        {
//            newY -= (_minHeight - newHeight);
//        }
//        newHeight = _minHeight;
//    }
//
//    // Apply the new window size and position
//    //glfwSetWindowSize(_window, newWidth, newHeight);
//    //glfwSetWindowPos(_window, newX, newY);
//
//    auto renderingApplication = _ecs->GetRenderingApplication3D();
//    renderingApplication->Resize(newWidth, newHeight);
//}


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

