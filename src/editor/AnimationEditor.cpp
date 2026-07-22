#include "AnimationEditor.h"

#include "Camera.h"
#include "ImGuiWindowRegistry.h"
#include "NameComponent.h"
#include "Transform.h"

#include <imgui.h>
#include <string>

namespace {

const char* EntityLabel(entt::registry& registry, entt::entity entity)
{
    static std::string label;

    if (entity == entt::null || !registry.valid(entity)) {
        label = "No selected entity";
        return label.c_str();
    }

    const int entityId = static_cast<int>(entt::to_integral(entity));
    if (auto* name = registry.try_get<NameComponent>(entity)) {
        label = name->name + " [" + std::to_string(entityId) + "]";
    }
    else {
        label = "Entity " + std::to_string(entityId);
    }

    return label.c_str();
}

} // namespace

AnimationEditor::AnimationEditor(entt::registry& registry, RegistryViewer* registryViewerPtr)
    : System(registry)
    , _registryViewerPtr(registryViewerPtr)
{
    auto& windowRegistry = _registry.ctx().get<ImGuiWindowRegistry>();
    windowRegistry.RegisterWindow("Animation Editor", false);
}

void AnimationEditor::DrawUi()
{
    auto& windowRegistry = _registry.ctx().get<ImGuiWindowRegistry>();
    if (!windowRegistry.IsWindowOpen("Animation Editor")) {
        return;
    }

    bool open = true;
    if (ImGui::Begin("Animation Editor", &open)) {
        const entt::entity selectedEntity = _registryViewerPtr
            ? _registryViewerPtr->GetSelectedEntity()
            : entt::null;

        ImGui::Text("Selected: %s", EntityLabel(_registry, selectedEntity));

        if (selectedEntity == entt::null || !_registry.valid(selectedEntity)) {
            ImGui::TextDisabled("Select an entity with a camera shot component.");
        }
        else if (!_registry.all_of<Camera, Transform, CinematicCameraShotComponent>(selectedEntity)) {
            ImGui::TextDisabled("Camera shot editing requires Camera, Transform, and Cinematic Camera Shot.");
        }
        else {
            auto& camera = _registry.get<Camera>(selectedEntity);
            auto& transform = _registry.get<Transform>(selectedEntity);
            auto& shot = _registry.get<CinematicCameraShotComponent>(selectedEntity);

            _cameraShotEditorUi.Draw(_registry, transform, camera, shot);
        }
    }

    ImGui::End();
    windowRegistry.SetWindowOpen("Animation Editor", open);
}
