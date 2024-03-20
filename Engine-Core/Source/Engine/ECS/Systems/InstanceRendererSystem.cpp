#include "InstanceRendererSystem.hpp"

void InstanceRendererSystem::CreateConstantBuffers()
{
    D3D11_BUFFER_DESC desc{};
    desc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    desc.ByteWidth = sizeof(PerFrameConstantBuffer);
    _device->CreateBuffer(&desc, nullptr, &_perFrameConstantBuffer);

    desc.ByteWidth = sizeof(CameraConstantBuffer);
    _device->CreateBuffer(&desc, nullptr, &_cameraConstantBuffer);

    desc.ByteWidth = sizeof(LightConstantBuffer);
    _device->CreateBuffer(&desc, nullptr, &_lightConstantBuffer);

    desc.ByteWidth = sizeof(InstanceConstantBuffer) * _batchSize;
    _device->CreateBuffer(&desc, nullptr, &_instanceConstantBuffer);
}

InstanceRendererSystem::InstanceRendererSystem(int batchSize) : _batchSize(batchSize)
{

}

InstanceRendererSystem::InstanceRendererSystem(ID3D11Device* device,
    ID3D11DeviceContext* deviceContext,
    ECS* ecs,
    int batchSize) : _device(device), _deviceContext(deviceContext), _ecs(ecs)
{
    CreateConstantBuffers();
}

void InstanceRendererSystem::AddInstance(int poolKey, int instanceId, const InstanceConstantBuffer& instanceData)
{
    auto it = _instancePools.find(poolKey);
    if (it != _instancePools.end())
    {
        it->second.entityIdToInstanceIndex[instanceId] = it->second.instances.size();
        it->second.instances.push_back(instanceData);
        it->second.instanceCount++;
    }
}

void InstanceRendererSystem::UpdateInstanceData(int poolKey, int instanceId, const InstanceConstantBuffer& newData)
{
    if (_instancePools.find(poolKey) != _instancePools.end())
    {
        auto& entityIdToInstances = _instancePools[poolKey].entityIdToInstanceIndex;
        if (entityIdToInstances.find(instanceId) != entityIdToInstances.end())
        {
            auto instanceIndex = entityIdToInstances[instanceId];
            _instancePools[poolKey].instances[instanceIndex] = newData;
            return;
        }
        AddInstance(poolKey, instanceId, newData);
        std::cout << "Added instance: " << instanceId << std::endl;
    }
}

void InstanceRendererSystem::RemoveInstance(int poolKey, int entityId)
{
    if (_instancePools.find(poolKey) != _instancePools.end())
    {
        auto& entityIdToInstance = _instancePools[poolKey].entityIdToInstanceIndex;
        auto& instances = _instancePools[poolKey].instances;
        auto& instanceCount = _instancePools[poolKey].instanceCount;
        if (entityIdToInstance.find(entityId) != entityIdToInstance.end())
        {
            auto instanceIndex = entityIdToInstance[entityId];
            for (auto& pair : entityIdToInstance)
            {
                if (pair.second > instanceIndex)
                {
                    pair.second--;
                }
            }
            entityIdToInstance.erase(entityId);
            instances.erase(instances.begin() + instanceIndex);
            _instancePools[poolKey].instanceCount--;
        }
    }
}

void InstanceRendererSystem::UpdateDirtyInstances()
{
    // querry returns a vector of tuples that contain component vectors
    auto componentQueryResult = _ecs->QueryComponentVectors<TransformComponent, MeshComponent, MaterialComponent>();
    // iterate
    for (auto& tuple : componentQueryResult)
    {
        // will need to optimize TODO (or actually never mind I think, we need entity indexes anyway)
        auto& transforms = std::get<0>(tuple);
        auto& meshes = std::get<1>(tuple);
        auto& materials = std::get<2>(tuple);

        // all vectors should be the same length (ALWAYS!!! Otherwise it should result in a crash)

        auto& rawTransforms = *transforms->GetRawVector();
        auto& rawMeshes = *meshes->GetRawVector();
        auto& rawMaterials = *materials->GetRawVector();

        for (auto& entityToComponent : transforms->GetEntityToIndex())
        {
            auto id = entityToComponent.first;
            auto idx = entityToComponent.second;
            if (rawTransforms[idx].IsDirty() || rawMaterials[idx].IsDirty())
            {
                UpdateInstanceData(
                    rawMeshes[idx].GetInstancePoolIndex(),
                    id,
                    InstanceConstantBuffer(
                        rawTransforms[idx].GetWorldMatrix(),
                        rawMaterials[idx].GetMaterialConstantBuffer()
                    ));
                rawTransforms[idx].SetIsDirty(false);
                rawMaterials[idx].SetIsDirty(false);
            }
        }
    }

    //if (_transformComponentIndices.find(entity) == _transformComponentIndices.end())
    //    continue;

    //int transformIndex = _transformComponentIndices[entity];
    //TransformComponent& transform = _transformComponents[transformIndex];

    //if (_materialComponentIndices.find(entity) == _materialComponentIndices.end())
    //    continue;

    //int materialIndex = _materialComponentIndices[entity];
    //MaterialComponent& material = _materialComponents[materialIndex];

    //if (!transform.IsDirty() && !material.IsDirty())
    //    continue;

    //if (_meshComponentIndices.find(entity) == _meshComponentIndices.end())
    //    continue;

    //int meshIndex = _meshComponentIndices[entity];
    //MeshComponent& mesh = _meshComponents[meshIndex];

    //UpdateRenderableInstanceData(mesh.GetInstancePoolIndex(), entity, InstanceConstantBuffer(transform.GetWorldMatrix(), material.GetMaterialConstantBuffer())); // MUST manage meshes
    //transform.SetIsDirty(false);
    //material.SetIsDirty(false);
    // update
}

void InstanceRendererSystem::RemoveAllInstances()
{
    for (auto& instancePoolPair : _instancePools)
    {
        InstancePool& instancePool = instancePoolPair.second;
        instancePool.entityIdToInstanceIndex.clear();
        instancePool.instances.clear();
        instancePool.instanceCount = 0;
    }
}

InstanceConstantBuffer::InstanceConstantBuffer()
{
}

InstanceConstantBuffer::InstanceConstantBuffer(const DirectX::XMMATRIX& worldMatrix) : worldMatrix(worldMatrix)
{
}

InstanceConstantBuffer::InstanceConstantBuffer(
    const DirectX::XMMATRIX& worldMatrix,
    const MaterialConstantBuffer& materialConstantBuffer) : worldMatrix(worldMatrix), material(materialConstantBuffer)
{
}
