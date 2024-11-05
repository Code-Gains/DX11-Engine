#include "EditorUIManagerSystem.hpp"
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

EditorUIManagerSystem::EditorUIManagerSystem()
{
}

EditorUIManagerSystem::EditorUIManagerSystem(ECS* ecs) : _ecs(ecs)
{
}

void EditorUIManagerSystem::Update(float deltaTime)
{
}

void EditorUIManagerSystem::Render()
{
    RenderMenuBar();
}


void EditorUIManagerSystem::PeriodicUpdate(float deltaTime)
{
}

void EditorUIManagerSystem::RenderMenuBar()
{
    // Preserve original styles
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

    // Begin the main menu bar
    if (ImGui::BeginMainMenuBar())
    {
        RenderMenuBarMenuButtons();

        // Calculate the width of the control buttons
        float controlButtonStackWidth = 68.0f;

        RenderMenuBarInvisibleDragButton(controlButtonStackWidth);
        RenderMenuBarControlButtons(controlButtonStackWidth);
        RenderWindowInvisibleResizePanels();

        // End the main menu bar
        ImGui::EndMainMenuBar();
    }

    // Restore original styles
    ImGui::PopStyleVar(2);
}

void EditorUIManagerSystem::RenderMenuBarMenuButtons()
{
    // Existing menu items
    if (ImGui::BeginMenu("File"))
    {
        ImGui::EndMenu();
    }
    // "View" menu
    if (ImGui::BeginMenu("View"))
    {
        // On click, get status from the global system registry and toggle enabled bool
        if (ImGui::MenuItem("World Hierarchy", nullptr, &_showWorldHierarchy))
        {
            auto worldHierarchy = _ecs->GetSystem<WorldHierarchy>();
            if (worldHierarchy)
                worldHierarchy->Toggle();
        }
        if (ImGui::MenuItem("ECS Debugger", nullptr, &_showEcsDebugger))
        {
            auto ecsDebugger = _ecs->GetSystem<ECSDebugger>();
            if (ecsDebugger)
                ecsDebugger->Toggle();
        }
        if (ImGui::MenuItem("EMS", nullptr, &_showEMS))
        {
            auto ems = _ecs->GetSystem<EntityMonitoringSystem>();
            if (ems)
                ems->Toggle();
        }
        if (ImGui::MenuItem("Profiler", nullptr, &_showProfiler))
        {
            auto profilerSystem = _ecs->GetSystem<ProfilerSystem>();
            if (profilerSystem)
                profilerSystem->Toggle();
        }
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Settings"))
    {
        ImGui::EndMenu();
    }
}

void EditorUIManagerSystem::RenderMenuBarControlButtons(int buttonStackWidth)
{

    // Position the control buttons at the right of the menu bar
    ImGui::SameLine(ImGui::GetWindowWidth() - buttonStackWidth);

    auto window = _ecs->GetGlfwWindow();
    if (ImGui::Button("-"))
    {
        glfwIconifyWindow(window);
        //auto renderingApplication = _ecs->GetRenderingApplication3D();
        //renderingApplication->Resize(0, 0);
    }
    ImGui::SameLine();
    if (ImGui::Button("[]"))
    {
        if (glfwGetWindowAttrib(window, GLFW_MAXIMIZED))
        {
            glfwRestoreWindow(window);
        }
        else
        {
            glfwMaximizeWindow(window);
        }

        //int width, height;
        //glfwGetWindowSize(window, &width, &height);

        //auto renderingApplication = _ecs->GetRenderingApplication3D();
        //renderingApplication->Resize(width, height);
    }
    ImGui::SameLine();
    if (ImGui::Button("X"))
    {
        glfwTerminate();
    }
}

void EditorUIManagerSystem::RenderMenuBarInvisibleDragButton(int controlButtonStackWidth)
{
    // Get the cursor position after the last menu item
        float cursorPosX = ImGui::GetCursorPosX();

        // Calculate the available width for the draggable area
        float availableWidth = ImGui::GetWindowWidth() - cursorPosX - controlButtonStackWidth;

        // Create an invisible button over the non-button area
        if (availableWidth > 0.0f)
        {
            ImGui::SameLine(cursorPosX);
            ImGui::InvisibleButton("##DragArea", ImVec2(availableWidth, ImGui::GetFrameHeight()));

            // Handle window dragging
            if (ImGui::IsItemActive())
            {
                InputSystem* inputSystem = _ecs->GetSystem<InputSystem>();
                if (inputSystem && inputSystem->IsMouseButtonHeld(GLFW_MOUSE_BUTTON_LEFT))
                {
                    if (!_isDraggingWindow)
                    {
                        // Dragging just started
                        _isDraggingWindow = true;

                        // Record the initial window position
                        glfwGetWindowPos(_ecs->GetGlfwWindow(), &_dragStartWindowX, &_dragStartWindowY);

                        // Get the mouse position relative to the window
                        double mouseX = inputSystem->GetMouseX();
                        double mouseY = inputSystem->GetMouseY();

                        // Calculate the initial mouse position in screen coordinates
                        _dragStartMouseX = mouseX + _dragStartWindowX;
                        _dragStartMouseY = mouseY + _dragStartWindowY;
                    }
                    else if (_isDraggingWindow)
                    {
                        // Dragging in progress

                        // Get current mouse position relative to the window
                        double mouseX = inputSystem->GetMouseX();
                        double mouseY = inputSystem->GetMouseY();

                        // Get current window position
                        int windowX, windowY;
                        glfwGetWindowPos(_ecs->GetGlfwWindow(), &windowX, &windowY);

                        // Calculate the current mouse position in screen coordinates
                        double currentMouseX = mouseX + windowX;
                        double currentMouseY = mouseY + windowY;

                        // Calculate the difference
                        double deltaX = currentMouseX - _dragStartMouseX;
                        double deltaY = currentMouseY - _dragStartMouseY;

                        // Move the window
                        glfwSetWindowPos(_ecs->GetGlfwWindow(),
                            _dragStartWindowX + static_cast<int>(deltaX),
                            _dragStartWindowY + static_cast<int>(deltaY));
                    }
                    else
                    {
                        // Dragging ended
                        _isDraggingWindow = false;
                    }
                }
            }
        }
}

void EditorUIManagerSystem::RenderWindowInvisibleResizePanels()
{
    // Create 4 invisible panels at the side of the window when it is not maximized
    // When Dragging these pannels cursor should indicate that the window is available for resize
}
