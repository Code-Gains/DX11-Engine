#include "EntityManager.hpp"

template <typename T, typename... Args>
T& EntityManager::AddComponent(Entity entity, Args&&... args)
{
    T* comp(new T(std::forward<Args>(args)...));
    comp->entity = entity;
    entities_[entity].emplace_back(comp);
    return *comp;
}