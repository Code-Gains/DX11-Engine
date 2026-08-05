#pragma once

#include <glm/vec3.hpp>

struct ScreenPostProcessComponent {
    bool enabled = true;
    bool debugOverlay = false;
    bool useWorldRadius = true;
    glm::vec3 color{ 0.75f, 0.95f, 1.0f };
    float scale = 8.0f;
    float softness = 0.04f;
    float intensity = 0.35f;
    float amount = 0.0f;
    float displacement = 0.012f;
    float chromaticAberration = 0.006f;
    float blockSize = 0.03f;
    float radius = 20.0f;
    float feather = 8.0f;
    float skyRadius = 0.25f;
    float speed = 1.0f;
    float age = 0.0f;
};
