#pragma once

#include <glm/vec3.hpp>

struct ScreenPostProcessComponent {
    bool enabled = true;
    bool debugOverlay = false;
    bool useScreenRadius = true;
    glm::vec3 color{ 0.75f, 0.95f, 1.0f };
    float scale = 8.0f;
    float softness = 0.04f;
    float intensity = 0.35f;
    float amount = 0.0f;
    float displacement = 0.012f;
    float chromaticAberration = 0.006f;
    float blockSize = 0.03f;
    float screenRadius = 0.18f;
    float screenFeather = 0.12f;
    float speed = 1.0f;
    float age = 0.0f;
};
