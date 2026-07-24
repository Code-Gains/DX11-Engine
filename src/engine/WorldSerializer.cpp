#include "WorldSerializer.h"

#include "Camera.h"
#include "Core.h"
#include "GravityComponents.h"
#include "HierarchyComponent.h"
#include "EntityState.h"
#include "JoltPhysicsComponents.h"
#include "Log.h"
#include "MeshComponent.h"
#include "NameComponent.h"
#include "SunlightComponent.h"
#include "Transform.h"

#include <algorithm>
#include <exception>
#include <fstream>
#include <glm/trigonometric.hpp>
#include <unordered_map>

namespace Engine {
namespace {

nlohmann::json Vec3ToJson(const glm::vec3& value)
{
    return {
        value.x,
        value.y,
        value.z
    };
}

glm::vec3 Vec3FromJson(const nlohmann::json& value)
{
    return {
        value.at(0).get<float>(),
        value.at(1).get<float>(),
        value.at(2).get<float>()
    };
}

nlohmann::json Vec4ToJson(const glm::vec4& value)
{
    return {
        value.x,
        value.y,
        value.z,
        value.w
    };
}

glm::vec4 Vec4FromJson(const nlohmann::json& value)
{
    return {
        value.at(0).get<float>(),
        value.at(1).get<float>(),
        value.at(2).get<float>(),
        value.at(3).get<float>()
    };
}

nlohmann::json QuatToJson(const glm::quat& value)
{
    return {
        value.w,
        value.x,
        value.y,
        value.z
    };
}

glm::quat QuatFromJson(const nlohmann::json& value)
{
    return {
        value.at(0).get<float>(),
        value.at(1).get<float>(),
        value.at(2).get<float>(),
        value.at(3).get<float>()
    };
}

const char* CameraShotInterpolationModeToString(CameraShotInterpolationMode mode)
{
    switch (mode) {
    case CameraShotInterpolationMode::Linear:
        return "Linear";
    case CameraShotInterpolationMode::Smoothstep:
        return "Smoothstep";
    case CameraShotInterpolationMode::CatmullRom:
        return "CatmullRom";
    default:
        return "CatmullRom";
    }
}

CameraShotInterpolationMode CameraShotInterpolationModeFromString(const std::string& value)
{
    if (value == "Linear") {
        return CameraShotInterpolationMode::Linear;
    }

    if (value == "Smoothstep") {
        return CameraShotInterpolationMode::Smoothstep;
    }

    return CameraShotInterpolationMode::CatmullRom;
}

const char* CameraShotAimModeToString(CameraShotAimMode mode)
{
    switch (mode) {
    case CameraShotAimMode::UseRotation:
        return "UseRotation";
    case CameraShotAimMode::LookAtPoint:
        return "LookAtPoint";
    case CameraShotAimMode::LookAtEntity:
        return "LookAtEntity";
    default:
        return "UseRotation";
    }
}

CameraShotAimMode CameraShotAimModeFromString(const std::string& value)
{
    if (value == "LookAtPoint") {
        return CameraShotAimMode::LookAtPoint;
    }

    if (value == "LookAtEntity") {
        return CameraShotAimMode::LookAtEntity;
    }

    return CameraShotAimMode::UseRotation;
}

const char* JoltColliderShapeToString(JoltColliderShape shape)
{
    switch (shape) {
    case JoltColliderShape::Sphere:
        return "Sphere";
    case JoltColliderShape::Box:
        return "Box";
    case JoltColliderShape::Capsule:
        return "Capsule";
    default:
        return "Sphere";
    }
}

JoltColliderShape JoltColliderShapeFromString(const std::string& value)
{
    if (value == "Box") {
        return JoltColliderShape::Box;
    }

    if (value == "Capsule") {
        return JoltColliderShape::Capsule;
    }

    return JoltColliderShape::Sphere;
}

const char* JoltBodyMotionToString(JoltBodyMotion motion)
{
    switch (motion) {
    case JoltBodyMotion::Static:
        return "Static";
    case JoltBodyMotion::Kinematic:
        return "Kinematic";
    default:
        return "Kinematic";
    }
}

JoltBodyMotion JoltBodyMotionFromString(const std::string& value)
{
    if (value == "Static") {
        return JoltBodyMotion::Static;
    }

    return JoltBodyMotion::Kinematic;
}

float ResolveCameraShotDuration(const CinematicCameraShotComponent& shot)
{
    float duration = 0.0f;
    for (std::size_t index = 1; index < shot.keyframes.size(); ++index) {
        duration += glm::max(0.0f, shot.keyframes[index].duration);
    }

    return duration;
}

entt::entity EntityFromSerializedId(uint64_t id)
{
    return static_cast<entt::entity>(id);
}

nlohmann::json WorldToJson(const Serialization::SerializedWorld& world)
{
    nlohmann::json entities = nlohmann::json::array();

    for (const auto& entity : world.entities) {
        nlohmann::json components = nlohmann::json::array();

        for (const auto& component : entity.components) {
            components.push_back({
                {"type", component.type},
                {"data", component.data}
            });
        }

        entities.push_back({
            {"id", entity.id},
            {"components", components}
        });
    }

    nlohmann::json root = {
        {"version", world.version},
        {"entities", entities}
    };

    if (!world.editor.empty()) {
        root["editor"] = world.editor;
    }

    return root;
}

nlohmann::json PrefabToJson(const Serialization::SerializedEntity& entity)
{
    nlohmann::json components = nlohmann::json::array();

    for (const auto& component : entity.components) {
        components.push_back({
            {"type", component.type},
            {"data", component.data}
        });
    }

    return {
        {"version", Serialization::CurrentWorldVersion},
        {"entity", {
            {"id", entity.id},
            {"components", components}
        }}
    };
}

Serialization::SerializedWorld WorldFromJson(const nlohmann::json& root)
{
    Serialization::SerializedWorld world;
    world.version = root.at("version").get<uint32_t>();
    world.editor = root.value("editor", nlohmann::json::object());

    for (const auto& entityJson : root.at("entities")) {
        Serialization::SerializedEntity entity;
        entity.id = entityJson.at("id").get<uint64_t>();

        for (const auto& componentJson : entityJson.at("components")) {
            entity.components.push_back({
                componentJson.at("type").get<std::string>(),
                componentJson.at("data")
            });
        }

        world.entities.push_back(std::move(entity));
    }

    return world;
}

Serialization::SerializedEntity EntityFromJson(const nlohmann::json& entityJson)
{
    Serialization::SerializedEntity entity;
    entity.id = entityJson.value("id", uint64_t{ 0 });

    for (const auto& componentJson : entityJson.at("components")) {
        entity.components.push_back({
            componentJson.at("type").get<std::string>(),
            componentJson.at("data")
        });
    }

    return entity;
}

Serialization::SerializedEntity PrefabFromJson(const nlohmann::json& root)
{
    const uint32_t version = root.at("version").get<uint32_t>();
    if (version != Serialization::CurrentWorldVersion) {
        throw std::runtime_error("Unsupported prefab version: " + std::to_string(version));
    }

    if (root.contains("entity")) {
        return EntityFromJson(root.at("entity"));
    }

    auto world = WorldFromJson(root);
    if (world.entities.size() != 1) {
        throw std::runtime_error("Prefab world must contain exactly one entity.");
    }

    return std::move(world.entities.front());
}

nlohmann::json CaptureEditorState(Core& core)
{
    auto& registry = core.GetRegistry();
    auto cameraView = registry.view<Camera, Transform, ActiveCameraTag, CoreOwnedTag>();

    if (cameraView.begin() == cameraView.end()) {
        return nlohmann::json::object();
    }

    const auto cameraEntity = *cameraView.begin();
    const auto& transform = cameraView.get<Transform>(cameraEntity);

    return {
        {"camera", {
            {"position", Vec3ToJson(transform.position)},
            {"rotation", QuatToJson(transform.rotation)}
        }}
    };
}

void ApplyEditorState(Core& core, const nlohmann::json& editorState)
{
    if (!editorState.contains("camera")) {
        return;
    }

    auto& registry = core.GetRegistry();
    auto cameraView = registry.view<Camera, Transform, ActiveCameraTag, CoreOwnedTag>();

    if (cameraView.begin() == cameraView.end()) {
        return;
    }

    const auto cameraEntity = *cameraView.begin();
    auto& camera = cameraView.get<Camera>(cameraEntity);
    auto& transform = cameraView.get<Transform>(cameraEntity);
    const auto& cameraState = editorState.at("camera");

    transform.position = Vec3FromJson(cameraState.at("position"));
    transform.rotation = QuatFromJson(cameraState.at("rotation"));

    camera.direction = glm::normalize(transform.rotation * glm::vec3{ 0.0f, 0.0f, -1.0f });
    camera.pitch = glm::degrees(asin(glm::clamp(camera.direction.y, -1.0f, 1.0f)));
    camera.yaw = glm::degrees(atan2(camera.direction.z, camera.direction.x));
}

} // namespace

std::vector<entt::entity> ComponentSerializerRegistry::SerializableEntities(
    entt::registry& registry
) const
{
    std::unordered_set<entt::entity> uniqueEntities;

    for (const auto& serializer : _serializers) {
        serializer.collectEntities(registry, uniqueEntities);
    }

    return {
        uniqueEntities.begin(),
        uniqueEntities.end()
    };
}

std::vector<Serialization::SerializedComponent> ComponentSerializerRegistry::SaveComponents(
    Core& core,
    entt::registry& registry,
    entt::entity entity
) const
{
    std::vector<Serialization::SerializedComponent> components;

    for (const auto& serializer : _serializers) {
        auto component = serializer.save(core, registry, entity);
        if (component.has_value()) {
            components.push_back(std::move(component.value()));
        }
    }

    return components;
}

void ComponentSerializerRegistry::LoadComponent(
    Core& core,
    entt::registry& registry,
    entt::entity entity,
    const Serialization::SerializedComponent& component
) const
{
    for (const auto& serializer : _serializers) {
        if (serializer.type == component.type) {
            try {
                serializer.load(core, registry, entity, component);
            } catch (const std::exception& exception) {
                ENGINE_LOG_ERROR(
                    "Failed to load component " +
                    component.type +
                    ": " +
                    exception.what()
                );
            }
            return;
        }
    }

    ENGINE_LOG_WARN("No component serializer registered for: " + component.type);
}

WorldSerializer::WorldSerializer()
{
    RegisterDefaultComponentSerializers();
}

bool WorldSerializer::SaveWorld(Core& core, const std::filesystem::path& path) const
{
    std::ofstream output(path);
    if (!output.is_open()) {
        ENGINE_LOG_ERROR("Failed to open world file for saving: " + path.string());
        return false;
    }

    auto world = CaptureWorld(core);
    output << WorldToJson(world).dump(4);

    return true;
}

bool WorldSerializer::LoadWorld(Core& core, const std::filesystem::path& path) const
{
    std::ifstream input(path);
    if (!input.is_open()) {
        ENGINE_LOG_ERROR("Failed to open world file for loading: " + path.string());
        return false;
    }

    nlohmann::json root;
    try {
        input >> root;
    } catch (const std::exception& exception) {
        ENGINE_LOG_ERROR("Failed to parse world file: " + std::string(exception.what()));
        return false;
    }

    Serialization::SerializedWorld world;
    try {
        world = WorldFromJson(root);
    } catch (const std::exception& exception) {
        ENGINE_LOG_ERROR("Invalid world file: " + std::string(exception.what()));
        return false;
    }

    if (world.version != Serialization::CurrentWorldVersion) {
        ENGINE_LOG_ERROR("Unsupported world version: " + std::to_string(world.version));
        return false;
    }

    ApplyWorld(core, world);
    return true;
}

bool WorldSerializer::SavePrefab(Core& core, entt::entity entity, const std::filesystem::path& path) const
{
    auto serializedEntity = CaptureEntity(core, entity);
    if (!serializedEntity.has_value()) {
        ENGINE_LOG_ERROR("Failed to capture prefab entity.");
        return false;
    }

    std::ofstream output(path);
    if (!output.is_open()) {
        ENGINE_LOG_ERROR("Failed to open prefab file for saving: " + path.string());
        return false;
    }

    output << PrefabToJson(serializedEntity.value()).dump(4);
    return true;
}

std::optional<entt::entity> WorldSerializer::InstantiatePrefab(
    Core& core,
    const std::filesystem::path& path) const
{
    std::ifstream input(path);
    if (!input.is_open()) {
        ENGINE_LOG_ERROR("Failed to open prefab file for loading: " + path.string());
        return std::nullopt;
    }

    nlohmann::json root;
    try {
        input >> root;
    } catch (const std::exception& exception) {
        ENGINE_LOG_ERROR("Failed to parse prefab file: " + std::string(exception.what()));
        return std::nullopt;
    }

    Serialization::SerializedEntity entity;
    try {
        entity = PrefabFromJson(root);
    } catch (const std::exception& exception) {
        ENGINE_LOG_ERROR("Invalid prefab file: " + std::string(exception.what()));
        return std::nullopt;
    }

    return ApplyEntity(core, entity);
}

std::optional<Serialization::SerializedEntity> WorldSerializer::SerializeEntity(
    Core& core,
    entt::entity entity) const
{
    return CaptureEntity(core, entity);
}

entt::entity WorldSerializer::InstantiateEntity(
    Core& core,
    const Serialization::SerializedEntity& entity) const
{
    return ApplyEntity(core, entity);
}

nlohmann::json WorldSerializer::SaveWorldToJson(Core& core) const
{
    return WorldToJson(CaptureWorld(core));
}

bool WorldSerializer::LoadWorldFromJson(Core& core, const nlohmann::json& root) const
{
    Serialization::SerializedWorld world;
    try {
        world = WorldFromJson(root);
    } catch (const std::exception& exception) {
        ENGINE_LOG_ERROR("Invalid world JSON: " + std::string(exception.what()));
        return false;
    }

    if (world.version != Serialization::CurrentWorldVersion) {
        ENGINE_LOG_ERROR("Unsupported world version: " + std::to_string(world.version));
        return false;
    }

    ApplyWorld(core, world);
    return true;
}

ComponentSerializerRegistry& WorldSerializer::ComponentSerializers()
{
    return _componentSerializers;
}

const ComponentSerializerRegistry& WorldSerializer::ComponentSerializers() const
{
    return _componentSerializers;
}

Serialization::SerializedWorld WorldSerializer::CaptureWorld(Core& core) const
{
    Serialization::SerializedWorld world;
    auto& registry = core.GetRegistry();

    world.editor = CaptureEditorState(core);

    for (auto entity : _componentSerializers.SerializableEntities(registry)) {
        if (registry.all_of<CoreOwnedTag>(entity)) {
            continue;
        }

        auto serializedEntity = CaptureEntity(core, entity);
        if (serializedEntity.has_value()) {
            world.entities.push_back(std::move(serializedEntity.value()));
        }
    }

    return world;
}

void WorldSerializer::ApplyWorld(Core& core, const Serialization::SerializedWorld& world) const
{
    auto& registry = core.GetRegistry();
    auto entities = _componentSerializers.SerializableEntities(registry);

    for (auto entity : entities) {
        if (!registry.valid(entity) || registry.all_of<CoreOwnedTag>(entity)) {
            continue;
        }

        registry.destroy(entity);
    }

    for (const auto& serializedEntity : world.entities) {
        ApplyEntity(core, serializedEntity, true);
    }

    ApplyEditorState(core, world.editor);
}

std::optional<Serialization::SerializedEntity> WorldSerializer::CaptureEntity(
    Core& core,
    entt::entity entity) const
{
    auto& registry = core.GetRegistry();
    if (!registry.valid(entity) || registry.all_of<CoreOwnedTag>(entity)) {
        return std::nullopt;
    }

    auto components = _componentSerializers.SaveComponents(core, registry, entity);
    if (components.empty()) {
        return std::nullopt;
    }

    return Serialization::SerializedEntity {
        static_cast<uint64_t>(entt::to_integral(entity)),
        std::move(components)
    };
}

entt::entity WorldSerializer::ApplyEntity(
    Core& core,
    const Serialization::SerializedEntity& serializedEntity,
    bool preserveSerializedId) const
{
    auto& registry = core.GetRegistry();
    entt::entity entity = entt::null;

    if (preserveSerializedId && serializedEntity.id != 0) {
        const entt::entity preferredEntity = EntityFromSerializedId(serializedEntity.id);
        entity = registry.valid(preferredEntity)
            ? registry.create()
            : registry.create(preferredEntity);
    }
    else {
        entity = registry.create();
    }

    for (const auto& component : serializedEntity.components) {
        _componentSerializers.LoadComponent(core, registry, entity, component);
    }

    return entity;
}

void WorldSerializer::RegisterDefaultComponentSerializers()
{
    _componentSerializers.Register<Transform>(
        "Transform",
        [](Core&, const Transform& transform) {
            return nlohmann::json {
                {"position", Vec3ToJson(transform.position)},
                {"rotation", QuatToJson(transform.rotation)},
                {"scale", Vec3ToJson(transform.scale)}
            };
        },
        [](Core&, const nlohmann::json& data) {
            Transform transform;
            transform.position = Vec3FromJson(data.at("position"));
            transform.rotation = QuatFromJson(data.at("rotation"));
            transform.scale = Vec3FromJson(data.at("scale"));
            return transform;
        }
    );

    _componentSerializers.Register<HierarchyComponent>(
        "HierarchyComponent",
        [](Core&, const HierarchyComponent& hierarchy) {
            const bool hasParent = hierarchy.parent != entt::null;
            return nlohmann::json {
                {"parent", hasParent ? static_cast<uint64_t>(entt::to_integral(hierarchy.parent)) : uint64_t{ 0 }},
                {"inheritTransform", hierarchy.inheritTransform},
                {"localPosition", Vec3ToJson(hierarchy.localTransform.position)},
                {"localRotation", QuatToJson(hierarchy.localTransform.rotation)},
                {"localScale", Vec3ToJson(hierarchy.localTransform.scale)}
            };
        },
        [](Core&, const nlohmann::json& data) {
            HierarchyComponent hierarchy;
            const uint64_t parentId = data.value("parent", uint64_t{ 0 });
            hierarchy.parent = parentId == 0 ? entt::null : EntityFromSerializedId(parentId);
            hierarchy.inheritTransform = data.value("inheritTransform", true);
            hierarchy.localTransform.position = Vec3FromJson(data.at("localPosition"));
            hierarchy.localTransform.rotation = QuatFromJson(data.at("localRotation"));
            hierarchy.localTransform.scale = Vec3FromJson(data.at("localScale"));
            return hierarchy;
        }
    );

    _componentSerializers.Register<NameComponent>(
        "NameComponent",
        [](Core&, const NameComponent& name) {
            return nlohmann::json {
                {"name", name.name}
            };
        },
        [](Core&, const nlohmann::json& data) {
            return NameComponent {
                data.at("name").get<std::string>()
            };
        }
    );

    _componentSerializers.Register<MeshComponent>(
        "MeshComponent",
        [](Core&, const MeshComponent& meshComponent) {
            MeshAssetReference reference = meshComponent.source;

            if (!reference.IsValid() && meshComponent.mesh) {
                reference = meshComponent.mesh->source;
            }

            return nlohmann::json {
                {"path", reference.path},
                {"meshIndex", reference.meshIndex},
                {"baseColorFactor", Vec4ToJson(meshComponent.baseColorFactor)}
            };
        },
        [](Core& core, const nlohmann::json& data) {
            MeshComponent meshComponent;
            meshComponent.source = MeshAssetReference {
                data.at("path").get<std::string>(),
                data.at("meshIndex").get<uint32_t>()
            };
            meshComponent.baseColorFactor =
                data.contains("baseColorFactor")
                    ? Vec4FromJson(data.at("baseColorFactor"))
                    : glm::vec4{ 1.0f };

            auto meshes = core.LoadGltfMeshes(&core, meshComponent.source.path);
            if (!meshes.has_value()) {
                ENGINE_LOG_ERROR("Failed to load serialized mesh asset: " + meshComponent.source.path);
                return meshComponent;
            }

            if (meshComponent.source.meshIndex >= meshes->size()) {
                ENGINE_LOG_ERROR("Serialized mesh index is out of range for asset: " + meshComponent.source.path);
                return meshComponent;
            }

            meshComponent.mesh = meshes->at(meshComponent.source.meshIndex);
            return meshComponent;
        }
    );

    _componentSerializers.Register<EffectMeshComponent>(
        "EffectMeshComponent",
        [](Core&, const EffectMeshComponent& effect) {
            return nlohmann::json {
                {"color", Vec4ToJson(effect.color)},
                {"corruptionColor", Vec4ToJson(effect.corruptionColor)},
                {"velocity", Vec3ToJson(effect.velocity)},
                {"angularVelocity", Vec3ToJson(effect.angularVelocity)},
                {"lifetime", effect.lifetime},
                {"age", effect.age},
                {"startScale", effect.startScale},
                {"endScale", effect.endScale},
                {"fresnelPower", effect.fresnelPower},
                {"fresnelIntensity", effect.fresnelIntensity},
                {"baseIntensity", effect.baseIntensity},
                {"corruptionScale", effect.corruptionScale},
                {"corruptionSoftness", effect.corruptionSoftness},
                {"corruptionIntensity", effect.corruptionIntensity},
                {"corruptionAmount", effect.corruptionAmount},
                {"destroyOnComplete", effect.destroyOnComplete}
            };
        },
        [](Core&, const nlohmann::json& data) {
            EffectMeshComponent effect;
            effect.color = Vec4FromJson(data.at("color"));
            effect.corruptionColor = Vec4FromJson(data.at("corruptionColor"));
            effect.velocity = Vec3FromJson(data.at("velocity"));
            effect.angularVelocity = Vec3FromJson(data.at("angularVelocity"));
            effect.lifetime = data.value("lifetime", 0.35f);
            effect.age = data.value("age", 0.0f);
            effect.startScale = data.value("startScale", 1.0f);
            effect.endScale = data.value("endScale", 2.0f);
            effect.fresnelPower = data.value("fresnelPower", 2.5f);
            effect.fresnelIntensity = data.value("fresnelIntensity", 2.0f);
            effect.baseIntensity = data.value("baseIntensity", 0.2f);
            effect.corruptionScale = data.value("corruptionScale", 8.0f);
            effect.corruptionSoftness = data.value("corruptionSoftness", 0.04f);
            effect.corruptionIntensity = data.value("corruptionIntensity", 0.0f);
            effect.corruptionAmount = data.value("corruptionAmount", 0.0f);
            effect.destroyOnComplete = data.value("destroyOnComplete", true);
            return effect;
        }
    );

    _componentSerializers.RegisterTag<SingleRenderTag>("SingleRenderTag");
    _componentSerializers.RegisterTag<ActiveCameraTag>("ActiveCameraTag");
    _componentSerializers.RegisterTag<DisabledEntityTag>("DisabledEntityTag");

    _componentSerializers.Register<Camera>(
        "Camera",
        [](Core&, const Camera& camera) {
            return nlohmann::json {
                {"fov", camera.fov},
                {"nearPlane", camera.nearPlane},
                {"farPlane", camera.farPlane},
                {"clearColor", Vec4ToJson(camera.clearColor)},
                {"speed", camera.speed}
            };
        },
        [](Core&, const nlohmann::json& data) {
            Camera camera;
            camera.fov = data.at("fov").get<float>();
            camera.nearPlane = data.at("nearPlane").get<float>();
            camera.farPlane = data.at("farPlane").get<float>();
            camera.clearColor = Vec4FromJson(data.at("clearColor"));
            camera.speed = data.at("speed").get<float>();
            return camera;
        }
    );

    _componentSerializers.Register<CinematicCameraShotComponent>(
        "CinematicCameraShotComponent",
        [](Core&, const CinematicCameraShotComponent& shot) {
            nlohmann::json keyframes = nlohmann::json::array();
            for (const auto& keyframe : shot.keyframes) {
                keyframes.push_back({
                    {"duration", keyframe.duration},
                    {"position", Vec3ToJson(keyframe.position)},
                    {"rotation", QuatToJson(keyframe.rotation)},
                    {"fov", keyframe.fov},
                    {"interpolationMode", CameraShotInterpolationModeToString(keyframe.interpolationMode)},
                    {"aimMode", CameraShotAimModeToString(keyframe.aimMode)},
                    {"lookAtPoint", Vec3ToJson(keyframe.lookAtPoint)},
                    {"lookAtEntity", keyframe.lookAtEntity == entt::null
                        ? uint64_t{ 0 }
                        : static_cast<uint64_t>(entt::to_integral(keyframe.lookAtEntity))}
                });
            }

            return nlohmann::json {
                {"duration", ResolveCameraShotDuration(shot)},
                {"time", shot.time},
                {"playbackSpeed", shot.playbackSpeed},
                {"playing", shot.playing},
                {"loop", shot.loop},
                {"showPath", shot.showPath},
                {"interpolationMode", CameraShotInterpolationModeToString(shot.interpolationMode)},
                {"keyframes", keyframes}
            };
        },
        [](Core&, const nlohmann::json& data) {
            CinematicCameraShotComponent shot;
            shot.duration = data.value("duration", 3.0f);
            shot.time = data.value("time", 0.0f);
            shot.playbackSpeed = data.value("playbackSpeed", 1.0f);
            shot.playing = data.value("playing", false);
            shot.loop = data.value("loop", false);
            shot.showPath = data.value("showPath", true);
            if (data.contains("interpolationMode")) {
                shot.interpolationMode = CameraShotInterpolationModeFromString(
                    data.value("interpolationMode", "CatmullRom"));
            }
            else {
                shot.interpolationMode = data.value("smoothInterpolation", true)
                    ? CameraShotInterpolationMode::Smoothstep
                    : CameraShotInterpolationMode::Linear;
            }

            bool loadedAbsoluteKeyframeTimes = false;
            if (data.contains("keyframes") && data["keyframes"].is_array()) {
                for (const auto& keyframeJson : data["keyframes"]) {
                    CameraShotKeyframe keyframe;
                    if (keyframeJson.contains("duration")) {
                        keyframe.duration = keyframeJson.value("duration", 1.0f);
                    }
                    else {
                        keyframe.duration = keyframeJson.value("time", 0.0f);
                        loadedAbsoluteKeyframeTimes = true;
                    }
                    keyframe.position = Vec3FromJson(keyframeJson.at("position"));
                    keyframe.rotation = QuatFromJson(keyframeJson.at("rotation"));
                    keyframe.fov = keyframeJson.value("fov", 90.0f);
                    keyframe.interpolationMode = keyframeJson.contains("interpolationMode")
                        ? CameraShotInterpolationModeFromString(
                            keyframeJson.value("interpolationMode", "CatmullRom"))
                        : shot.interpolationMode;
                    keyframe.aimMode = CameraShotAimModeFromString(
                        keyframeJson.value("aimMode", "UseRotation"));
                    keyframe.lookAtPoint = keyframeJson.contains("lookAtPoint")
                        ? Vec3FromJson(keyframeJson.at("lookAtPoint"))
                        : glm::vec3{ 0.0f };
                    const uint64_t lookAtEntityId =
                        keyframeJson.value("lookAtEntity", uint64_t{ 0 });
                    keyframe.lookAtEntity = lookAtEntityId == 0
                        ? entt::null
                        : EntityFromSerializedId(lookAtEntityId);
                    shot.keyframes.push_back(keyframe);
                }
            }

            if (loadedAbsoluteKeyframeTimes) {
                std::sort(
                    shot.keyframes.begin(),
                    shot.keyframes.end(),
                    [](const CameraShotKeyframe& first, const CameraShotKeyframe& second) {
                        return first.duration < second.duration;
                    });

                float previousAbsoluteTime = 0.0f;
                for (std::size_t index = 0; index < shot.keyframes.size(); ++index) {
                    const float absoluteTime = glm::max(0.0f, shot.keyframes[index].duration);
                    shot.keyframes[index].duration = index == 0
                        ? 0.0f
                        : glm::max(0.0f, absoluteTime - previousAbsoluteTime);
                    previousAbsoluteTime = absoluteTime;
                }
            }
            else if (!shot.keyframes.empty()) {
                shot.keyframes.front().duration = 0.0f;
            }

            shot.duration = ResolveCameraShotDuration(shot);

            return shot;
        }
    );

    _componentSerializers.Register<SunlightComponent>(
        "SunlightComponent",
        [](Core&, const SunlightComponent& sunlight) {
            return nlohmann::json {
                {"direction", Vec3ToJson(sunlight.direction)},
                {"intensity", sunlight.intensity},
                {"color", Vec3ToJson(sunlight.color)},
                {"ambient", sunlight.ambient}
            };
        },
        [](Core&, const nlohmann::json& data) {
            SunlightComponent sunlight;
            sunlight.direction = Vec3FromJson(data.at("direction"));
            sunlight.intensity = data.at("intensity").get<float>();
            sunlight.color = Vec3FromJson(data.at("color"));
            sunlight.ambient = data.at("ambient").get<float>();
            return sunlight;
        }
    );

    _componentSerializers.Register<VelocityComponent>(
        "VelocityComponent",
        [](Core&, const VelocityComponent& velocity) {
            return nlohmann::json {
                {"linear", Vec3ToJson(velocity.linear)},
                {"angular", Vec3ToJson(velocity.angular)}
            };
        },
        [](Core&, const nlohmann::json& data) {
            VelocityComponent velocity;
            velocity.linear = Vec3FromJson(data.at("linear"));
            velocity.angular = Vec3FromJson(data.at("angular"));
            return velocity;
        }
    );

    _componentSerializers.Register<GravityBodyComponent>(
        "GravityBodyComponent",
        [](Core&, const GravityBodyComponent& gravityBody) {
            return nlohmann::json {
                {"mass", gravityBody.mass}
            };
        },
        [](Core&, const nlohmann::json& data) {
            GravityBodyComponent gravityBody;
            gravityBody.mass = data.at("mass").get<float>();
            return gravityBody;
        }
    );

    _componentSerializers.Register<GravityParticleComponent>(
        "GravityParticleComponent",
        [](Core&, const GravityParticleComponent& gravityParticle) {
            return nlohmann::json {
                {"gravityScale", gravityParticle.gravityScale}
            };
        },
        [](Core&, const nlohmann::json& data) {
            GravityParticleComponent gravityParticle;
            gravityParticle.gravityScale = data.at("gravityScale").get<float>();
            return gravityParticle;
        }
    );

    _componentSerializers.Register<JoltColliderComponent>(
        "JoltColliderComponent",
        [](Core&, const JoltColliderComponent& collider) {
            return nlohmann::json {
                {"shape", JoltColliderShapeToString(collider.shape)},
                {"motion", JoltBodyMotionToString(collider.motion)},
                {"sensor", collider.sensor},
                {"center", Vec3ToJson(collider.center)},
                {"radius", collider.radius},
                {"halfExtents", Vec3ToJson(collider.halfExtents)},
                {"capsuleHalfHeight", collider.capsuleHalfHeight},
                {"friction", collider.friction},
                {"restitution", collider.restitution}
            };
        },
        [](Core&, const nlohmann::json& data) {
            JoltColliderComponent collider;
            collider.shape = JoltColliderShapeFromString(data.value("shape", std::string{ "Sphere" }));
            collider.motion = JoltBodyMotionFromString(data.value("motion", std::string{ "Kinematic" }));
            collider.sensor = data.value("sensor", false);
            collider.center = data.contains("center")
                ? Vec3FromJson(data.at("center"))
                : glm::vec3{ 0.0f };
            collider.radius = data.value("radius", 1.0f);
            collider.halfExtents = data.contains("halfExtents")
                ? Vec3FromJson(data.at("halfExtents"))
                : glm::vec3{ 0.5f };
            collider.capsuleHalfHeight = data.value("capsuleHalfHeight", 0.5f);
            collider.friction = data.value("friction", 0.2f);
            collider.restitution = data.value("restitution", 0.0f);
            return collider;
        }
    );
}

} // namespace Engine
