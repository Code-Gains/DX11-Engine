#pragma once
#include <unordered_map>
#include <vector>
#include <memory>
#include "Component.hpp"

using Entity = std::size_t;

class EntityManager
{
public:
    EntityManager() {};
    ~EntityManager() {};

    Entity CreateEntity();
    void DestroyEntity(Entity entity);

    template <typename T, typename... Args>
    T& AddComponent(Entity entity, Args&&... args);

    template <typename T>
    void RemoveComponent(Entity entity);

    template <typename T>
    T& GetComponent(Entity entity);

    template <typename T>
    bool HasComponent(Entity entity);

private:
    std::unordered_map<Entity, std::vector<std::unique_ptr<Component>>> entities_;
    Entity nextEntityId_ = 0;
};
