#include "Component.hpp"

// static member allocation
ComponentType ComponentRegistry::_nextComponentType = 0;
std::unordered_map<std::type_index, ComponentType> ComponentRegistry::_componentTypes;
std::unordered_map<std::type_index, std::string> ComponentRegistry::_componentNames;