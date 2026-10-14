#pragma once
#include <glm/glm.hpp>
#include <algorithm>
#include <cmath>

struct SpotLightComponent {
    glm::vec3 color{1.0f};
    float intensity = 40.0f;
    float range = 20.0f;
    float innerAngle = 20.0f;
    float outerAngle = 30.0f;
    bool enabled = true;
    bool castShadows = true;
    float shadowBias = 0.02f;

    void Sanitize() {
        intensity = std::isfinite(intensity) ? std::max(intensity, 0.0f) : 0.0f;
        range = std::isfinite(range) ? std::max(range, 0.0f) : 0.0f;
        outerAngle = std::isfinite(outerAngle) ? std::clamp(outerAngle, 1.0f, 85.0f) : 30.0f;
        innerAngle = std::isfinite(innerAngle) ? std::clamp(innerAngle, 0.0f, outerAngle - 0.01f) : 0.0f;
        shadowBias = std::isfinite(shadowBias) ? std::max(shadowBias, 0.0f) : 0.02f;
    }
};

namespace Engine {
constexpr uint32_t MaxSpotLights = 8;
constexpr uint32_t SpotShadowResolution = 1024;
struct GPUSpotLight {
    glm::vec4 positionAndRange{0.0f};
    glm::vec4 colorAndIntensity{0.0f};
    glm::vec4 directionAndOuterCos{0.0f};
    glm::vec4 coneParameters{0.0f}; // x: inner cosine
};
}
