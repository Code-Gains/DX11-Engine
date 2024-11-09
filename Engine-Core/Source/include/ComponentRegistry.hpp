#pragma once
#include <regex>
#include <string>
#include "Component.hpp"

class ComponentRegistry
{
    // max of 2^16 component types
    static ComponentType _nextComponentType;
    static std::unordered_map<std::type_index, ComponentType> _componentTypes;
    static std::unordered_map<std::type_index, std::string> _componentNames;
    static std::unordered_map<ComponentType, std::string> _componentTypeNames;
    static std::unordered_map<ComponentType, std::function<std::unique_ptr<IComponentVector>()>> _vectorFactories;
    static std::unordered_map<ComponentType, std::function<std::unique_ptr<ComponentUI>()>> _uiFactories;

public:

    template<typename TComponent>
    static std::optional<ComponentType> GetComponentType()
    {
        static_assert(std::is_base_of<Component, TComponent>::value, "TComponent must inherit from Component!");

        auto type = std::type_index(typeid(TComponent));
        if (_componentTypes.find(type) == _componentTypes.end())
            return std::nullopt;

        return _componentTypes[type];
    }

    template<typename TComponent>
    static std::optional<std::string> GetComponentName()
    {
        static_assert(std::is_base_of<Component, TComponent>::value, "TComponent must inherit from Component!");

        auto type = std::type_index(typeid(TComponent));
        if (_componentTypes.find(type) == _componentTypes.end())
            return std::nullopt;

        return _componentNames[type];
    }

    static std::optional<std::string> GetComponentNameByType(ComponentType type)
    {
        auto it = _componentTypeNames.find(type);
        if (it != _componentTypeNames.end())
            return it->second;

        return std::nullopt;
    }

    template<typename... TComponents>
    static std::optional<ComponentSignature> GetComponentArraySignature()
    {
        ComponentSignature signature;

        // &signature captures the local variable and places it inside the scope
        auto processComponent = [&signature](auto compTypeOpt) -> bool
            {
                if (!compTypeOpt)
                    return false;

                // set the bit in the signature bitset
                signature.set(*compTypeOpt);
                return true;
            };

        // call GetComponentType for each value in TComponents and accumulate validations with &&
        bool valid = (processComponent(GetComponentType<TComponents>()) && ...);

        if (!valid) // return even if at least one component was failed to be identified
            return std::nullopt;

        // bitset was generated in processComponent
        return signature;
    }

    template<typename TComponent>
    static ComponentType GetOrRegisterComponentType()
    {
        static_assert(std::is_base_of<Component, TComponent>::value, "TComponent must inherit from Component!");

        auto type = std::type_index(typeid(TComponent));
        if (_componentTypes.find(type) == _componentTypes.end())
        {
            ComponentType ct = _nextComponentType++;
            _componentTypes[type] = ct;
            auto componentName = GetCleanComponentName(typeid(TComponent).name());
            _componentNames[type] = componentName;
            _componentTypeNames[ct] = componentName;
            _vectorFactories[ct] = []() -> std::unique_ptr<IComponentVector>
                {
                    // this does not return, just saves the expression into std::function
                    return std::make_unique<ComponentVector<TComponent>>();
                };
            return ct;
        }
        return _componentTypes[type];
    }

    static std::function<std::unique_ptr<IComponentVector>()> GetFactoryFunction(ComponentType componentType)
    {
        auto it = _vectorFactories.find(componentType);
        if (it != _vectorFactories.end())
        {
            return it->second;
        }

        throw std::runtime_error("Could not find a factory for component vector");
    }

    static size_t GetComponentCount()
    {
        return _nextComponentType;
    }

    template<typename TComponent, typename TComponentUI>
    static void RegisterComponentUI()
    {
        static_assert(std::is_base_of<Component, TComponent>::value, "TComponent must inherit from Component!");
        static_assert(std::is_base_of<ComponentUI, TComponentUI>::value, "TComponentUI must inherit from ComponentUI!");

        ComponentType ct = GetOrRegisterComponentType<TComponent>();

        _uiFactories[ct] = []() -> std::unique_ptr<ComponentUI>
            {
                //return std::unique_ptr<ComponentUI>(std::make_unique<TComponentUI>().release());
                return std::make_unique<TComponentUI>();
            };
    }

    // Get factory function for UI component
    static std::function<std::unique_ptr<ComponentUI>()> GetUIFactoryFunction(ComponentType componentType)
    {
        auto it = _uiFactories.find(componentType);
        if (it != _uiFactories.end())
        {
            return it->second;
        }

        // The component does not have a registered UI
        return nullptr;
    }

    static std::string GetCleanComponentName(const std::string& fullName)
    {
        // Remove "class " prefix if present
        std::string cleanName = std::regex_replace(fullName, std::regex(R"(^class\s+)"), "");

        // Remove template arguments (anything within angle brackets)
        cleanName = std::regex_replace(cleanName, std::regex(R"(<.*>)"), "");

        // Remove "Component" suffix if present
        if (cleanName.size() >= 9 && cleanName.compare(cleanName.size() - 9, 9, "Component") == 0)
        {
            cleanName.erase(cleanName.size() - 9);
        }

        return cleanName;
    }
};