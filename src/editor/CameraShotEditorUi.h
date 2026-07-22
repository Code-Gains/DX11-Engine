#pragma once

#include "Camera.h"
#include "Transform.h"

#include <entt/entt.hpp>

class CameraShotEditorUi {
public:
    void Draw(
        entt::registry& registry,
        Transform& transform,
        Camera& camera,
        CinematicCameraShotComponent& shot);

    static float ResolveDuration(const CinematicCameraShotComponent& shot);

private:
    int _selectedKeyframeIndex = -1;
    int _draggedKeyframeIndex = -1;
    int _timelineContextKeyframeIndex = -1;
    bool _scrubbingTimeline = false;
    entt::entity _orbitTargetEntity{ entt::null };
    float _orbitRadius = 10.0f;
    float _orbitHeight = 3.0f;
    float _orbitDuration = 5.0f;
    float _orbitAngleDegrees = 360.0f;
    int _orbitKeyframeCount = 9;
    bool _orbitClockwise = false;

    void DrawPresets(
        entt::registry& registry,
        CinematicCameraShotComponent& shot,
        const Camera& camera,
        const Transform& cameraTransform);

    void DrawTimeline(
        entt::registry& registry,
        Transform& transform,
        Camera& camera,
        CinematicCameraShotComponent& shot);

    void CreateOrbitPreset(
        entt::registry& registry,
        CinematicCameraShotComponent& shot,
        const Camera& camera,
        const Transform& cameraTransform);
};
