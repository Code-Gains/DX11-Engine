#pragma once
#include <string>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

class EditorWindow
{
	std::string _title;
	bool _isVisible;

public:
	EditorWindow();
	EditorWindow(const std::string& title, bool isVisible);

	bool IsVisible() const;
	const std::string& GetTitle() const;

	void ToggleVisibility();

	virtual void Render() = 0;
};