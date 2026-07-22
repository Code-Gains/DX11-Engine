#include "CinematicCameraSystem.h"

#include "Camera.h"
#include "Core.h"
#include "EntityState.h"
#include "Transform.h"

#include <algorithm>
#include <cmath>
#include <glm/geometric.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <optional>

namespace {

float SmoothStep(float value)
{
    const float t = glm::clamp(value, 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

glm::vec3 CatmullRom(
    const glm::vec3& p0,
    const glm::vec3& p1,
    const glm::vec3& p2,
    const glm::vec3& p3,
    float value)
{
    const float t = glm::clamp(value, 0.0f, 1.0f);
    const float t2 = t * t;
    const float t3 = t2 * t;

    return 0.5f * (
        (2.0f * p1) +
        (-p0 + p2) * t +
        (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 +
        (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3
    );
}

bool IsClosedCameraShotPath(const CinematicCameraShotComponent& shot)
{
    if (!shot.loop || shot.keyframes.size() < 4) {
        return false;
    }

    const glm::vec3 delta =
        shot.keyframes.front().position - shot.keyframes.back().position;
    return glm::dot(delta, delta) < 0.0001f;
}

std::size_t PreviousControlIndex(
    const CinematicCameraShotComponent& shot,
    std::size_t previousIndex,
    bool closedPath)
{
    if (previousIndex > 0) {
        return previousIndex - 1;
    }

    return closedPath ? shot.keyframes.size() - 2 : previousIndex;
}

std::size_t NextControlIndex(
    const CinematicCameraShotComponent& shot,
    std::size_t nextIndex,
    bool closedPath)
{
    if (nextIndex + 1 < shot.keyframes.size()) {
        return nextIndex + 1;
    }

    return closedPath ? 1 : nextIndex;
}

glm::vec3 ForwardFromRotation(const glm::quat& rotation)
{
    return glm::normalize(rotation * glm::vec3{ 0.0f, 0.0f, -1.0f });
}

void SyncCameraDirection(Camera& camera, const Transform& transform)
{
    const glm::vec3 direction = ForwardFromRotation(transform.rotation);
    camera.direction = direction;
    camera.pitch = glm::degrees(asin(glm::clamp(direction.y, -1.0f, 1.0f)));
    camera.yaw = glm::degrees(atan2(direction.z, direction.x));
}

std::optional<glm::vec3> ResolveLookTarget(
    entt::registry& registry,
    const CameraShotKeyframe& keyframe)
{
    if (keyframe.aimMode == CameraShotAimMode::LookAtPoint) {
        return keyframe.lookAtPoint;
    }

    if (keyframe.aimMode == CameraShotAimMode::LookAtEntity &&
        keyframe.lookAtEntity != entt::null &&
        registry.valid(keyframe.lookAtEntity) &&
        registry.all_of<Transform>(keyframe.lookAtEntity) &&
        !IsEntityDisabled(registry, keyframe.lookAtEntity)) {
        return registry.get<Transform>(keyframe.lookAtEntity).position;
    }

    return std::nullopt;
}

glm::quat ResolveAimRotation(
    entt::registry& registry,
    const CameraShotKeyframe& keyframe,
    const glm::vec3& cameraPosition,
    const glm::quat& fallbackRotation)
{
    auto target = ResolveLookTarget(registry, keyframe);
    if (!target.has_value()) {
        return glm::normalize(fallbackRotation);
    }

    const glm::vec3 direction = target.value() - cameraPosition;
    if (glm::dot(direction, direction) < 0.000001f) {
        return glm::normalize(fallbackRotation);
    }

    return glm::normalize(glm::quatLookAt(glm::normalize(direction), glm::vec3{ 0.0f, 1.0f, 0.0f }));
}

void ApplyKeyframe(
    entt::registry& registry,
    Transform& transform,
    Camera& camera,
    const CameraShotKeyframe& keyframe)
{
    transform.position = keyframe.position;
    transform.rotation = ResolveAimRotation(
        registry,
        keyframe,
        transform.position,
        keyframe.rotation);
    camera.fov = keyframe.fov;
    SyncCameraDirection(camera, transform);
}

} // namespace

void ApplyCameraShotAtCurrentTime(
    entt::registry& registry,
    Transform& transform,
    Camera& camera,
    const CinematicCameraShotComponent& shot)
{
    if (shot.keyframes.empty()) {
        return;
    }

    if (shot.keyframes.size() == 1) {
        ApplyKeyframe(registry, transform, camera, shot.keyframes.front());
        return;
    }

    if (shot.time <= 0.0f) {
        ApplyKeyframe(registry, transform, camera, shot.keyframes.front());
        return;
    }

    float segmentStartTime = 0.0f;
    const bool closedPath = IsClosedCameraShotPath(shot);
    for (std::size_t nextIndex = 1; nextIndex < shot.keyframes.size(); ++nextIndex) {
        const std::size_t previousIndex = nextIndex - 1;
        const auto& next = shot.keyframes[nextIndex];
        const auto& previous = shot.keyframes[previousIndex];
        const float segmentDuration = glm::max(0.0001f, next.duration);
        const float segmentEndTime = segmentStartTime + segmentDuration;

        if (shot.time <= segmentEndTime) {
            const float linearT = glm::clamp(
                (shot.time - segmentStartTime) / segmentDuration,
                0.0f,
                1.0f);
            const CameraShotInterpolationMode interpolationMode = next.interpolationMode;

            float segmentT = linearT;
            if (interpolationMode == CameraShotInterpolationMode::Smoothstep) {
                segmentT = SmoothStep(linearT);
            }

            if (interpolationMode == CameraShotInterpolationMode::CatmullRom) {
                const std::size_t firstIndex = PreviousControlIndex(shot, previousIndex, closedPath);
                const std::size_t lastIndex = NextControlIndex(shot, nextIndex, closedPath);

                transform.position = CatmullRom(
                    shot.keyframes[firstIndex].position,
                    previous.position,
                    next.position,
                    shot.keyframes[lastIndex].position,
                    linearT);
            }
            else {
                transform.position = glm::mix(previous.position, next.position, segmentT);
            }

            const glm::quat interpolatedRotation =
                glm::normalize(glm::slerp(previous.rotation, next.rotation, segmentT));
            transform.rotation = next.aimMode == CameraShotAimMode::UseRotation
                ? interpolatedRotation
                : ResolveAimRotation(registry, next, transform.position, interpolatedRotation);
            camera.fov = glm::mix(previous.fov, next.fov, segmentT);
            SyncCameraDirection(camera, transform);
            return;
        }

        segmentStartTime = segmentEndTime;
    }

    ApplyKeyframe(registry, transform, camera, shot.keyframes.back());
}

float ResolveCameraShotDuration(const CinematicCameraShotComponent& shot)
{
    float duration = 0.0f;
    for (std::size_t index = 1; index < shot.keyframes.size(); ++index) {
        duration += glm::max(0.0f, shot.keyframes[index].duration);
    }
    return duration;
}

CinematicCameraSystem::CinematicCameraSystem(entt::registry& registry, Engine::Core* core)
    : System(registry)
    , _core(core)
{
}

void CinematicCameraSystem::Update(float deltaTime)
{
    auto view = _registry.view<Camera, Transform, CinematicCameraShotComponent>(
        entt::exclude<DisabledEntityTag>);

    for (auto entity : view) {
        auto& camera = view.get<Camera>(entity);
        auto& transform = view.get<Transform>(entity);
        auto& shot = view.get<CinematicCameraShotComponent>(entity);

        if (!shot.playing) {
            continue;
        }

        const float duration = ResolveCameraShotDuration(shot);
        if (duration <= 0.0f) {
            ApplyCameraShotAtCurrentTime(_registry, transform, camera, shot);
            shot.playing = false;
            continue;
        }

        shot.time += deltaTime * glm::max(0.0f, shot.playbackSpeed);
        if (shot.time > duration) {
            if (shot.loop) {
                shot.time = std::fmod(shot.time, duration);
            }
            else {
                shot.time = duration;
                shot.playing = false;
            }
        }

        ApplyCameraShotAtCurrentTime(_registry, transform, camera, shot);
    }
}
