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

        // End the main menu bar
        ImGui::EndMainMenuBar();
    }

    // Restore original styles
    ImGui::PopStyleVar(2);

    RenderWindowInvisibleResizePanels();
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
        if (ImGui::MenuItem("Entity Editor", nullptr, &_showEntityEditor))
        {
            auto entityEditor = _ecs->GetSystem<EntityEditor>();
            if (entityEditor)
                entityEditor->Toggle();
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

                    // Get the absolute mouse position using GLFW
                    double mouseX, mouseY;
                    glfwGetCursorPos(_ecs->GetGlfwWindow(), &mouseX, &mouseY);

                    // Get the monitor's position relative to the desktop
                    int monitorX, monitorY;
                    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
                    glfwGetMonitorPos(glfwGetPrimaryMonitor(), &monitorX, &monitorY);

                    // Calculate the initial mouse position in screen coordinates
                    _dragStartMouseX = mouseX + _dragStartWindowX;
                    _dragStartMouseY = mouseY + _dragStartWindowY;
                }
                else if (_isDraggingWindow)
                {
                    // Dragging in progress

                    // Get current mouse position using GLFW
                    double currentMouseX, currentMouseY;
                    glfwGetCursorPos(_ecs->GetGlfwWindow(), &currentMouseX, &currentMouseY);

                    // Get the initial window position and calculate the new position
                    int windowX, windowY;
                    glfwGetWindowPos(_ecs->GetGlfwWindow(), &windowX, &windowY);

                    // Calculate the current mouse position in screen coordinates
                    double absoluteMouseX = currentMouseX + windowX;
                    double absoluteMouseY = currentMouseY + windowY;

                    // Calculate the difference
                    double deltaX = absoluteMouseX - _dragStartMouseX;
                    double deltaY = absoluteMouseY - _dragStartMouseY;

                    // Move the window
                    glfwSetWindowPos(_ecs->GetGlfwWindow(),
                        _dragStartWindowX + static_cast<int>(deltaX),
                        _dragStartWindowY + static_cast<int>(deltaY));
                }
            }
        }
        else
        {
            // If the item is not active, stop dragging
            _isDraggingWindow = false;
        }
    }
}


void EditorUIManagerSystem::RenderWindowInvisibleResizePanels()
{
    // Create 4 invisible panels at the side of the window when it is not maximized
    // When Dragging these pannels cursor should indicate that the window is available for resize

    //// Get the available content region size
    //ImVec2 content_avail = ImGui::GetContentRegionAvail();
    //float window_width = content_avail.x;
    //float window_height = content_avail.y;

    //// Static variable for panel width
    //static float left_panel_width = 200.0f; // Initial width

    //// Left Panel
    //ImGui::SetNextWindowPos(ImVec2(0, ImGui::GetFrameHeight())); // Position below the menu bar
    //ImGui::SetNextWindowSize(ImVec2(left_panel_width, window_height));

    //ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
    //    ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
    //    ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    //ImGui::Begin("Left Panel", nullptr, window_flags);
    //{
    //    // Your left panel content goes here
    //    ImGui::Text("Left Panel");
    //    // ... more content
    //}
    //ImGui::End();
}


bool EditorUIManagerSystem::Splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2)
{
    ImVec2 backup_pos = ImGui::GetCursorPos();

    if (split_vertically)
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + *size1);
    else
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + *size1);

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.6f, 0.6f, 0.1f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.8f, 0.8f, 0.1f));

    ImGui::InvisibleButton("splitter", ImVec2(
        split_vertically ? thickness : -1.0f,
        split_vertically ? -1.0f : thickness));

    ImGui::PopStyleColor(3);

    bool is_active = ImGui::IsItemActive();
    if (ImGui::IsItemActive())
    {
        float mouse_delta = split_vertically ? ImGui::GetIO().MouseDelta.x : ImGui::GetIO().MouseDelta.y;

        // Adjust sizes
        if (mouse_delta < min_size1 - *size1)
            mouse_delta = min_size1 - *size1;
        if (mouse_delta > *size2 - min_size2)
            mouse_delta = *size2 - min_size2;

        *size1 += mouse_delta;
        *size2 -= mouse_delta;
    }

    ImGui::SetCursorPos(backup_pos);
    return is_active;
}

