#include "World.hpp"

World::World()
{
}

bool World::Initialize(HWND win32Window, ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
    _win32Window = win32Window;
	_lightConstantBufferData.Position = { -50.0f, 150.0f, 150.0f, 0.0f };
	_lightConstantBufferData.Ambient = { 0.4f, 0.4f, 0.4f, 1.0f };
	_lightConstantBufferData.Diffuse = { 0.5f, 0.5f, 0.5f, 1.0f };
	_lightConstantBufferData.Specular = { 0.0f, 0.0f, 0.0f, 1.0f };

	_instanceRenderer = InstanceRendererSystem(device, deviceContext);
	return true;
}

void World::Update(float deltaTime)
{
    // DirectX namespace contains overloads for vector and float multiplication
    using namespace DirectX;

    float cameraMoveSpeed = 10.0f;
    float cameraRotationSpeed = 5.0f;

    static DirectX::XMFLOAT3 cameraPosition = { 0.0f, 0.0f, 10.0f };
    static DirectX::XMFLOAT3 cameraRotation = { 0.0f,  (float)Constants::DegreesToRadians(180), 0.0f };


    // Camera Movement

    if (GetAsyncKeyState('W') & 0x8000)
    {
        // Move camera forward
        DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(cameraRotation.x, cameraRotation.y, cameraRotation.z);
        DirectX::XMVECTOR forward = DirectX::XMVector3TransformCoord(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), rotationMatrix);
        DirectX::XMVECTOR newPosition = DirectX::XMLoadFloat3(&cameraPosition) + forward * cameraMoveSpeed * deltaTime;
        XMStoreFloat3(&cameraPosition, newPosition);
    }
    if (GetAsyncKeyState('S') & 0x8000)
    {
        // Move camera backward
        DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(cameraRotation.x, cameraRotation.y, cameraRotation.z);
        DirectX::XMVECTOR forward = DirectX::XMVector3TransformCoord(DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f), rotationMatrix);
        DirectX::XMVECTOR newPosition = DirectX::XMLoadFloat3(&cameraPosition) + forward * cameraMoveSpeed * deltaTime;
        XMStoreFloat3(&cameraPosition, newPosition);
    }
    if (GetAsyncKeyState('A') & 0x8000)
    {
        // Move camera left
        DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(cameraRotation.x, cameraRotation.y, cameraRotation.z);
        DirectX::XMVECTOR right = DirectX::XMVector3TransformCoord(DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), rotationMatrix);
        DirectX::XMVECTOR newPosition = DirectX::XMLoadFloat3(&cameraPosition) + right * cameraMoveSpeed * deltaTime;
        XMStoreFloat3(&cameraPosition, newPosition);
    }
    if (GetAsyncKeyState('D') & 0x8000)
    {
        // Move camera right
        DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(cameraRotation.x, cameraRotation.y, cameraRotation.z);
        DirectX::XMVECTOR right = DirectX::XMVector3TransformCoord(DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), rotationMatrix);
        DirectX::XMVECTOR newPosition = DirectX::XMLoadFloat3(&cameraPosition) - right * cameraMoveSpeed * deltaTime;
        XMStoreFloat3(&cameraPosition, newPosition);
    }


    static float lastMouseX = 0.0f;
    static float lastMouseY = 0.0f;

    bool isRightMouseDown = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
    static bool wasRightMouseDown = false; // Keep track of previous right mouse button state
    POINT cursorPos;
    GetCursorPos(&cursorPos);
    ScreenToClient(_win32Window, &cursorPos);
    int mouseX = cursorPos.x;
    int mouseY = cursorPos.y;

    if (isRightMouseDown) {
        if (!wasRightMouseDown) {
            // Right mouse button was just pressed, initialize previous mouse position
            lastMouseX = mouseX;
            lastMouseY = mouseY;
        }

        // Calculate the change in mouse position since the last frame
        int deltaX = mouseX - lastMouseX;
        int deltaY = mouseY - lastMouseY;

        // Update the camera rotation based on the change in mouse position
        cameraRotation.y -= deltaX * cameraRotationSpeed * deltaTime;
        cameraRotation.x += deltaY * cameraRotationSpeed * deltaTime;

        // Clamp pitch to prevent camera flipping
        cameraRotation.x = (-DirectX::XM_PIDIV2, min(DirectX::XM_PIDIV2, cameraRotation.x));

        // Update the previous mouse position
        lastMouseX = mouseX;
        lastMouseY = mouseY;
    }

    wasRightMouseDown = isRightMouseDown;

    DirectX::XMVECTOR camPos = XMLoadFloat3(&cameraPosition);

    DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(cameraRotation.x, cameraRotation.y, cameraRotation.z);

    // Calculate the forward, right, and up vectors
    DirectX::XMVECTOR forward = DirectX::XMVector3TransformCoord(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), rotationMatrix);
    DirectX::XMVECTOR right = DirectX::XMVector3TransformCoord(DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), rotationMatrix);
    DirectX::XMVECTOR up = DirectX::XMVector3TransformCoord(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), rotationMatrix);

    // Calculate the new camera target
    DirectX::XMVECTOR cameraTarget = DirectX::XMVectorAdd(XMLoadFloat3(&cameraPosition), forward);

    // Create the view matrix
    DirectX::XMMATRIX view = DirectX::XMMatrixLookAtRH(XMLoadFloat3(&cameraPosition), cameraTarget, up);

    DirectX::XMMATRIX proj = DirectX::XMMatrixPerspectiveFovRH(Constants::DegreesToRadians(90),
        static_cast<float>(_viewportWidth) / static_cast<float>(_viewportHeight),
        0.1f,
        400);
    DirectX::XMMATRIX viewProjection = DirectX::XMMatrixMultiply(view, proj);
    DirectX::XMStoreFloat4x4(&_perFrameConstantBufferData.viewProjectionMatrix, viewProjection);

    _cameraConstantBufferData.cameraPosition = cameraPosition;

    UpdateDirtyRenderableTransforms();
}

void World::PeriodicUpdate(float deltaTime)
{
}

void World::Render()
{
	_instanceRenderer.RenderInstances<VertexPositionNormalUv>(_instancePools, _perFrameConstantBufferData, _cameraConstantBufferData, _lightConstantBufferData, MaterialConstantBuffer());
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
    auto newEntity = Entity();
    _entities.push_back(newEntity);
    return newEntity;
}

bool World::LoadWorld(std::string fileName)
{
	if(fileName != "")
	{
	}

	// Create a few meshes, materials, a light source

	auto blueCube = Entity();
    auto redCube = Entity();

    auto transformComponent = TransformComponent(DirectX::XMFLOAT3 {-1, 0, 0}, DirectX::XMFLOAT3{ 0, 0, 0 }, DirectX::XMFLOAT3{ 1, 1, 1 });
    AddComponent(blueCube.GetId(), transformComponent);

    auto transformComponent2 = TransformComponent(DirectX::XMFLOAT3{ 1, 0, 0 }, DirectX::XMFLOAT3{ 0, 0, 0 }, DirectX::XMFLOAT3{ 1, 1, 1 });
    AddComponent(redCube.GetId(), transformComponent2);

	auto cube = Cube();
	auto meshComponent = MeshComponent(cube.GetVertices(), cube.GetIndices(), _nextPoolId);
    AddComponent(blueCube.GetId(), meshComponent);
    AddComponent(redCube.GetId(), meshComponent);


	DirectX::XMFLOAT4 ambient{ 0.0f, 0.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT4 diffuse{ 0.0f, 0.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT4 specular{ 0.0f, 0.0f, 1.0f, 1.0f };
	float shininess = 3;
	auto blueMaterial = MaterialComponent(ambient, diffuse, specular, shininess);

    ambient = { 1.0f, 0.0f, 0.0f, 1.0f };
    diffuse = { 1.0f, 0.0f, 0.0f, 1.0f };
    specular = { 1.0f, 0.0f, 0.0f, 1.0f };
    auto redMaterial = MaterialComponent(ambient, diffuse, specular, shininess);

    AddComponent(blueCube.GetId(), blueMaterial);
    AddComponent(redCube.GetId(), redMaterial);


	// Set up instance renderer
	std::vector<VertexPositionNormalUv> vertices = cube.GetVertices();
	std::vector<UINT> indices = cube.GetIndices();

    InstanceRendererSystem::InstancePool instancePool = _instanceRenderer.CreateInstancePool<VertexPositionNormalUv>(blueCube.GetId(), meshComponent);
    LinkRenderableInstancePool(instancePool);

    _entities.push_back(blueCube);
    _entities.push_back(redCube);

	return true;
}

void World::UpdateViewportDimensions(int32_t width, int32_t height)
{
	_viewportWidth = width;
	_viewportHeight = height;
}

void World::AddComponent(int entityId, const TransformComponent& component)
{
    _transformComponentIndices[entityId] = _transformComponents.size();
    _transformComponents.push_back(component);
}

void World::AddComponent(int entityId, const MeshComponent& component)
{
    _meshComponentIndices[entityId] = _meshComponents.size();
    _meshComponents.push_back(component);
}

void World::AddComponent(int entityId, const MaterialComponent& component)
{
    _materialComponentIndices[entityId] = _materialComponents.size();
    _materialComponents.push_back(component);
}

void World::AddComponent(int entityId, const LightComponent& component)
{
    _lightComponentIndices[entityId] = _lightComponents.size();
    _lightComponents.push_back(component);
}

void World::AddComponent(int entityId, const CameraComponent& component)
{
    _cameraComponentIndices[entityId] = _cameraComponents.size();
    _cameraComponents.push_back(component);
}

void World::RemoveComponent(int entityId, const TransformComponent& component)
{
    auto it = _transformComponentIndices.find(entityId);
    if (it != _transformComponentIndices.end())
    {
        _transformComponentIndices.erase(it);
    }
}

void World::RemoveComponent(int entityId, const MeshComponent& component)
{
    auto it = _meshComponentIndices.find(entityId);
    if (it != _meshComponentIndices.end())
    {
        _meshComponentIndices.erase(it);
    }
}

void World::RemoveComponent(int entityId, const MaterialComponent& component)
{
    auto it = _materialComponentIndices.find(entityId);
    if (it != _materialComponentIndices.end())
    {
        _materialComponentIndices.erase(it);
    }
}

void World::RemoveComponent(int entityId, const LightComponent& component)
{
    auto it = _lightComponentIndices.find(entityId);
    if (it != _lightComponentIndices.end())
    {
        _lightComponentIndices.erase(it);
    }
}

void World::RemoveComponent(int entityId, const CameraComponent& component)
{
    auto it = _cameraComponentIndices.find(entityId);
    if (it != _cameraComponentIndices.end())
    {
        _cameraComponentIndices.erase(it);
    }
}

void World::LinkRenderableInstancePool(const InstanceRendererSystem::InstancePool& instancePool)
{
    _instancePools[_nextPoolId] = instancePool;
    _nextPoolId++;
}

void World::AddRenderableInstance(
    int poolKey,
    int entityId,
    const InstanceConstantBuffer& instanceData)
{
    auto it = _instancePools.find(poolKey);
    if (it != _instancePools.end())
    {
        it->second.entityIdToInstanceIndex[entityId] = it->second.instances.size();
        it->second.instances.push_back(instanceData);
        it->second.instanceCount++;
    }
}

void World::UpdateRenderableInstanceData(
    int poolKey,
    int entityId,
    const InstanceConstantBuffer& newData)
{
    if (_instancePools.find(poolKey) != _instancePools.end())
    {
        auto& entityIdToInstances = _instancePools[poolKey].entityIdToInstanceIndex;
        if (entityIdToInstances.find(entityId) != entityIdToInstances.end())
        {
            auto instanceIndex = entityIdToInstances[entityId];
            _instancePools[poolKey].instances[instanceIndex] = newData;
            return;
        }
        AddRenderableInstance(poolKey, entityId, newData);
    }
}

void World::RemoveRenderableInstance(
    int poolKey,
    int entityId)
{
    if (_instancePools.find(poolKey) != _instancePools.end())
    {
        auto& entityIdToInstance = _instancePools[poolKey].entityIdToInstanceIndex;
        auto& instances = _instancePools[poolKey].instances;
        if (entityIdToInstance.find(entityId) != entityIdToInstance.end())
        {
            auto instanceIndex = entityIdToInstance[entityId];
            entityIdToInstance.erase(entityId);
            instances.erase(instances.begin() + instanceIndex);
            _instancePools[poolKey].instanceCount--;
        }
    }
}

void World::RemoveAllRenderableInstances()
{
    for (auto& instancePoolPair : _instancePools)
    {
        InstanceRendererSystem::InstancePool& instancePool = instancePoolPair.second;
        instancePool.entityIdToInstanceIndex.clear();
        instancePool.instances.clear();
        instancePool.instanceCount = 0;
    }
}

void World::UpdateDirtyRenderableTransforms()
{
    for (auto& entity : _entities)
    {
        if (_transformComponentIndices.find(entity.GetId()) == _transformComponentIndices.end())
            return;
        if (_meshComponentIndices.find(entity.GetId()) == _meshComponentIndices.end())
            return;
        if (_materialComponentIndices.find(entity.GetId()) == _materialComponentIndices.end())
            return;

        int transformIndex = _transformComponentIndices[entity.GetId()];
        TransformComponent& transform = _transformComponents[transformIndex];
        if (!transform.IsDirty())
            return;

        int materialIndex = _materialComponentIndices[entity.GetId()];
        MaterialComponent& material = _materialComponents[materialIndex];

        int meshIndex = _meshComponentIndices[entity.GetId()];
        MeshComponent& mesh = _meshComponents[meshIndex];

        UpdateRenderableInstanceData(mesh.GetInstancePoolIndex(), entity.GetId(), InstanceConstantBuffer(transform.GetWorldMatrix(), material)); // MUST manage meshes
    }

}

