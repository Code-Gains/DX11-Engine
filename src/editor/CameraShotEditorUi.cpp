#include "CameraShotEditorUi.h"

#include "CinematicCameraSystem.h"
#include "EditorUiLayout.h"
#include "NameComponent.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <imgui.h>
#include <string>

namespace {

bool DrawEntityReferenceField(
    entt::registry& registry,
    const char* label,
    entt::entity& entity)
{
    bool changed = false;
    const int currentEntityId = entity == entt::null
        ? -1
        : static_cast<int>(entt::to_integral(entity));

    std::string displayValue = "None";
    if (entity != entt::null && registry.valid(entity)) {
        if (auto* name = registry.try_get<NameComponent>(entity)) {
            displayValue = name->name + " [" + std::to_string(currentEntityId) + "]";
        }
        else {
            displayValue = "Entity " + std::to_string(currentEntityId);
        }
    }
    else if (entity != entt::null) {
        displayValue = "Missing [" + std::to_string(currentEntityId) + "]";
    }

    ImGui::PushID(label);
    ImGui::TextUnformatted(label);
    ImGui::SameLine();

    const float clearButtonWidth = 54.0f;
    const float fieldWidth = std::max(
        80.0f,
        ImGui::GetContentRegionAvail().x - clearButtonWidth - ImGui::GetStyle().ItemSpacing.x);
    ImGui::SetNextItemWidth(fieldWidth);
    ImGui::Button(displayValue.c_str(), ImVec2(fieldWidth, 0.0f));

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("REGISTRY_ENTITY")) {
            const auto entityId = *static_cast<const uint32_t*>(payload->Data);
            const auto droppedEntity = static_cast<entt::entity>(entityId);
            if (droppedEntity != entt::null && registry.valid(droppedEntity)) {
                entity = droppedEntity;
                changed = true;
            }
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::SameLine();
    if (ImGui::Button("Clear")) {
        entity = entt::null;
        changed = true;
    }

    ImGui::PopID();
    return changed;
}

CameraShotKeyframe MakeKeyframe(
    const Transform& transform,
    const Camera& camera,
    float duration,
    float fov,
    CameraShotInterpolationMode interpolationMode,
    CameraShotAimMode aimMode = CameraShotAimMode::UseRotation,
    glm::vec3 lookAtPoint = glm::vec3{ 0.0f },
    entt::entity lookAtEntity = entt::null)
{
    return CameraShotKeyframe {
        duration,
        transform.position,
        transform.rotation,
        fov,
        interpolationMode,
        aimMode,
        lookAtPoint,
        lookAtEntity
    };
}

void NormalizeKeyframeDurations(CinematicCameraShotComponent& shot)
{
    if (!shot.keyframes.empty()) {
        shot.keyframes.front().duration = 0.0f;
    }
}

void DrawInterpolationCombo(
    const char* label,
    CameraShotInterpolationMode& interpolationMode)
{
    const char* interpolationModes[] = {
        "Linear",
        "Smoothstep",
        "Catmull-Rom"
    };
    int selectedMode = std::clamp(
        static_cast<int>(interpolationMode),
        0,
        IM_ARRAYSIZE(interpolationModes) - 1);
    if (ImGui::Combo(label, &selectedMode, interpolationModes, IM_ARRAYSIZE(interpolationModes))) {
        interpolationMode = static_cast<CameraShotInterpolationMode>(selectedMode);
    }
}

void DrawAimModeControls(entt::registry& registry, CameraShotKeyframe& keyframe)
{
    const char* aimModes[] = {
        "Use Rotation",
        "Look At Point",
        "Look At Entity"
    };
    int aimMode = std::clamp(
        static_cast<int>(keyframe.aimMode),
        0,
        IM_ARRAYSIZE(aimModes) - 1);
    if (ImGui::Combo("Aim Mode", &aimMode, aimModes, IM_ARRAYSIZE(aimModes))) {
        keyframe.aimMode = static_cast<CameraShotAimMode>(aimMode);
    }

    if (keyframe.aimMode == CameraShotAimMode::LookAtPoint) {
        ImGui::DragFloat3("Look At Point", &keyframe.lookAtPoint.x, 0.1f);
    }
    else if (keyframe.aimMode == CameraShotAimMode::LookAtEntity) {
        DrawEntityReferenceField(registry, "Look At Entity", keyframe.lookAtEntity);
    }
}

float ResolveKeyframeTime(const CinematicCameraShotComponent& shot, int keyframeIndex)
{
    float time = 0.0f;
    const int clampedIndex = std::clamp(
        keyframeIndex,
        0,
        static_cast<int>(shot.keyframes.size()) - 1);

    for (int index = 1; index <= clampedIndex; ++index) {
        time += glm::max(0.0f, shot.keyframes[static_cast<std::size_t>(index)].duration);
    }

    return time;
}

void RemoveKeyframe(CinematicCameraShotComponent& shot, int& selectedKeyframeIndex, int keyframeIndex)
{
    if (keyframeIndex < 0 || keyframeIndex >= static_cast<int>(shot.keyframes.size())) {
        return;
    }

    shot.keyframes.erase(shot.keyframes.begin() + keyframeIndex);
    selectedKeyframeIndex = std::min(
        selectedKeyframeIndex,
        static_cast<int>(shot.keyframes.size()) - 1);
}

void DuplicateKeyframe(CinematicCameraShotComponent& shot, int& selectedKeyframeIndex, int keyframeIndex)
{
    if (keyframeIndex < 0 || keyframeIndex >= static_cast<int>(shot.keyframes.size())) {
        return;
    }

    auto duplicate = shot.keyframes[static_cast<std::size_t>(keyframeIndex)];
    duplicate.duration = keyframeIndex == 0 ? 1.0f : duplicate.duration;
    shot.keyframes.insert(shot.keyframes.begin() + keyframeIndex + 1, duplicate);
    selectedKeyframeIndex = keyframeIndex + 1;
}

void MoveKeyframeTime(CinematicCameraShotComponent& shot, int keyframeIndex, float newTime)
{
    if (keyframeIndex <= 0 || keyframeIndex >= static_cast<int>(shot.keyframes.size())) {
        return;
    }

    constexpr float minimumSegmentDuration = 0.01f;
    const float previousTime = ResolveKeyframeTime(shot, keyframeIndex - 1);
    const bool hasNextKeyframe = keyframeIndex + 1 < static_cast<int>(shot.keyframes.size());
    const float nextTime = hasNextKeyframe
        ? ResolveKeyframeTime(shot, keyframeIndex + 1)
        : std::max(previousTime + minimumSegmentDuration, newTime);

    const float minTime = previousTime + minimumSegmentDuration;
    const float maxTime = hasNextKeyframe
        ? nextTime - minimumSegmentDuration
        : std::max(minTime, newTime);
    const float clampedTime = glm::clamp(newTime, minTime, maxTime);

    shot.keyframes[static_cast<std::size_t>(keyframeIndex)].duration =
        clampedTime - previousTime;

    if (hasNextKeyframe) {
        shot.keyframes[static_cast<std::size_t>(keyframeIndex + 1)].duration =
            nextTime - clampedTime;
    }
}

} // namespace

void CameraShotEditorUi::Draw(
    entt::registry& registry,
    Transform& transform,
    Camera& camera,
    CinematicCameraShotComponent& shot)
{
    shot.duration = ResolveDuration(shot);

    ImGui::SeparatorText("Camera Shot");
    ImGui::Text("Total Duration: %.2f", shot.duration);
    ImGui::PushItemWidth(EditorUi::CompactControlWidth());
    ImGui::DragFloat("Playback Time", &shot.time, 0.01f, 0.0f, shot.duration);
    ImGui::DragFloat("Playback Speed", &shot.playbackSpeed, 0.01f, 0.0f, 100.0f);
    DrawInterpolationCombo("Default Interpolation", shot.interpolationMode);
    ImGui::PopItemWidth();
    ImGui::Checkbox("Loop", &shot.loop);
    ImGui::Checkbox("Show Path", &shot.showPath);

    if (ImGui::Button("Preview From Start")) {
        shot.time = 0.0f;
        shot.playing = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Stop")) {
        shot.playing = false;
    }
    ImGui::SameLine();
    ImGui::TextDisabled(shot.playing ? "Playing" : "Stopped");

    DrawTimeline(registry, transform, camera, shot);

    if (ImGui::Button("Add Keyframe From Camera")) {
        const float keyframeDuration = shot.keyframes.empty() ? 0.0f : 1.0f;
        const float keyframeFov = shot.keyframes.empty() ? camera.fov : shot.keyframes.back().fov;
        const CameraShotAimMode keyframeAimMode = shot.keyframes.empty()
            ? CameraShotAimMode::UseRotation
            : shot.keyframes.back().aimMode;
        const glm::vec3 keyframeLookAtPoint = shot.keyframes.empty()
            ? glm::vec3{ 0.0f }
            : shot.keyframes.back().lookAtPoint;
        const entt::entity keyframeLookAtEntity = shot.keyframes.empty()
            ? entt::null
            : shot.keyframes.back().lookAtEntity;
        shot.keyframes.push_back(MakeKeyframe(
            transform,
            camera,
            keyframeDuration,
            keyframeFov,
            shot.interpolationMode,
            keyframeAimMode,
            keyframeLookAtPoint,
            keyframeLookAtEntity));
        _selectedKeyframeIndex = static_cast<int>(shot.keyframes.size()) - 1;
    }

    DrawPresets(registry, shot, camera, transform);

    ImGui::SeparatorText("Keyframes");
    if (shot.keyframes.empty()) {
        ImGui::TextDisabled("No keyframes yet.");
    }
    else {
        if (_selectedKeyframeIndex < 0 ||
            _selectedKeyframeIndex >= static_cast<int>(shot.keyframes.size())) {
            _selectedKeyframeIndex = 0;
        }

        ImGui::BeginChild("CameraShotKeyframeList", ImVec2(160.0f, 0.0f), true);
        float cumulativeTime = 0.0f;
        for (int index = 0; index < static_cast<int>(shot.keyframes.size()); ++index) {
            if (index > 0) {
                cumulativeTime += glm::max(0.0f, shot.keyframes[static_cast<std::size_t>(index)].duration);
            }

            char label[64] {};
            std::snprintf(label, sizeof(label), "%02d  t=%.2f", index + 1, cumulativeTime);
            if (ImGui::Selectable(label, _selectedKeyframeIndex == index)) {
                _selectedKeyframeIndex = index;
            }
        }
        ImGui::EndChild();

        ImGui::SameLine();
        ImGui::BeginGroup();

        auto& keyframe = shot.keyframes[static_cast<std::size_t>(_selectedKeyframeIndex)];
        ImGui::Text("Keyframe %d", _selectedKeyframeIndex + 1);
        ImGui::PushItemWidth(EditorUi::CompactControlWidth(320.0f));
        if (_selectedKeyframeIndex == 0) {
            ImGui::TextUnformatted("Start Keyframe");
        }
        else {
            ImGui::DragFloat("Incoming Duration", &keyframe.duration, 0.01f, 0.0f, 100000.0f);
        }

        ImGui::DragFloat3("Position", &keyframe.position.x, 0.1f);
        ImGui::DragFloat("FOV", &keyframe.fov, 0.1f, 1.0f, 179.0f);
        DrawInterpolationCombo("Incoming Interpolation", keyframe.interpolationMode);
        DrawAimModeControls(registry, keyframe);
        ImGui::PopItemWidth();

        if (ImGui::Button("Update From Camera")) {
            keyframe = MakeKeyframe(
                transform,
                camera,
                keyframe.duration,
                camera.fov,
                keyframe.interpolationMode,
                keyframe.aimMode,
                keyframe.lookAtPoint,
                keyframe.lookAtEntity);
        }
        ImGui::SameLine();
        if (ImGui::Button("Jump To")) {
            shot.time = ResolveKeyframeTime(shot, _selectedKeyframeIndex);
            ApplyCameraShotAtCurrentTime(registry, transform, camera, shot);
        }

        if (ImGui::Button("Duplicate")) {
            DuplicateKeyframe(shot, _selectedKeyframeIndex, _selectedKeyframeIndex);
        }
        ImGui::SameLine();
        if (ImGui::Button("Move Up") && _selectedKeyframeIndex > 0) {
            std::swap(
                shot.keyframes[static_cast<std::size_t>(_selectedKeyframeIndex)],
                shot.keyframes[static_cast<std::size_t>(_selectedKeyframeIndex - 1)]);
            --_selectedKeyframeIndex;
        }
        ImGui::SameLine();
        if (ImGui::Button("Move Down") &&
            _selectedKeyframeIndex + 1 < static_cast<int>(shot.keyframes.size())) {
            std::swap(
                shot.keyframes[static_cast<std::size_t>(_selectedKeyframeIndex)],
                shot.keyframes[static_cast<std::size_t>(_selectedKeyframeIndex + 1)]);
            ++_selectedKeyframeIndex;
        }

        if (ImGui::Button("Remove") && !shot.keyframes.empty()) {
            RemoveKeyframe(shot, _selectedKeyframeIndex, _selectedKeyframeIndex);
        }

        ImGui::EndGroup();
    }

    NormalizeKeyframeDurations(shot);
    shot.duration = ResolveDuration(shot);
}

float CameraShotEditorUi::ResolveDuration(const CinematicCameraShotComponent& shot)
{
    float duration = 0.0f;
    for (std::size_t index = 1; index < shot.keyframes.size(); ++index) {
        duration += glm::max(0.0f, shot.keyframes[index].duration);
    }

    return duration;
}

void CameraShotEditorUi::DrawTimeline(
    entt::registry& registry,
    Transform& transform,
    Camera& camera,
    CinematicCameraShotComponent& shot)
{
    ImGui::SeparatorText("Timeline");

    const float timelineDuration = glm::max(shot.duration, 0.0001f);
    const float availableWidth = ImGui::GetContentRegionAvail().x;
    const ImVec2 canvasSize{ glm::max(360.0f, availableWidth), 96.0f };
    const ImVec2 canvasPosition = ImGui::GetCursorScreenPos();

    ImGui::InvisibleButton("CameraShotTimelineCanvas", canvasSize);
    const bool hovered = ImGui::IsItemHovered();
    const bool active = ImGui::IsItemActive();

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    const ImU32 backgroundColor = IM_COL32(32, 34, 38, 255);
    const ImU32 borderColor = IM_COL32(82, 86, 96, 255);
    const ImU32 trackColor = IM_COL32(93, 97, 108, 255);
    const ImU32 segmentColorA = IM_COL32(48, 54, 65, 255);
    const ImU32 segmentColorB = IM_COL32(55, 61, 73, 255);
    const ImU32 keyframeColor = IM_COL32(245, 198, 82, 255);
    const ImU32 selectedKeyframeColor = IM_COL32(105, 185, 255, 255);
    const ImU32 playheadColor = IM_COL32(255, 255, 255, 255);

    const float paddingX = 18.0f;
    const float trackStartX = canvasPosition.x + paddingX;
    const float trackEndX = canvasPosition.x + canvasSize.x - paddingX;
    const float trackWidth = glm::max(1.0f, trackEndX - trackStartX);
    const float trackY = canvasPosition.y + 48.0f;
    const float trackHeight = 24.0f;

    auto timeToX = [&](float time) {
        return trackStartX + glm::clamp(time / timelineDuration, 0.0f, 1.0f) * trackWidth;
    };
    auto xToTime = [&](float x) {
        const float normalized = glm::clamp((x - trackStartX) / trackWidth, 0.0f, 1.0f);
        return normalized * timelineDuration;
    };

    drawList->AddRectFilled(
        canvasPosition,
        ImVec2{ canvasPosition.x + canvasSize.x, canvasPosition.y + canvasSize.y },
        backgroundColor,
        4.0f);
    drawList->AddRect(
        canvasPosition,
        ImVec2{ canvasPosition.x + canvasSize.x, canvasPosition.y + canvasSize.y },
        borderColor,
        4.0f);

    char durationLabel[64] {};
    std::snprintf(durationLabel, sizeof(durationLabel), "0.00s");
    drawList->AddText(ImVec2{ trackStartX, canvasPosition.y + 10.0f }, borderColor, durationLabel);
    std::snprintf(durationLabel, sizeof(durationLabel), "%.2fs", shot.duration);
    const ImVec2 durationTextSize = ImGui::CalcTextSize(durationLabel);
    drawList->AddText(
        ImVec2{ trackEndX - durationTextSize.x, canvasPosition.y + 10.0f },
        borderColor,
        durationLabel);

    drawList->AddLine(
        ImVec2{ trackStartX, trackY + trackHeight * 0.5f },
        ImVec2{ trackEndX, trackY + trackHeight * 0.5f },
        trackColor,
        2.0f);

    float segmentStartTime = 0.0f;
    for (int index = 1; index < static_cast<int>(shot.keyframes.size()); ++index) {
        const float segmentEndTime =
            segmentStartTime + glm::max(0.0f, shot.keyframes[static_cast<std::size_t>(index)].duration);
        const float x0 = timeToX(segmentStartTime);
        const float x1 = timeToX(segmentEndTime);
        drawList->AddRectFilled(
            ImVec2{ x0, trackY },
            ImVec2{ x1, trackY + trackHeight },
            index % 2 == 0 ? segmentColorA : segmentColorB,
            2.0f);

        if (x1 - x0 > 44.0f) {
            char segmentLabel[32] {};
            std::snprintf(
                segmentLabel,
                sizeof(segmentLabel),
                "%.2fs",
                glm::max(0.0f, shot.keyframes[static_cast<std::size_t>(index)].duration));
            const ImVec2 labelSize = ImGui::CalcTextSize(segmentLabel);
            drawList->AddText(
                ImVec2{ x0 + (x1 - x0 - labelSize.x) * 0.5f, trackY + 4.0f },
                borderColor,
                segmentLabel);
        }

        segmentStartTime = segmentEndTime;
    }

    const ImVec2 mousePosition = ImGui::GetIO().MousePos;
    const bool mouseInScrubBand =
        mousePosition.y >= canvasPosition.y &&
        mousePosition.y <= canvasPosition.y + 34.0f;
    const bool mouseInKeyframeBand =
        mousePosition.y >= trackY - 12.0f &&
        mousePosition.y <= trackY + trackHeight + 30.0f;

    auto findClosestKeyframe = [&](float mouseX, float maxDistance) {
        int closestKeyframeIndex = -1;
        float closestDistance = maxDistance;

        for (int index = 0; index < static_cast<int>(shot.keyframes.size()); ++index) {
            const float keyframeX = timeToX(ResolveKeyframeTime(shot, index));
            const float distance = std::abs(mouseX - keyframeX);
            if (distance < closestDistance) {
                closestDistance = distance;
                closestKeyframeIndex = index;
            }
        }

        return closestKeyframeIndex;
    };

    if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right) && mouseInKeyframeBand) {
        _timelineContextKeyframeIndex = findClosestKeyframe(mousePosition.x, 10.0f);
        if (_timelineContextKeyframeIndex >= 0) {
            _selectedKeyframeIndex = _timelineContextKeyframeIndex;
            ImGui::OpenPopup("CameraShotTimelineKeyframeMenu");
        }
    }

    if (ImGui::BeginPopup("CameraShotTimelineKeyframeMenu")) {
        if (_timelineContextKeyframeIndex >= 0 &&
            _timelineContextKeyframeIndex < static_cast<int>(shot.keyframes.size())) {
            ImGui::Text("Keyframe %d", _timelineContextKeyframeIndex + 1);
            ImGui::Separator();
            if (ImGui::MenuItem("Duplicate")) {
                DuplicateKeyframe(shot, _selectedKeyframeIndex, _timelineContextKeyframeIndex);
                _timelineContextKeyframeIndex = _selectedKeyframeIndex;
            }
            if (ImGui::MenuItem("Delete")) {
                RemoveKeyframe(shot, _selectedKeyframeIndex, _timelineContextKeyframeIndex);
                _timelineContextKeyframeIndex = -1;
            }
        }
        ImGui::EndPopup();
    }

    if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        const int closestKeyframeIndex = mouseInKeyframeBand
            ? findClosestKeyframe(mousePosition.x, 10.0f)
            : -1;

        if (closestKeyframeIndex >= 0) {
            _selectedKeyframeIndex = closestKeyframeIndex;
            _draggedKeyframeIndex = closestKeyframeIndex;
            _scrubbingTimeline = false;
            shot.time = ResolveKeyframeTime(shot, closestKeyframeIndex);
            ApplyCameraShotAtCurrentTime(registry, transform, camera, shot);
        }
        else if (mouseInScrubBand) {
            _draggedKeyframeIndex = -1;
            _scrubbingTimeline = true;
            shot.time = xToTime(mousePosition.x);
            ApplyCameraShotAtCurrentTime(registry, transform, camera, shot);
        }
    }

    if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
        _draggedKeyframeIndex = -1;
        _scrubbingTimeline = false;
    }

    if (active && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        if (_draggedKeyframeIndex > 0) {
            MoveKeyframeTime(shot, _draggedKeyframeIndex, xToTime(mousePosition.x));
            shot.duration = ResolveDuration(shot);
            shot.time = ResolveKeyframeTime(shot, _draggedKeyframeIndex);
            ApplyCameraShotAtCurrentTime(registry, transform, camera, shot);
        }
        else if (_scrubbingTimeline) {
            shot.time = xToTime(mousePosition.x);
            ApplyCameraShotAtCurrentTime(registry, transform, camera, shot);
        }
    }

    for (int index = 0; index < static_cast<int>(shot.keyframes.size()); ++index) {
        const float keyframeTime = ResolveKeyframeTime(shot, index);
        const float x = timeToX(keyframeTime);
        const bool selected = _selectedKeyframeIndex == index;
        const ImU32 color = selected ? selectedKeyframeColor : keyframeColor;

        drawList->AddCircleFilled(ImVec2{ x, trackY + trackHeight * 0.5f }, selected ? 7.0f : 5.0f, color);
        drawList->AddLine(
            ImVec2{ x, trackY - 8.0f },
            ImVec2{ x, trackY + trackHeight + 8.0f },
            color,
            selected ? 2.0f : 1.0f);

        char keyframeLabel[16] {};
        std::snprintf(keyframeLabel, sizeof(keyframeLabel), "%d", index + 1);
        const ImVec2 labelSize = ImGui::CalcTextSize(keyframeLabel);
        drawList->AddText(
            ImVec2{ x - labelSize.x * 0.5f, trackY + trackHeight + 12.0f },
            color,
            keyframeLabel);
    }

    shot.time = glm::clamp(shot.time, 0.0f, shot.duration);
    const float playheadX = timeToX(shot.time);
    drawList->AddLine(
        ImVec2{ playheadX, canvasPosition.y + 8.0f },
        ImVec2{ playheadX, canvasPosition.y + canvasSize.y - 8.0f },
        playheadColor,
        2.0f);
    drawList->AddTriangleFilled(
        ImVec2{ playheadX, canvasPosition.y + 31.0f },
        ImVec2{ playheadX - 5.0f, canvasPosition.y + 22.0f },
        ImVec2{ playheadX + 5.0f, canvasPosition.y + 22.0f },
        playheadColor);

    char playheadLabel[64] {};
    std::snprintf(playheadLabel, sizeof(playheadLabel), "%.2fs", shot.time);
    drawList->AddText(
        ImVec2{ glm::min(playheadX + 8.0f, trackEndX - 48.0f), canvasPosition.y + 72.0f },
        playheadColor,
        playheadLabel);
}

void CameraShotEditorUi::DrawPresets(
    entt::registry& registry,
    CinematicCameraShotComponent& shot,
    const Camera& camera,
    const Transform& cameraTransform)
{
    if (!ImGui::TreeNode("Presets##CinematicShotPresets")) {
        return;
    }

    DrawEntityReferenceField(registry, "Target", _orbitTargetEntity);
    ImGui::PushItemWidth(EditorUi::CompactControlWidth());
    ImGui::DragFloat("Radius##OrbitPresetRadius", &_orbitRadius, 0.1f, 0.01f, 100000.0f);
    ImGui::DragFloat("Height##OrbitPresetHeight", &_orbitHeight, 0.1f, -100000.0f, 100000.0f);
    ImGui::DragFloat("Duration##OrbitPresetDuration", &_orbitDuration, 0.05f, 0.01f, 100000.0f);
    ImGui::DragFloat("Angle##OrbitPresetAngle", &_orbitAngleDegrees, 1.0f, -100000.0f, 100000.0f);
    ImGui::InputInt("Keyframes##OrbitPresetKeyframes", &_orbitKeyframeCount);
    ImGui::PopItemWidth();
    _orbitKeyframeCount = std::clamp(_orbitKeyframeCount, 4, 128);
    ImGui::Checkbox("Clockwise##OrbitPresetClockwise", &_orbitClockwise);

    const bool canCreate =
        _orbitTargetEntity != entt::null &&
        registry.valid(_orbitTargetEntity) &&
        registry.all_of<Transform>(_orbitTargetEntity);

    if (!canCreate) {
        ImGui::BeginDisabled();
    }

    if (ImGui::Button("Create Orbit##CinematicShotCreateOrbit")) {
        CreateOrbitPreset(registry, shot, camera, cameraTransform);
    }

    if (!canCreate) {
        ImGui::EndDisabled();
        ImGui::TextDisabled("Drop an entity with Transform as target.");
    }

    ImGui::TreePop();
}

void CameraShotEditorUi::CreateOrbitPreset(
    entt::registry& registry,
    CinematicCameraShotComponent& shot,
    const Camera& camera,
    const Transform& cameraTransform)
{
    if (_orbitTargetEntity == entt::null ||
        !registry.valid(_orbitTargetEntity) ||
        !registry.all_of<Transform>(_orbitTargetEntity)) {
        return;
    }

    const auto& targetTransform = registry.get<Transform>(_orbitTargetEntity);
    const glm::vec3 targetPosition = targetTransform.position;
    const float radius = glm::max(0.01f, _orbitRadius);
    const int keyframeCount = std::max(4, _orbitKeyframeCount);
    const float segmentDuration = glm::max(0.01f, _orbitDuration) /
        static_cast<float>(keyframeCount - 1);
    const float direction = _orbitClockwise ? -1.0f : 1.0f;
    const float totalAngle = glm::radians(_orbitAngleDegrees) * direction;
    const float normalizedOrbitAngle = std::fmod(std::abs(_orbitAngleDegrees), 360.0f);
    const bool fullOrbit =
        normalizedOrbitAngle < 0.01f ||
        std::abs(normalizedOrbitAngle - 360.0f) < 0.01f;
    glm::vec3 startOffset = cameraTransform.position - targetPosition;
    startOffset.y = 0.0f;
    float startAngle = std::atan2(camera.direction.x, camera.direction.z);
    if (glm::dot(startOffset, startOffset) > 0.000001f) {
        startAngle = std::atan2(startOffset.x, startOffset.z);
    }

    shot.keyframes.clear();
    shot.keyframes.reserve(static_cast<std::size_t>(keyframeCount));

    for (int index = 0; index < keyframeCount; ++index) {
        const float t = static_cast<float>(index) / static_cast<float>(keyframeCount - 1);
        const float angle = startAngle + totalAngle * t;
        const glm::vec3 position = targetPosition + glm::vec3{
            std::sin(angle) * radius,
            _orbitHeight,
            std::cos(angle) * radius
        };
        const glm::vec3 directionToTarget = targetPosition - position;
        const glm::quat rotation = glm::dot(directionToTarget, directionToTarget) < 0.000001f
            ? glm::quat{ 1.0f, 0.0f, 0.0f, 0.0f }
            : glm::quatLookAt(glm::normalize(directionToTarget), glm::vec3{ 0.0f, 1.0f, 0.0f });

        CameraShotKeyframe keyframe;
        keyframe.duration = index == 0 ? 0.0f : segmentDuration;
        keyframe.position = position;
        keyframe.rotation = glm::normalize(rotation);
        keyframe.fov = camera.fov;
        keyframe.interpolationMode = fullOrbit
            ? CameraShotInterpolationMode::CatmullRom
            : shot.interpolationMode;
        keyframe.aimMode = CameraShotAimMode::LookAtEntity;
        keyframe.lookAtPoint = targetPosition;
        keyframe.lookAtEntity = _orbitTargetEntity;
        shot.keyframes.push_back(keyframe);
    }

    NormalizeKeyframeDurations(shot);
    shot.duration = ResolveDuration(shot);
    shot.time = 0.0f;
    shot.loop = fullOrbit;
    shot.showPath = true;
    _selectedKeyframeIndex = 0;
}
