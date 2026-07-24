#pragma once

#include "System.h"

#include <entt/entt.hpp>

#include <memory>
#include <unordered_map>

namespace JPH {
class BodyInterface;
class JobSystemThreadPool;
class PhysicsSystem;
class TempAllocatorImpl;
}

namespace Engine {

class Core;

class JoltPhysicsSystem : public System {
public:
    JoltPhysicsSystem(entt::registry& registry, Core* core);
    ~JoltPhysicsSystem() override;

    void Update(float deltaTime) override;
    void OnPlayStop() override;

private:
    void InitializeJolt();
    void SyncBodies();
    void RemoveStaleBodies();
    void RemoveBody(entt::entity entity);
    void CreateOrUpdateBody(entt::entity entity);
    bool BodyMatchesAuthoring(entt::entity entity) const;

    Core* _core = nullptr;
    std::unique_ptr<JPH::PhysicsSystem> _physicsSystem;
    std::unique_ptr<JPH::TempAllocatorImpl> _tempAllocator;
    std::unique_ptr<JPH::JobSystemThreadPool> _jobSystem;
    std::unordered_map<entt::entity, uint32_t> _bodies;
};

} // namespace Engine
