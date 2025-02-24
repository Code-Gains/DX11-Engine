#pragma once
//#include <cereal/cereal.hpp>

#include "Component.hpp"
#include "IComponentVector.hpp"

template<typename TComponent>
class ComponentVector : public IComponentVector
{
    std::unordered_map<Entity, size_t> _entityToIndex;
    std::vector<TComponent> _components;

public:
    void* GetComponent(Entity entity)
    {
        auto it = _entityToIndex.find(entity);
        if (it != _entityToIndex.end())
            return &(_components[it->second]);

        throw std::runtime_error("Component not found for entity!");
    }

    /*void GetComponent(Entity entity) const override
    {
        auto it = _entityToIndex.find(entity);
        if (it != _entityToIndex.end())
        {
            TComponent const& component = _components[it->second];
            return static_cast<ComponentVector<TComponent>&>(component);
        }
    }*/

    size_t GetComponentCount() const override
    {
        return _components.size();
    }

    // RETURN REF TODO
    std::vector<TComponent>* GetRawVector()
    {
        return &_components;
    }

    const std::vector<TComponent>& GetRawVectorConst() const
    {
        return _components;
    }

    const std::unordered_map<Entity, size_t>& GetEntityToIndex() const
    {
        return _entityToIndex;
    }

    void CloneComponent(Entity entity, IComponentVector& target) const override
    {
        auto it = _entityToIndex.find(entity);
        if (it != _entityToIndex.end())
        {
            TComponent const& component = _components[it->second];
            static_cast<ComponentVector<TComponent>&>(target).AddComponent(entity, component);
        }
    }

    void TransferComponent(Entity entity, IComponentVector& target) const override
    {
        auto it = _entityToIndex.find(entity);
        if (it != _entityToIndex.end())
        {
            TComponent const& component = _components[it->second];
            static_cast<ComponentVector<TComponent>&>(target).AddComponent(entity, component);

            const_cast<ComponentVector<TComponent>*>(this)->RemoveComponent(entity);
        }
    }

    void AddComponent(Entity entity, const TComponent& component)
    {
        _components.push_back(component);
        _entityToIndex[entity] = _components.size() - 1;
        std::cout << "Entity to index added\n";
    }

    void RemoveComponent(Entity entity) override
    {
        auto it = _entityToIndex.find(entity);
        if (it != _entityToIndex.end()) {
            size_t indexToRemove = it->second;

            // if it's not the last element, swap with the last one for fast removal
            if (indexToRemove != _components.size() - 1) {
                std::swap(_components[indexToRemove], _components.back());
                // update the swapped component's index in _entityToIndex TODO simplify
                auto entityOfLastComponent = std::find_if(_entityToIndex.begin(), _entityToIndex.end(),
                    [this, indexToRemove](const auto& pair) {
                        return pair.second == _components.size() - 1;
                    })->first;
                _entityToIndex[entityOfLastComponent] = indexToRemove;
            }
            _components.pop_back();
            _entityToIndex.erase(it);
        }
    }

public:
    ComponentVector() {};
    ~ComponentVector() {};

    // Serialization
    template<typename Archive>
    void save(Archive& archive) const
    {
        archive(CEREAL_NVP(_entityToIndex));
        //archive(CEREAL_NVP(_components));
    }

    template<typename Archive>
    void load(Archive& archive)
    {
    }
};