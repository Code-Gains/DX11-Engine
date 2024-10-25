#include "EditorWindow.hpp"

EditorWindow::EditorWindow()
{
}

EditorWindow::EditorWindow(const std::string& title, bool isVisible = true)
	: _title(title), _isVisible(isVisible)
{
}

bool EditorWindow::IsVisible() const
{
	return _isVisible;
}

const std::string& EditorWindow::GetTitle() const
{
	return _title;
}

void EditorWindow::ToggleVisibility()
{
	_isVisible = !_isVisible;
}