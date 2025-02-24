#pragma once
#include <cereal/cereal.hpp>

#include "Entity.hpp"
class IComponentVector
{
public:
    virtual ~IComponentVector() = default;
    virtual void* GetComponent(Entity entity) = 0;
    virtual void CloneComponent(Entity entity, IComponentVector& target) const = 0;
    virtual void TransferComponent(Entity entity, IComponentVector& target) const = 0;
    virtual void RemoveComponent(Entity entity) = 0;
    virtual size_t GetComponentCount() const = 0;

    template <class Archive>
    void save(Archive& archive) const
    {
    }

    template <class Archive>
    void load(Archive& archive)
    {
    }
};

class IComponentVectorFactory
{
public:
    virtual ~IComponentVectorFactory();

};

