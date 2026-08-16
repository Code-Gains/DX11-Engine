#pragma once

#include <glm/vec3.hpp>

struct HeightFogComponent {
    bool enabled = true;
    bool debugOverlay = false;
    glm::vec3 color{ 0.42f, 0.55f, 0.68f };
    float planetRadius = 50.0f;
    float height = 80.0f;
    float density = 0.012f;
    float distanceFalloff = 1.0f;
    float maxOpacity = 0.7f;
    float skyDensityMultiplier = 1.0f;
    float skyMaxOpacity = 1.0f;
};
