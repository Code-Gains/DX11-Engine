#pragma once

#include "PointLightComponent.h"
#include "Transform.h"
#include "EntityState.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <vector>

namespace Engine {
struct PointLightData {
    std::array<GPUPointLight, MaxPointLights> lights{};
    uint32_t count = 0;
    uint32_t eligibleCount = 0;
    int shadowLightIndex = -1;
    entt::entity shadowEntity = entt::null;
    float shadowBias = 0.02f;
};

inline PointLightData CollectPointLights(entt::registry& registry)
{
    PointLightData result;
    std::vector<entt::entity> entities;
    auto view = registry.view<PointLightComponent, Transform>(entt::exclude<DisabledEntityTag>);
    for (auto entity : view) {
        const auto& light = view.get<PointLightComponent>(entity);
        const auto& position = view.get<Transform>(entity).position;
        if (!light.enabled || IsEntityDisabled(registry, entity) ||
            !std::isfinite(light.intensity) || light.intensity <= 0.0f ||
            !std::isfinite(light.range) || light.range <= 0.0f ||
            !std::isfinite(position.x) || !std::isfinite(position.y) || !std::isfinite(position.z) ||
            !std::isfinite(light.color.x) || !std::isfinite(light.color.y) || !std::isfinite(light.color.z)) {
            continue;
        }
        entities.push_back(entity);
    }
    // Stable selection prevents registry iteration order from changing the capped light set.
    std::sort(entities.begin(), entities.end(), [](auto a, auto b) {
        return entt::to_integral(a) < entt::to_integral(b);
    });
    result.eligibleCount = static_cast<uint32_t>(entities.size());
    result.count = std::min(result.eligibleCount, MaxPointLights);
    for (uint32_t i = 0; i < result.count; ++i) {
        const auto& light = registry.get<PointLightComponent>(entities[i]);
        result.lights[i].positionAndRange = glm::vec4(registry.get<Transform>(entities[i]).position, light.range);
        result.lights[i].colorAndIntensity = glm::vec4(glm::max(light.color, glm::vec3(0.0f)), light.intensity);
        if (result.shadowLightIndex < 0 && light.castShadows && light.range > 0.001f) {
            result.shadowLightIndex = static_cast<int>(i);
            result.shadowEntity = entities[i];
            result.shadowBias = std::isfinite(light.shadowBias) ? std::max(light.shadowBias, 0.0f) : 0.02f;
        }
    }
    return result;
}
}
