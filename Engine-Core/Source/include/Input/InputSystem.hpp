#pragma once

#include "ECS.hpp"
#include <GLFW/glfw3.h>
#include <unordered_map>
#include <functional>
//#include "Application.hpp"
#include "RenderingApplication3D.hpp"


enum ResizeEdge
{
	None,
	Left,
	Right,
	Top,
	Bottom,
	TopLeft,
	TopRight,
	BottomLeft,
	BottomRight
};

class InputSystem : public System
{
	ECS* _ecs; // non owning
	GLFWwindow* _window; // non owning

	const int _resizeEdgeSize = 5;

	// Sets to keep track of keys and mouse buttons that are currently pressed
	std::unordered_set<int> _keysPressed;
	std::unordered_set<int> _keysHeld;
	std::unordered_set<int> _keysReleased;

	std::unordered_set<int> _mouseButtonsPressed;
	std::unordered_set<int> _mouseButtonsHeld;
	std::unordered_set<int> _mouseButtonsReleased;

	// Mouse position and delta
	double _mouseX, _mouseY;
	double _lastMouseX, _lastMouseY;
	double _mouseDeltaX, _mouseDeltaY;


	const int _minWidth = 200;
	const int _minHeight = 200;
	ResizeEdge _resizeEdge = ResizeEdge::None;
	bool _isResizing = false;

	// Owning
	GLFWcursor* _currentCursor = nullptr;

	// Modifiers
	int _mods;

	// Callbacks
	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void CursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
	static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	static void CharCallback(GLFWwindow* window, unsigned int codepoint);
	static void WindowSizeCallback(GLFWwindow* window, int width, int height);

	//void PerformWindowResize();

	//void UpdateCursorShape();
	//void UpdateCursorIcon();



public:
	InputSystem(ECS* ecs);
	~InputSystem();

	// System loops
	void Update(float deltaTime) override;
	void Render() override {}
	void PeriodicUpdate(float deltaTime) override;

	// Methods to query input state
	bool IsKeyPressed(int key);
	bool IsKeyHeld(int key);
	bool IsKeyReleased(int key);

	bool IsMouseButtonPressed(int button);
	bool IsMouseButtonHeld(int button);
	bool IsMouseButtonReleased(int button);

	//void GetMousePosition(double& x, double& y);
	//void GetMouseDelta(double& deltaX, double& deltaY);

	int GetModifiers() const { return _mods; }



	// Clear per-frame input states
	void ClearInputStates();


	// Getters for mouse position and delta
	double GetMouseX() const;
	double GetMouseY() const;

	double GetLastMouseX() const;
	double GetLastMouseY() const;

	double GetMouseDeltaX() const;
	double GetMouseDeltaY() const;

};