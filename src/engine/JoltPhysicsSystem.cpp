#include "JoltPhysicsSystem.h"

#include "Core.h"
#include "EntityState.h"
#include "JoltPhysicsComponents.h"
#include "Log.h"
#include "Transform.h"

#include <Jolt/Jolt.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyID.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Body/MotionType.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Physics/Collision/CollideShape.h>
#include <Jolt/Physics/Collision/CollisionCollectorImpl.h>
#include <Jolt/Physics/Collision/NarrowPhaseQuery.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/RegisterTypes.h>

#include <algorithm>
#include <cmath>
#include <thread>

namespace Engine {
namespace {

namespace Layers {
    constexpr JPH::ObjectLayer Static = 0;
    constexpr JPH::ObjectLayer Kinematic = 1;
    constexpr JPH::ObjectLayer Count = 2;
}

namespace BroadPhaseLayers {
    constexpr JPH::BroadPhaseLayer Static(0);
    constexpr JPH::BroadPhaseLayer Kinematic(1);
    constexpr JPH::uint Count = 2;
}

class BroadPhaseLayerInterface final : public JPH::BroadPhaseLayerInterface {
public:
    JPH::uint GetNumBroadPhaseLayers() const override
    {
        return BroadPhaseLayers::Count;
    }

    JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer layer) const override
    {
        return layer == Layers::Static
            ? BroadPhaseLayers::Static
            : BroadPhaseLayers::Kinematic;
    }

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
    const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer layer) const override
    {
        return static_cast<JPH::BroadPhaseLayer::Type>(layer) == static_cast<JPH::BroadPhaseLayer::Type>(BroadPhaseLayers::Static)
            ? "Static"
            : "Kinematic";
    }
#endif
};

class ObjectVsBroadPhaseLayerFilter final : public JPH::ObjectVsBroadPhaseLayerFilter {
public:
    bool ShouldCollide(JPH::ObjectLayer layer, JPH::BroadPhaseLayer broadPhaseLayer) const override
    {
        if (layer == Layers::Static) {
            return broadPhaseLayer == BroadPhaseLayers::Kinematic;
        }

        return true;
    }
};

class ObjectLayerPairFilter final : public JPH::ObjectLayerPairFilter {
public:
    bool ShouldCollide(JPH::ObjectLayer first, JPH::ObjectLayer second) const override
    {
        return first != Layers::Static || second != Layers::Static;
    }
};

class StaticObjectLayerFilter final : public JPH::ObjectLayerFilter {
public:
    bool ShouldCollide(JPH::ObjectLayer layer) const override
    {
        return layer == Layers::Static;
    }
};

BroadPhaseLayerInterface& GetBroadPhaseLayerInterface()
{
    static BroadPhaseLayerInterface interface;
    return interface;
}

ObjectVsBroadPhaseLayerFilter& GetObjectVsBroadPhaseLayerFilter()
{
    static ObjectVsBroadPhaseLayerFilter filter;
    return filter;
}

ObjectLayerPairFilter& GetObjectLayerPairFilter()
{
    static ObjectLayerPairFilter filter;
    return filter;
}

StaticObjectLayerFilter& GetStaticObjectLayerFilter()
{
    static StaticObjectLayerFilter filter;
    return filter;
}

void EnsureJoltTypesRegistered()
{
    static bool initialized = false;
    if (initialized) {
        return;
    }

    JPH::RegisterDefaultAllocator();
    JPH::Factory::sInstance = new JPH::Factory();
    JPH::RegisterTypes();
    initialized = true;
}

JPH::Vec3 ToJoltVec3(const glm::vec3& value)
{
    return JPH::Vec3{ value.x, value.y, value.z };
}

JPH::Quat ToJoltQuat(const glm::quat& value)
{
    return JPH::Quat{ value.x, value.y, value.z, value.w };
}

glm::vec3 FromJoltVec3(const JPH::Vec3& value)
{
    return { value.GetX(), value.GetY(), value.GetZ() };
}

float MaxScaleAxis(const glm::vec3& scale)
{
    return std::max({ std::abs(scale.x), std::abs(scale.y), std::abs(scale.z), 0.0001f });
}

glm::vec3 AbsScale(const glm::vec3& scale)
{
    return {
        std::max(0.0001f, std::abs(scale.x)),
        std::max(0.0001f, std::abs(scale.y)),
        std::max(0.0001f, std::abs(scale.z))
    };
}

JPH::RefConst<JPH::Shape> CreateShape(const JoltColliderComponent& collider, const Transform& transform)
{
    const glm::vec3 scale = AbsScale(transform.scale);

    JPH::RefConst<JPH::Shape> shape;
    switch (collider.shape) {
    case JoltColliderShape::Sphere:
        shape = new JPH::SphereShape(std::max(0.0001f, collider.radius * MaxScaleAxis(transform.scale)));
        break;
    case JoltColliderShape::Box:
        shape = new JPH::BoxShape(ToJoltVec3(glm::max(collider.halfExtents * scale, glm::vec3{ 0.0001f })));
        break;
    case JoltColliderShape::Capsule:
        shape = new JPH::CapsuleShape(
            std::max(0.0f, collider.capsuleHalfHeight * scale.y),
            std::max(0.0001f, collider.radius * std::max(scale.x, scale.z)));
        break;
    }

    if (glm::dot(collider.center, collider.center) <= 0.0000001f) {
        return shape;
    }

    return new JPH::RotatedTranslatedShape(
        ToJoltVec3(collider.center * scale),
        JPH::Quat::sIdentity(),
        shape);
}

JPH::EMotionType ToJoltMotionType(JoltBodyMotion motion)
{
    return motion == JoltBodyMotion::Static
        ? JPH::EMotionType::Static
        : JPH::EMotionType::Kinematic;
}

JPH::ObjectLayer ToJoltObjectLayer(JoltBodyMotion motion)
{
    return motion == JoltBodyMotion::Static
        ? Layers::Static
        : Layers::Kinematic;
}

bool ApproximatelyEqual(float first, float second)
{
    return std::abs(first - second) <= 0.0001f;
}

bool ApproximatelyEqual(const glm::vec3& first, const glm::vec3& second)
{
    return
        ApproximatelyEqual(first.x, second.x) &&
        ApproximatelyEqual(first.y, second.y) &&
        ApproximatelyEqual(first.z, second.z);
}

} // namespace

JoltPhysicsSystem::JoltPhysicsSystem(entt::registry& registry, Core* core)
    : System(registry)
    , _core(core)
{
    InitializeJolt();
}

JoltPhysicsSystem::~JoltPhysicsSystem()
{
    for (auto& [entity, bodyId] : _bodies) {
        if (!_physicsSystem) {
            continue;
        }

        auto& bodyInterface = _physicsSystem->GetBodyInterface();
        const JPH::BodyID joltBodyId{ bodyId };
        bodyInterface.RemoveBody(joltBodyId);
        bodyInterface.DestroyBody(joltBodyId);
    }
}

void JoltPhysicsSystem::InitializeJolt()
{
    EnsureJoltTypesRegistered();

    constexpr JPH::uint maxBodies = 65536;
    constexpr JPH::uint numBodyMutexes = 0;
    constexpr JPH::uint maxBodyPairs = 65536;
    constexpr JPH::uint maxContactConstraints = 32768;
    constexpr int maxPhysicsJobs = 2048;
    constexpr int maxPhysicsBarriers = 8;

    _tempAllocator = std::make_unique<JPH::TempAllocatorImpl>(32 * 1024 * 1024);
    _jobSystem = std::make_unique<JPH::JobSystemThreadPool>(
        maxPhysicsJobs,
        maxPhysicsBarriers,
        std::max(1u, std::thread::hardware_concurrency()) - 1);
    _physicsSystem = std::make_unique<JPH::PhysicsSystem>();
    _physicsSystem->Init(
        maxBodies,
        numBodyMutexes,
        maxBodyPairs,
        maxContactConstraints,
        GetBroadPhaseLayerInterface(),
        GetObjectVsBroadPhaseLayerFilter(),
        GetObjectLayerPairFilter());
    _physicsSystem->SetGravity(JPH::Vec3::sZero());
}

void JoltPhysicsSystem::Update(float deltaTime)
{
    if (!_physicsSystem) {
        return;
    }

    if (_core && !_core->IsPlayMode()) {
        RemoveStaleBodies();
        SyncBodies();
        return;
    }

    _physicsSystem->Update(
        std::max(0.0f, deltaTime),
        1,
        _tempAllocator.get(),
        _jobSystem.get());
}

void JoltPhysicsSystem::FixedUpdate(float)
{
    if (!_physicsSystem || (_core && !_core->IsPlayMode())) {
        return;
    }

    RemoveStaleBodies();
    SyncBodies();
}

void JoltPhysicsSystem::OnPlayStart()
{
    while (!_bodies.empty()) {
        RemoveBody(_bodies.begin()->first);
    }

    InitializeJolt();
    SyncBodies();
}

void JoltPhysicsSystem::OnPlayStop()
{
    while (!_bodies.empty()) {
        RemoveBody(_bodies.begin()->first);
    }

    InitializeJolt();
}

std::vector<JoltPhysicsSystem::ShapeHit> JoltPhysicsSystem::CollideSphereWithStatic(
    const glm::vec3& position,
    float radius) const
{
    std::vector<ShapeHit> hits;
    if (!_physicsSystem || radius <= 0.0f) {
        return hits;
    }

    const JPH::SphereShape sphere{ std::max(0.0001f, radius) };
    JPH::CollideShapeSettings settings;
    JPH::AllHitCollisionCollector<JPH::CollideShapeCollector> collector;

    _physicsSystem->GetNarrowPhaseQuery().CollideShape(
        &sphere,
        JPH::Vec3::sOne(),
        JPH::RMat44::sTranslation(ToJoltVec3(position)),
        settings,
        JPH::RVec3::sZero(),
        collector,
        {},
        GetStaticObjectLayerFilter());

    hits.reserve(collector.mHits.size());
    for (const auto& hit : collector.mHits) {
        if (hit.mPenetrationDepth <= 0.0f) {
            continue;
        }

        const auto entityIterator =
            _bodyEntities.find(hit.mBodyID2.GetIndexAndSequenceNumber());
        if (entityIterator == _bodyEntities.end()) {
            continue;
        }

        const glm::vec3 normal =
            FromJoltVec3((-hit.mPenetrationAxis).NormalizedOr(JPH::Vec3::sAxisY()));
        hits.push_back(ShapeHit{
            entityIterator->second,
            normal,
            FromJoltVec3(hit.mContactPointOn1),
            hit.mPenetrationDepth
        });
    }

    return hits;
}

void JoltPhysicsSystem::SyncBodies()
{
    auto view = _registry.view<Transform, JoltColliderComponent>(entt::exclude<DisabledEntityTag>);
    for (auto entity : view) {
        if (IsEntityDisabled(_registry, entity)) {
            RemoveBody(entity);
            continue;
        }

        CreateOrUpdateBody(entity);
    }
}

void JoltPhysicsSystem::RemoveStaleBodies()
{
    for (auto iterator = _bodies.begin(); iterator != _bodies.end();) {
        const entt::entity entity = iterator->first;
        if (_registry.valid(entity) &&
            _registry.all_of<Transform, JoltColliderComponent>(entity) &&
            !IsEntityDisabled(_registry, entity)) {
            ++iterator;
            continue;
        }

        const JPH::BodyID bodyId{ iterator->second };
        auto& bodyInterface = _physicsSystem->GetBodyInterface();
        bodyInterface.RemoveBody(bodyId);
        bodyInterface.DestroyBody(bodyId);
        _bodyEntities.erase(bodyId.GetIndexAndSequenceNumber());
        if (_registry.valid(entity) && _registry.all_of<JoltBodyComponent>(entity)) {
            _registry.remove<JoltBodyComponent>(entity);
        }
        iterator = _bodies.erase(iterator);
    }
}

void JoltPhysicsSystem::RemoveBody(entt::entity entity)
{
    const auto bodyIterator = _bodies.find(entity);
    if (bodyIterator == _bodies.end()) {
        if (_registry.valid(entity) && _registry.all_of<JoltBodyComponent>(entity)) {
            _registry.remove<JoltBodyComponent>(entity);
        }
        return;
    }

    auto& bodyInterface = _physicsSystem->GetBodyInterface();
    const JPH::BodyID bodyId{ bodyIterator->second };
    bodyInterface.RemoveBody(bodyId);
    bodyInterface.DestroyBody(bodyId);
    _bodyEntities.erase(bodyId.GetIndexAndSequenceNumber());
    _bodies.erase(bodyIterator);

    if (_registry.valid(entity) && _registry.all_of<JoltBodyComponent>(entity)) {
        _registry.remove<JoltBodyComponent>(entity);
    }
}

void JoltPhysicsSystem::CreateOrUpdateBody(entt::entity entity)
{
    if (!BodyMatchesAuthoring(entity)) {
        RemoveBody(entity);
    }

    const auto& transform = _registry.get<Transform>(entity);
    const auto& collider = _registry.get<JoltColliderComponent>(entity);

    auto& bodyInterface = _physicsSystem->GetBodyInterface();
    const auto bodyIterator = _bodies.find(entity);
    if (bodyIterator != _bodies.end()) {
        bodyInterface.SetPositionAndRotationWhenChanged(
            JPH::BodyID{ bodyIterator->second },
            ToJoltVec3(transform.position),
            ToJoltQuat(transform.rotation),
            JPH::EActivation::Activate);
        return;
    }

    const auto shape = CreateShape(collider, transform);
    JPH::BodyCreationSettings settings(
        shape,
        ToJoltVec3(transform.position),
        ToJoltQuat(transform.rotation),
        ToJoltMotionType(collider.motion),
        ToJoltObjectLayer(collider.motion));
    settings.mFriction = std::max(0.0f, collider.friction);
    settings.mRestitution = std::max(0.0f, collider.restitution);
    settings.mIsSensor = collider.sensor;
    settings.mUserData = static_cast<JPH::uint64>(entt::to_integral(entity));

    const JPH::BodyID bodyId = bodyInterface.CreateAndAddBody(settings, JPH::EActivation::Activate);
    if (bodyId.IsInvalid()) {
        ENGINE_LOG_ERROR("Failed to create Jolt body.");
        return;
    }

    _bodies[entity] = bodyId.GetIndexAndSequenceNumber();
    _bodyEntities[bodyId.GetIndexAndSequenceNumber()] = entity;
    _registry.emplace_or_replace<JoltBodyComponent>(
        entity,
        JoltBodyComponent{
            bodyId.GetIndexAndSequenceNumber(),
            collider.shape,
            collider.motion,
            collider.sensor,
            transform.scale,
            collider.center,
            collider.radius,
            collider.halfExtents,
            collider.capsuleHalfHeight,
            collider.friction,
            collider.restitution
        });
}

bool JoltPhysicsSystem::BodyMatchesAuthoring(entt::entity entity) const
{
    if (!_registry.all_of<JoltBodyComponent>(entity)) {
        return false;
    }

    const auto& collider = _registry.get<JoltColliderComponent>(entity);
    const auto& transform = _registry.get<Transform>(entity);
    const auto& body = _registry.get<JoltBodyComponent>(entity);

    return
        body.shape == collider.shape &&
        body.motion == collider.motion &&
        body.sensor == collider.sensor &&
        ApproximatelyEqual(body.transformScale, transform.scale) &&
        ApproximatelyEqual(body.center, collider.center) &&
        ApproximatelyEqual(body.radius, collider.radius) &&
        ApproximatelyEqual(body.halfExtents, collider.halfExtents) &&
        ApproximatelyEqual(body.capsuleHalfHeight, collider.capsuleHalfHeight) &&
        ApproximatelyEqual(body.friction, collider.friction) &&
        ApproximatelyEqual(body.restitution, collider.restitution);
}

} // namespace Engine
