#pragma once
#include "EditorWindow.hpp"

class ProfilerWindow : public EditorWindow
{

public:
	ProfilerWindow(bool isVisible = true);
	void Render() override;
};