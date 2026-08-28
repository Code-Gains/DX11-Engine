#pragma once
#include "Transform.h"

#include <entt/entt.hpp>
#include <vector>

struct ActiveCameraTag {};
struct EditorCameraPilotTag {};

enum class CameraBackgroundMode {
    None,
    SolidColor,
    Skybox
};

enum class CameraShotInterpolationMode {
    Linear,
    Smoothstep,
    CatmullRom
};

enum class CameraShotAimMode {
    UseRotation,
    LookAtPoint,
    LookAtEntity
};

struct CameraShotKeyframe {
    float duration = 1.0f;
    glm::vec3 position{ 0.0f };
    glm::quat rotation{ 1.0f, 0.0f, 0.0f, 0.0f };
    float fov = 90.0f;
    CameraShotInterpolationMode interpolationMode = CameraShotInterpolationMode::CatmullRom;
    CameraShotAimMode aimMode = CameraShotAimMode::UseRotation;
    glm::vec3 lookAtPoint{ 0.0f };
    entt::entity lookAtEntity{ entt::null };
};

struct CinematicCameraShotComponent {
    std::vector<CameraShotKeyframe> keyframes;
    float duration = 3.0f;
    float time = 0.0f;
    float playbackSpeed = 1.0f;
    bool playing = false;
    bool loop = false;
    bool showPath = true;
    CameraShotInterpolationMode interpolationMode = CameraShotInterpolationMode::CatmullRom;
};

struct Camera {
    float fov = 90.0f;
    float nearPlane = 0.1f;
    float farPlane = 100000.0f;
    CameraBackgroundMode backgroundMode = CameraBackgroundMode::Skybox;
    glm::vec4 clearColor {0.1f, 0.1f, 0.5f, 1.0f};

    // looking along -Z initially
    glm::vec3 direction { 0.0f, 0.0f, -1.0f };
    float yaw = -90.0f;

    // horizontal view
    glm::vec3 up { 0.0f, 1.0f, 0.0f };
    float pitch = 0.0f;

    // Compute view matrix from Transform
    // glm::mat4 GetViewMatrix(const Transform& transform) const {
    //     return glm::lookAt(transform.position, transform.position + direction, up);
    // }

    glm::mat4 GetProjectionMatrix(float aspectRatio) const {
        auto projectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, farPlane, nearPlane);
        projectionMatrix[1][1] *= -1; // Vulkan Y coordinate correction
        return projectionMatrix;
    }
    
    glm::mat4 GetViewMatrix(const Transform& transform) const {
        glm::vec3 forward = glm::normalize(transform.rotation * glm::vec3(0.0f, 0.0f, -1.0f));
        glm::vec3 up      = glm::normalize(transform.rotation * glm::vec3(0.0f, 1.0f,  0.0f));

        return glm::lookAt(transform.position, transform.position + forward, up);
    }
    

    // controls
    float speed = 20.0f;
    float orbitSensitivity = 0.35f;
    float panSensitivity = 0.002f;
    float zoomSensitivity = 0.12f;
    float focusDistanceScale = 4.0f;
    bool screenshotRequested = false;
};
