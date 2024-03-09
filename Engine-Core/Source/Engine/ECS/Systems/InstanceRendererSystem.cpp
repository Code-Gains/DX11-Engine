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
    int batchSize) : _device(device), _deviceContext(deviceContext)
{
    CreateConstantBuffers();
}

void InstanceRendererSystem::AddInstance(int poolKey, int entityId, const InstanceConstantBuffer& instanceData)
{
    auto it = _instancePools.find(poolKey);
    if (it != _instancePools.end())
    {
        it->second.entityIdToInstanceIndex[entityId] = it->second.instances.size();
        it->second.instances.push_back(instanceData);
        it->second.instanceCount++;
    }
}

void InstanceRendererSystem::UpdateInstanceData(int poolKey, int entityId, const InstanceConstantBuffer& newData)
{
    if (_instancePools.find(poolKey) != _instancePools.end())
    {
        auto& entityIdToInstances = _instancePools[poolKey].entityIdToInstanceIndex;
        if(entityIdToInstances.find(entityId) != entityIdToInstances.end())
        {
            auto instanceIndex = entityIdToInstances[entityId];
            _instancePools[poolKey].instances[instanceIndex] = newData;
        }
    }
}

void InstanceRendererSystem::RemoveInstance(int poolKey, int entityId)
{
    auto zeroedMatrix = DirectX::XMMatrixSet(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    if (_instancePools.find(poolKey) != _instancePools.end())
    {
        auto& entityIdToInstance = _instancePools[poolKey].entityIdToInstanceIndex;
        auto& instances = _instancePools[poolKey].instances;
        auto& instanceCount = _instancePools[poolKey].instanceCount;
        if (entityIdToInstance.find(entityId) != entityIdToInstance.end())
        {
            auto instanceIndex = entityIdToInstance[entityId];
            instances[instanceIndex].worldMatrix = zeroedMatrix;
        }
    }
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
