#include "World.hpp"
#include "Universe.hpp"

World::World()
{
}

World::~World()
{
}

bool World::Initialize(RenderingApplication3D* renderingApplication, Universe* universe, HWND win32Window, ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
    _win32Window = win32Window;
    _renderingApplication = renderingApplication;
    _universe = universe;

    LightConstantBuffer lightConstantBuffer;
    lightConstantBuffer.Position = { -1.0f, 50.0f, -5.0f, 0.0f };
    lightConstantBuffer.Ambient = { 0.3f, 0.3f, 0.3f, 1.0f };
    lightConstantBuffer.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	lightConstantBuffer.Specular = { 1.0f, 1.0f, 1.0f, 1.0f };
    _renderingApplication->SetLightConstantBuffer(lightConstantBuffer);

    _worldHierarchy = WorldHierarchy(this);
	return true;
}

void World::Update(float deltaTime)
{
    _worldHierarchy.Update(deltaTime);

    //// DirectX namespace contains overloads for vector and float multiplication
    //using namespace DirectX;

    //float cameraMoveSpeed = 1.0f;
    //float cameraRotationSpeed = 1.0f;

    //static DirectX::XMFLOAT3 cameraPosition = { 0.0f, 3.0f, 10.0f };
    //static DirectX::XMFLOAT3 cameraRotation = { 0.0f,  (float)Constants::DegreesToRadians(180), 0.0f };


    //// Camera Movement

    //if (GetAsyncKeyState('W') & 0x8000)
    //{
    //    // Move camera forward
    //    DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(cameraRotation.x, cameraRotation.y, cameraRotation.z);
    //    DirectX::XMVECTOR forward = DirectX::XMVector3TransformCoord(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), rotationMatrix);
    //    DirectX::XMVECTOR newPosition = DirectX::XMLoadFloat3(&cameraPosition) + forward * cameraMoveSpeed * deltaTime;
    //    XMStoreFloat3(&cameraPosition, newPosition);
    //}
    //if (GetAsyncKeyState('S') & 0x8000)
    //{
    //    // Move camera backward
    //    DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(cameraRotation.x, cameraRotation.y, cameraRotation.z);
    //    DirectX::XMVECTOR forward = DirectX::XMVector3TransformCoord(DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f), rotationMatrix);
    //    DirectX::XMVECTOR newPosition = DirectX::XMLoadFloat3(&cameraPosition) + forward * cameraMoveSpeed * deltaTime;
    //    XMStoreFloat3(&cameraPosition, newPosition);
    //}
    //if (GetAsyncKeyState('A') & 0x8000)
    //{
    //    // Move camera left
    //    DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(cameraRotation.x, cameraRotation.y, cameraRotation.z);
    //    DirectX::XMVECTOR right = DirectX::XMVector3TransformCoord(DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), rotationMatrix);
    //    DirectX::XMVECTOR newPosition = DirectX::XMLoadFloat3(&cameraPosition) + right * cameraMoveSpeed * deltaTime;
    //    XMStoreFloat3(&cameraPosition, newPosition);
    //}
    //if (GetAsyncKeyState('D') & 0x8000)
    //{
    //    // Move camera right
    //    DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(cameraRotation.x, cameraRotation.y, cameraRotation.z);
    //    DirectX::XMVECTOR right = DirectX::XMVector3TransformCoord(DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), rotationMatrix);
    //    DirectX::XMVECTOR newPosition = DirectX::XMLoadFloat3(&cameraPosition) - right * cameraMoveSpeed * deltaTime;
    //    XMStoreFloat3(&cameraPosition, newPosition);
    //}


    //static float lastMouseX = 0.0f;
    //static float lastMouseY = 0.0f;

    //bool isRightMouseDown = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
    //static bool wasRightMouseDown = false; // Keep track of previous right mouse button state
    //POINT cursorPos;
    //GetCursorPos(&cursorPos);
    //ScreenToClient(_win32Window, &cursorPos);
    //int mouseX = cursorPos.x;
    //int mouseY = cursorPos.y;

    //if (isRightMouseDown) {
    //    if (!wasRightMouseDown) {
    //        // Right mouse button was just pressed, initialize previous mouse position
    //        lastMouseX = mouseX;
    //        lastMouseY = mouseY;
    //    }

    //    // Calculate the change in mouse position since the last frame
    //    int deltaX = mouseX - lastMouseX;
    //    int deltaY = mouseY - lastMouseY;

    //    // Update the camera rotation based on the change in mouse position
    //    cameraRotation.y -= deltaX * cameraRotationSpeed * deltaTime;
    //    cameraRotation.x += deltaY * cameraRotationSpeed * deltaTime;

    //    // Clamp pitch to prevent camera flipping
    //    cameraRotation.x = (-DirectX::XM_PIDIV2, min(DirectX::XM_PIDIV2, cameraRotation.x));

    //    // Update the previous mouse position
    //    lastMouseX = mouseX;
    //    lastMouseY = mouseY;
    //}

    //wasRightMouseDown = isRightMouseDown;

    //DirectX::XMVECTOR camPos = XMLoadFloat3(&cameraPosition);

    //DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(cameraRotation.x, cameraRotation.y, cameraRotation.z);

    //// Calculate the forward, right, and up vectors
    //DirectX::XMVECTOR forward = DirectX::XMVector3TransformCoord(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), rotationMatrix);
    //DirectX::XMVECTOR right = DirectX::XMVector3TransformCoord(DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), rotationMatrix);
    //DirectX::XMVECTOR up = DirectX::XMVector3TransformCoord(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), rotationMatrix);

    //// Calculate the new camera target
    //DirectX::XMVECTOR cameraTarget = DirectX::XMVectorAdd(XMLoadFloat3(&cameraPosition), forward);

    //// Create the view matrix
    //DirectX::XMMATRIX view = DirectX::XMMatrixLookAtRH(XMLoadFloat3(&cameraPosition), cameraTarget, up);

    //DirectX::XMMATRIX proj = DirectX::XMMatrixPerspectiveFovRH(
    //    Constants::DegreesToRadians(90),
    //    static_cast<float>(_viewportWidth) / static_cast<float>(_viewportHeight),
    //    0.1f,
    //    400
    //);

    //DirectX::XMMATRIX viewProjection = DirectX::XMMatrixMultiply(view, proj);
    //_renderingApplication->SetPerFrameConstantBuffer(viewProjection);
    //_renderingApplication->SetCameraConstantBuffer(cameraPosition);
}

void World::PeriodicUpdate(float deltaTime)
{
}

void World::Render()
{
    _worldHierarchy.Render();
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
    _worldHierarchy.Clear();
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
    _universe->SaveWorld(filePath);
    return true;
}

bool World::LoadWorld(std::string filePath)
{
    if (filePath != "")
        _universe->LoadWorldSingle(filePath);
    else // load new world
    {
        // TODO FIX RETURN REF AND NOT POINTER
        auto ecs = _renderingApplication->GetECS();

        auto camera = ecs->CreateEntity();
        DirectX::XMFLOAT3 cameraStartPosition = { 0.0f, 3.0f, 10.0f };
        DirectX::XMFLOAT3 cameraStartRotation = { 0.0f,  (float)Constants::DegreesToRadians(180), 0.0f };

        auto cameraComponent = CameraComponent(cameraStartRotation, 20.0f, 3.0f);
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

        // Plugins for system bundles
        //TODO TEMP MONITORING SYSTEM, WILL NEED TO DO EDITOR PREPROCESSORS
        ecs->AddSystem<EntityMonitoringSystem>(ecs);
        ecs->AddSystem<LightingSystem>(
            _renderingApplication->GetApplicationDevice(), _renderingApplication->GetApplicationDeviceContext(), ecs);
        ecs->AddSystem<CameraSystem>(_renderingApplication, ecs);
    }

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


