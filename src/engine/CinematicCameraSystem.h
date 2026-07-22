#pragma once

#include "Camera.h"
#include "System.h"
#include "Transform.h"

namespace Engine {
class Core;
}

void ApplyCameraShotAtCurrentTime(
    entt::registry& registry,
    Transform& transform,
    Camera& camera,
    const CinematicCameraShotComponent& shot);

float ResolveCameraShotDuration(const CinematicCameraShotComponent& shot);

class CinematicCameraSystem : public System {
public:
    CinematicCameraSystem(entt::registry& registry, Engine::Core* core);

    void Update(float deltaTime) override;

private:
    Engine::Core* _core = nullptr;
};
