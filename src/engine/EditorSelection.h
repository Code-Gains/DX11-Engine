#pragma once

#include <entt/entt.hpp>
#include <glm/vec3.hpp>

struct EditorSelection {
    entt::entity selectedEntity{ entt::null };
};

// One-shot framing request from editor tools, without changing scene selection.
struct EditorCameraFocusRequest {
    glm::vec3 center{0.0f};
    float radius = 1.0f;
};
