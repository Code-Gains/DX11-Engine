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
    _worldHierarchy = WorldHierarchy(this);
	return true;
}

void World::Update(float deltaTime)
{
    _worldHierarchy.Update(deltaTime);

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
    _worldHierarchy.Render();
	_instanceRenderer.RenderInstances<VertexPositionNormalUv>(_instancePools, _perFrameConstantBufferData, _cameraConstantBufferData, _lightConstantBufferData, MaterialConstantBuffer());
}

int World::GetNextEntityId() const
{
    return _nextEntityId;
}

int World::GetNextComponentId() const
{
    return _nextComponentId;
}

int World::GetNextPoolId() const
{
    return _nextPoolId;
}

void World::IncrementEntityId()
{
    _nextEntityId++;
}

void World::IncrementPoolId()
{
    _nextPoolId;
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

void World::AddEntity(Entity entity)
{
    _entities.push_back(entity);
}

Entity World::CreateEntity()
{
    auto newEntity = Entity();
    _entities.push_back(newEntity);
    return newEntity;
}

void World::RemoveEntity(int id)
{

}

bool World::LoadWorld(std::string fileName)
{
    LinkEngineInstancePools();
	if(fileName != "")
	{
        // handle files later
	}

    /*std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    int width = 10;
    int height = 10;
    float spacing = 1.5f;*/

   /* for (int column = 0; column < width; column++)
    {
        for (int row = 0; row < height; row++)
        {
            auto cubeEntity = Entity();

            DirectX::XMFLOAT3 position{
                column * spacing - (width * spacing) / 2 + spacing / 2,
                row * spacing - (height * spacing) / 2 + spacing / 2,
                0.0f
            };

            auto transformComponent = TransformComponent(_nextComponentId,
                position, DirectX::XMFLOAT3{ 0, 0, 0 }, DirectX::XMFLOAT3{ 1, 1, 1 }
            );
            AddComponent(cubeEntity.GetId(), transformComponent);

            auto meshComponent = MeshComponent(_nextComponentId, templateCube.GetVertices(), templateCube.GetIndices(), _nextPoolId);
            AddComponent(cubeEntity.GetId(), meshComponent);

            DirectX::XMFLOAT4 ambient{ dist(gen), dist(gen), dist(gen), 1.0f};
            DirectX::XMFLOAT4 diffuse{ dist(gen), dist(gen), dist(gen), 1.0f };
            DirectX::XMFLOAT4 specular{ dist(gen), dist(gen), dist(gen), 1.0f };
            float shininess = 3.0f;

            auto materialComponent = MaterialComponent(_nextComponentId, ambient, diffuse, specular, shininess);
            AddComponent(cubeEntity.GetId(), materialComponent);

            _entities.push_back(cubeEntity);
        }
    }

    LinkRenderableInstancePool(instancePool);*/

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
    _nextComponentId++;
}

void World::AddComponent(int entityId, const MeshComponent& component)
{
    _meshComponentIndices[entityId] = _meshComponents.size();
    _meshComponents.push_back(component);
    _nextComponentId++;
}

void World::AddComponent(int entityId, const MaterialComponent& component)
{
    _materialComponentIndices[entityId] = _materialComponents.size();
    _materialComponents.push_back(component);
    _nextComponentId++;
}

void World::AddComponent(int entityId, const LightComponent& component)
{
    _lightComponentIndices[entityId] = _lightComponents.size();
    _lightComponents.push_back(component);
    _nextComponentId++;
}

void World::AddComponent(int entityId, const CameraComponent& component)
{
    _cameraComponentIndices[entityId] = _cameraComponents.size();
    _cameraComponents.push_back(component);
    _nextComponentId++;
}

void World::RemoveTransformComponent(int entityId)
{
    auto it = _transformComponentIndices.find(entityId);
    if (it != _transformComponentIndices.end())
    {
       // _transformComponents.erase(_transformComponents.begin() + it->second);
        //_transformComponentIndices.erase(it);
        _freeTransforms.push_back(it->second);
    }
}

void World::RemoveMeshComponent(int entityId)
{
    auto it = _meshComponentIndices.find(entityId);
    if (it != _meshComponentIndices.end())
    {
       /* _meshComponents.erase(_meshComponents.begin() + it->second);
        _meshComponentIndices.erase(it);*/
        _freeMeshes.push_back(it->second);
    }
}

void World::RemoveMaterialComponent(int entityId)
{
    auto it = _materialComponentIndices.find(entityId);
    if (it != _materialComponentIndices.end())
    {
        /*_materialComponents.erase(_materialComponents.begin() + it->second);
        _materialComponentIndices.erase(it);*/
        _freeMaterials.push_back(it->second);
    }
}

void World::RemoveLightComponent(int entityId)
{
    auto it = _lightComponentIndices.find(entityId);
    if (it != _lightComponentIndices.end())
    {
        /*_lightComponents.erase(_lightComponents.begin() + it->second);
        _lightComponentIndices.erase(it);*/
        _freeLights.push_back(it->second);
    }
}

void World::RemoveCameraComponent(int entityId)
{
    auto it = _cameraComponentIndices.find(entityId);
    if (it != _cameraComponentIndices.end())
    {
        /*_cameraComponents.erase(_cameraComponents.begin() + it->second);
        _cameraComponentIndices.erase(it);*/
        _freeCameras.push_back(it->second);
    }
}

void World::LinkEngineInstancePools()
{
    auto cubeMesh = MeshComponent::GetPrimitiveMeshComponent(PrimitiveGeometryType3D::Cube);
    int cubeIndex = (int)PrimitiveGeometryType3D::Cube;
    InstanceRendererSystem::InstancePool cubePool =
        _instanceRenderer.CreateInstancePool<VertexPositionNormalUv>(cubeIndex, cubeMesh);
    LinkRenderableInstancePool(cubeIndex, cubePool);

    auto sphereMesh = MeshComponent::GetPrimitiveMeshComponent(PrimitiveGeometryType3D::Sphere);
    int sphereIndex = (int)PrimitiveGeometryType3D::Sphere;
    InstanceRendererSystem::InstancePool spherePool =
        _instanceRenderer.CreateInstancePool<VertexPositionNormalUv>(sphereIndex, sphereMesh);
    LinkRenderableInstancePool(sphereIndex, spherePool);

    auto cylinderMesh = MeshComponent::GetPrimitiveMeshComponent(PrimitiveGeometryType3D::Cylinder);
    int cylinderIndex = (int)PrimitiveGeometryType3D::Cylinder;
    InstanceRendererSystem::InstancePool cylinderPool =
        _instanceRenderer.CreateInstancePool<VertexPositionNormalUv>(cylinderIndex, cylinderMesh);
    LinkRenderableInstancePool(cylinderIndex, cylinderPool);

    auto pipeMesh = MeshComponent::GetPrimitiveMeshComponent(PrimitiveGeometryType3D::Pipe);
    int pipeIndex = (int)PrimitiveGeometryType3D::Pipe;
    InstanceRendererSystem::InstancePool pipePool =
        _instanceRenderer.CreateInstancePool<VertexPositionNormalUv>(pipeIndex, pipeMesh);
    LinkRenderableInstancePool(pipeIndex, pipePool);
}

void World::LinkRenderableInstancePool(int index, const InstanceRendererSystem::InstancePool& instancePool)
{
    _instancePools[index] = instancePool;
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
    auto zeroedMatrix = DirectX::XMMatrixSet(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    if (_instancePools.find(poolKey) != _instancePools.end())
    {
        std::cout << _instancePools[0].instances.size() << std::endl;
        auto& entityIdToInstance = _instancePools[poolKey].entityIdToInstanceIndex;
        auto& instances = _instancePools[poolKey].instances;
        auto& freeInstances = _instancePools[poolKey].freeInstances;
        auto& instanceCount = _instancePools[poolKey].instanceCount;
        auto& freeInstanceCount = _instancePools[poolKey].freeInstanceCount;
        if (entityIdToInstance.find(entityId) != entityIdToInstance.end())
        {
            auto instanceIndex = entityIdToInstance[entityId];
            //instances[instanceIndex].worldMatrix = zeroedMatrix;
            //freeInstances.push_back(instanceIndex);
            //freeInstanceCount++;
            //if ((float)freeInstanceCount / instanceCount > _deadDataCompactionTreshold)
            //{
            //    // COMPACT
            //}
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

        UpdateRenderableInstanceData(mesh.GetInstancePoolIndex(), entity.GetId(), InstanceConstantBuffer(transform.GetWorldMatrix(), material.GetMaterialConstantBuffer())); // MUST manage meshes
    }

}

