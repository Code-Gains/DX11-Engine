#include "World.hpp"

World::World()
{
}

World::World(HWND win32Window, RenderingApplication3D* renderingApplication,
ID3D11Device* device, ID3D11DeviceContext* deviceContext, int viewportWidth,
int viewportHeight, std::wstring worldPath)
    : _win32Window(win32Window),
    _renderingApplication(renderingApplication),
    _device(device),
    _deviceContext(deviceContext),
    _viewportWidth(viewportWidth),
    _viewportHeight(viewportHeight),
    _worldPath(worldPath)
{
    if (_worldPath.length() != 0)
    {
        // do nothing
    }
    else
    {
        std::cout << "[World] Loading default world...\n";
        LoadDefaultWorld();
    }
}

//bool World::Initialize(RenderingApplication3D* renderingApplication, Universe* universe, HWND win32Window, ID3D11Device* device, ID3D11DeviceContext* deviceContext)
//{
//    _win32Window = win32Window;
//    _renderingApplication = renderingApplication;
//    _universe = universe;
//
//    //_worldHierarchy = WorldHierarchy(this);
//	return true;
//}

void World::Update(float deltaTime)
{
    //_worldHierarchy.Update(deltaTime);
}

void World::PeriodicUpdate(float deltaTime)
{
}

void World::Render()
{
    //_worldHierarchy.Render();
}

void World::DestroyEntity(Entity entity)
{
    _renderingApplication->DestroyEntity(entity);
}

std::vector<int> World::GetRenderableEntities(
    const std::unordered_map<int, int>& transformIndices,
    const std::unordered_map<int, int>& meshIndices,
    const std::unordered_map<int, int>& materialIndices) const
{
    std::vector<int> entities;
    for (const auto& [entity, meshIndex] : meshIndices) {
        if (transformIndices.find(entity) != transformIndices.end() && materialIndices.find(entity) != materialIndices.end()) {
            entities.push_back(entity);
        }
    }
    return entities;
}

Entity World::CreateEntity()
{
    return _renderingApplication->CreateEntity();
}

bool World::PrepareLoading()
{
    //_worldHierarchy.Clear();
    return true;
}

bool World::FinalizeLoading()
{
    /*_renderingApplication->ClearAllInstancePools();
    for (auto& transformComponent : _transformComponents)
    {
        transformComponent.SetIsDirty(true);
    }
    for (auto& materialComponents : _materialComponents)
    {
        materialComponents.SetIsDirty(true);
    }

    _worldHierarchy.SetWorld(this);*/

    return false;
}

bool World::SaveWorld(std::string filePath)
{
    //_universe->SaveWorld(filePath);
    return true;
}

bool World::LoadDefaultWorld()
{
    // TODO FIX RETURN REF AND NOT POINTER
    auto ecs = _renderingApplication->GetECS();
    HWND hwnd = _renderingApplication->GetApplicationWindow();
    HANDLE handle = _renderingApplication->GetProcessHandle();

    // Default Scene
    auto camera = ecs->CreateEntity();
    DirectX::XMFLOAT3 cameraStartPosition = { 0.0f, 3.0f, 10.0f };
    DirectX::XMFLOAT3 cameraStartRotation = { 0.0f,  (float)Constants::DegreesToRadians(180), 0.0f };

    auto cameraComponent = CameraComponent(_renderingApplication->GetApplicationDevice(), cameraStartRotation, 20.0f, 3.0f);
    ecs->AddComponent(camera, cameraComponent);

    auto transformComponent = TransformComponent(cameraStartPosition, cameraStartRotation, DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));
    ecs->AddComponent(camera, transformComponent);

    auto materialComponent = MaterialComponent::GetDefaultMaterialComponent();
    ecs->AddComponent(camera, materialComponent);

    auto meshComponent = MeshComponent<VertexPositionNormalUv>::GeneratePrimitiveMeshComponent(PrimitiveGeometryType3D::Skybox);
    ecs->AddComponent(camera, meshComponent);

    auto directionalLight = ecs->CreateEntity();
    auto directionalLightComponent = DirectionalLightComponent
    (
        DirectX::XMFLOAT4(-0.5f, -1.0f, -0.5f, 0.0f),
        DirectX::XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f),
        DirectX::XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f),
        DirectX::XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f)
    );
    ecs->AddComponent(directionalLight, directionalLightComponent);
    ecs->AddSystem<LightingSystem>(
        _renderingApplication->GetApplicationDevice(), _renderingApplication->GetApplicationDeviceContext(), ecs);
    ecs->AddSystem<CameraSystem>(_renderingApplication, ecs);

    std::cout << "[World] Default world loaded!\n";
    return true;
}

void World::UpdateViewportDimensions(int32_t width, int32_t height)
{
	_viewportWidth = width;
	_viewportHeight = height;
}

void World::AddRenderableInstance(int poolKey, int entityId, const InstanceConstantBuffer& instanceData)
{
    _renderingApplication->AddRenderableInstance(poolKey, entityId, instanceData);
}

void World::UpdateRenderableInstanceData(int poolKey, int entityId, const InstanceConstantBuffer& newData)
{
    _renderingApplication->UpdateRenderableInstanceData(poolKey, entityId, newData);
}


