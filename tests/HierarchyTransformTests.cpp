#include "HierarchyComponent.h"
#include "HierarchySystem.h"
#include <cmath>
#include <iostream>
#include <stdexcept>

static void CheckMatrix(const glm::mat4 &actual, const glm::mat4 &expected, const char *message)
{
    for (int column = 0; column < 4; ++column)
        for (int row = 0; row < 4; ++row)
            if (!std::isfinite(actual[column][row]) || std::abs(actual[column][row] - expected[column][row]) > 0.005f)
                throw std::runtime_error(message);
}

int main()
{
    try
    {
        for (float scale : {0.002f, 1.0f, 50.0f, 500.0f, 10000.0f})
        {
            entt::registry registry;
            const auto parent = registry.create();
            Transform parentTransform;
            parentTransform.position = {120, -350, 80};
            parentTransform.rotation = glm::angleAxis(0.73f, glm::normalize(glm::vec3(1, 2, 3)));
            parentTransform.scale = glm::vec3(scale);
            registry.emplace<Transform>(parent, parentTransform);
            Transform world;
            world.position = {370, 250, -160};
            world.rotation = glm::angleAxis(-1.12f, glm::normalize(glm::vec3(3, 1, 2)));
            world.scale = {1, 0.6f, 1.8f};
            const auto local = Engine::ComputeHierarchyLocalTransform(registry, parent, world);
            CheckMatrix(parentTransform.GetModelMatrix() * local.GetModelMatrix(), world.GetModelMatrix(),
                        "small local scale must preserve the requested world transform");
            const auto child = registry.create();
            registry.emplace<Transform>(child, world);
            registry.emplace<HierarchyComponent>(child, parent, true, local);
            for (int frame = 0; frame < 20; ++frame)
                Engine::ResolveHierarchyTransforms(registry);
            CheckMatrix(registry.get<Transform>(child).GetModelMatrix(), world.GetModelMatrix(),
                        "hierarchy resolution must not amplify the child's size");
            auto moved = parentTransform;
            moved.position += glm::vec3(10, 20, -30);
            registry.replace<Transform>(parent, moved);
            Engine::ResolveHierarchyTransforms(registry);
            CheckMatrix(registry.get<Transform>(child).GetModelMatrix(),
                        moved.GetModelMatrix() * local.GetModelMatrix(), "children must still follow parent movement");
        }
        std::cout << "Hierarchy scale round trips, repeated resolution, and parent movement passed.\n";
    }
    catch (const std::exception &error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
