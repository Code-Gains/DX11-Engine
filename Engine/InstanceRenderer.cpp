#include "InstanceRenderer.hpp"
#include <Cube.hpp>

void InstanceRenderer::CreateConstantBuffers()
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

    desc.ByteWidth = sizeof(MaterialConstantBuffer);
    _device->CreateBuffer(&desc, nullptr, &_materialConstantBuffer);

    desc.ByteWidth = sizeof(InstanceConstantBuffer) * _batchSize;
    _device->CreateBuffer(&desc, nullptr, &_instanceConstantBuffer);
}

InstanceRenderer::InstanceRenderer(int batchSize) : _batchSize(batchSize)
{

}

InstanceRenderer::InstanceRenderer(ID3D11Device* device,
    ID3D11DeviceContext* deviceContext,
    int batchSize) : _device(device), _deviceContext(deviceContext)
{
    std::cout << device <<std::endl;
    CreateConstantBuffers();
}

void InstanceRenderer::AddInstance(int poolKey, int entityId, const InstanceConstantBuffer& instanceData)
{
    auto it = _instancePools.find(poolKey);
    if (it != _instancePools.end())
    {
        it->second.entityIdToInstanceIndex[entityId] = it->second.instances.size();
        it->second.instances.push_back(instanceData);
        it->second.instanceCount++;
    }
}

void InstanceRenderer::UpdateInstanceData(int poolKey, int entityId, const InstanceConstantBuffer& newData)
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

void InstanceRenderer::RemoveInstance(int poolKey, int entityId)
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

void InstanceRenderer::RemoveAllInstances()
{
    for (auto& instancePoolPair : _instancePools)
    {
        InstancePool& instancePool = instancePoolPair.second;
        instancePool.entityIdToInstanceIndex.clear();
        instancePool.instances.clear();
        instancePool.instanceCount = 0;
    }
}


int InstanceRenderer::GetOwnershipCount() const
{
    int totalCount = 0;
    for (auto poolPair : _instancePools)
    {
        totalCount += poolPair.second.instanceCount;
    }
    return totalCount;
}

InstanceConstantBuffer::InstanceConstantBuffer()
{
}

InstanceConstantBuffer::InstanceConstantBuffer(const DirectX::XMMATRIX& worldMatrix) : worldMatrix(worldMatrix)
{
}

InstanceConstantBuffer::InstanceConstantBuffer(
    const DirectX::XMMATRIX& worldMatrix,
    const MaterialComponent& materialComponent) : worldMatrix(worldMatrix), material(materialComponent)
{
}
