#pragma once
#include "SpotLightComponent.h"
#include "Transform.h"
#include "EntityState.h"
#include "PointShadowMath.h"
#include <vector>

namespace Engine {
struct SpotLightData {
    std::array<GPUSpotLight, MaxSpotLights> lights{};
    uint32_t count = 0;
    uint32_t eligibleCount = 0;
    int shadowLightIndex = -1;
    entt::entity shadowEntity = entt::null;
    float shadowBias = 0.02f;
};

inline SpotLightData CollectSpotLights(entt::registry& registry) {
    SpotLightData result;
    std::vector<entt::entity> entities;
    auto view = registry.view<SpotLightComponent, Transform>(entt::exclude<DisabledEntityTag>);
    for (auto entity : view) {
        const auto& light = view.get<SpotLightComponent>(entity);
        const auto& transform = view.get<Transform>(entity);
        const auto direction = transform.rotation * glm::vec3(0, 0, -1);
        if (!light.enabled || IsEntityDisabled(registry, entity) ||
            !std::isfinite(light.intensity) || light.intensity <= 0 ||
            !std::isfinite(light.range) || light.range <= 0 ||
            !std::isfinite(light.innerAngle) || !std::isfinite(light.outerAngle) ||
            !std::isfinite(transform.position.x) || !std::isfinite(transform.position.y) || !std::isfinite(transform.position.z) ||
            !std::isfinite(light.color.x) || !std::isfinite(light.color.y) || !std::isfinite(light.color.z) ||
            !std::isfinite(direction.x) || !std::isfinite(direction.y) || !std::isfinite(direction.z) ||
            glm::length(direction) < 0.001f) continue;
        entities.push_back(entity);
    }
    std::sort(entities.begin(), entities.end(), [](auto a, auto b) { return entt::to_integral(a) < entt::to_integral(b); });
    result.eligibleCount = static_cast<uint32_t>(entities.size());
    result.count = std::min(result.eligibleCount, MaxSpotLights);
    for (uint32_t i = 0; i < result.count; ++i) {
        auto light = registry.get<SpotLightComponent>(entities[i]);
        light.Sanitize();
        const auto& transform = registry.get<Transform>(entities[i]);
        result.lights[i].positionAndRange = glm::vec4(transform.position, light.range);
        result.lights[i].colorAndIntensity = glm::vec4(glm::max(light.color, glm::vec3(0)), light.intensity);
        result.lights[i].directionAndOuterCos = glm::vec4(glm::normalize(transform.rotation * glm::vec3(0,0,-1)),
            std::cos(glm::radians(light.outerAngle)));
        result.lights[i].coneParameters.x = std::cos(glm::radians(light.innerAngle));
        if (result.shadowLightIndex < 0 && light.castShadows && light.range > 0.001f) {
            result.shadowLightIndex = static_cast<int>(i);
            result.shadowEntity = entities[i];
            result.shadowBias = light.shadowBias;
        }
    }
    return result;
}

inline glm::mat4 BuildSpotShadowMatrix(glm::vec3 position, glm::vec3 direction, float outerAngle, float range) {
    direction = glm::normalize(direction);
    const glm::vec3 up = std::abs(direction.y) > 0.99f ? glm::vec3(1,0,0) : glm::vec3(0,1,0);
    auto projection = glm::perspectiveRH_ZO(glm::radians(2.0f * outerAngle), 1.0f, range, PointShadowNearPlane(range));
    projection[1][1] *= -1.0f;
    return projection * glm::lookAtRH(position, position + direction, up);
}
}
