#pragma once

#include <glm/glm.hpp>

struct PointLightComponent {
    glm::vec3 color{1.0f};
    float intensity = 20.0f;
    float range = 10.0f;
    bool enabled = true;
    bool castShadows = false;
    float shadowBias = 0.02f;
};

namespace Engine {
constexpr unsigned int MaxPointLights = 16;
struct GPUPointLight {
    glm::vec4 positionAndRange{0.0f};
    glm::vec4 colorAndIntensity{0.0f};
};
}
