#pragma once

#include <cstdint>

#include <glm/glm.hpp>

namespace Engine {

enum class JoltColliderShape {
    Sphere,
    Box,
    Capsule
};

enum class JoltBodyMotion {
    Static,
    Kinematic
};

struct JoltColliderComponent {
    JoltColliderShape shape = JoltColliderShape::Sphere;
    JoltBodyMotion motion = JoltBodyMotion::Kinematic;
    bool sensor = false;
    glm::vec3 center{ 0.0f };
    float radius = 1.0f;
    glm::vec3 halfExtents{ 0.5f };
    float capsuleHalfHeight = 0.5f;
    float friction = 0.2f;
    float restitution = 0.0f;
};

struct JoltBodyComponent {
    uint32_t bodyId = 0xffffffffu;
    JoltColliderShape shape = JoltColliderShape::Sphere;
    JoltBodyMotion motion = JoltBodyMotion::Kinematic;
    bool sensor = false;
    glm::vec3 center{ 0.0f };
    float radius = 1.0f;
    glm::vec3 halfExtents{ 0.5f };
    float capsuleHalfHeight = 0.5f;
    float friction = 0.2f;
    float restitution = 0.0f;
};

} // namespace Engine
