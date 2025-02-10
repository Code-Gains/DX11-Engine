#include "Editor.hpp"

Editor::Editor()
{
}

void Editor::Run()
{
    RenderingApplication3D application("CG Engine");
    application.Initialize();

    auto device = application.GetApplicationDevice();
    auto deviceContext = application.GetApplicationDeviceContext();
    HWND hwnd = application.GetApplicationWindow();
    HANDLE handle = application.GetProcessHandle();

    if (!application.Load())
    {
        std::cerr << "Could not load Rendering Application!\n";
        return;
    }

    auto world = World(
        application.GetApplicationWindow(),
        &application,
        device,
        deviceContext,
        static_cast<int>(application.GetWindowWidth()),
        static_cast<int>(application.GetWindowHeight())
    );

    auto ecs = application.GetECS();
    LoadSystems(ecs, world, hwnd, handle);

    application.AddEngineModule(std::unique_ptr<IEngineModule>(static_cast<IEngineModule*>(&world)));
    ConstantBufferBinder& constantBufferBinder = ConstantBufferBinder::GetInstance();
    constantBufferBinder.Initialize(deviceContext);
    application.Run();
}

void Editor::LoadSystems(ECS* ecs, World& world, HWND hwnd, HANDLE handle)
{
    ecs->AddSystem<EditorUIManagerSystem>(ecs);
    ecs->AddSystem<EntityMonitoringSystem>(ecs);
    ecs->AddSystem<EntityEditor>(ecs);

    // Optional CORE Systems
    auto profiler = ecs->CreateEntity();
    auto profilerComponent = ProfilerComponent();
    profilerComponent.Initialize(hwnd, handle);
    ecs->AddComponent(profiler, profilerComponent);
    ecs->AddSystem<ProfilerSystem>(ecs);

    //Editor Components
    auto worldHierarchy = ecs->CreateEntity();
    auto worldHierarchyComponent = WorldHierarchyComponent();
    ecs->AddComponent(worldHierarchy, worldHierarchyComponent);

    //Editor systems
    ecs->AddSystem<WorldHierarchy>(ecs, &world);
}
