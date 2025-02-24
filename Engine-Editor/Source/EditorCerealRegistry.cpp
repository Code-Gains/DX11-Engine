#include <cereal/types/memory.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/archives/json.hpp>
#include "IComponentVector.hpp"
#include "ComponentVector.hpp"
#include "WorldHierarchyComponent.hpp"

CEREAL_REGISTER_TYPE(ComponentVector<WorldHierarchyComponent>);
CEREAL_REGISTER_POLYMORPHIC_RELATION(IComponentVector, ComponentVector<WorldHierarchyComponent>)
