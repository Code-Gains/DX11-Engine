#pragma once

#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <array>
#include <algorithm>

namespace Engine {
constexpr uint32_t PointShadowResolution = 512;

inline float PointShadowNearPlane(float range)
{
    return std::min(0.05f, range * 0.01f);
}

inline std::array<glm::mat4, 6> BuildPointShadowMatrices(glm::vec3 position, float nearPlane, float range)
{
    const std::array<glm::vec3, 6> directions{{{1,0,0}, {-1,0,0}, {0,1,0}, {0,-1,0}, {0,0,1}, {0,0,-1}}};
    const std::array<glm::vec3, 6> up{{{0,-1,0}, {0,-1,0}, {0,0,1}, {0,0,-1}, {0,-1,0}, {0,-1,0}}};
    // Reversed depth. No Y flip: these views match Vulkan cubemap sampling coordinates.
    const auto projection = glm::perspectiveRH_ZO(glm::radians(90.0f), 1.0f, range, nearPlane);
    std::array<glm::mat4, 6> matrices;
    for (size_t face = 0; face < matrices.size(); ++face) {
        matrices[face] = projection * glm::lookAtRH(position, position + directions[face], up[face]);
    }
    return matrices;
}

inline bool PointShadowSphereVisible(glm::vec3 center, float radius, const glm::mat4& viewProjection)
{
    const auto rows = glm::transpose(viewProjection);
    const std::array<glm::vec4, 6> planes{{rows[3] + rows[0], rows[3] - rows[0],
        rows[3] + rows[1], rows[3] - rows[1], rows[2], rows[3] - rows[2]}};
    for (const auto& plane : planes) {
        if (glm::dot(glm::vec3(plane), center) + plane.w < -radius * glm::length(glm::vec3(plane))) return false;
    }
    return true;
}
}
