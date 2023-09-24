#include "InstanceRenderer.hpp"
#include <Cube.hpp>

void InstanceRenderer::CreateConstantBuffers()
{
    D3D11_BUFFER_DESC desc{};
    desc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    /*desc.ByteWidth = sizeof(PerFrameConstantBuffer);
    _device->CreateBuffer(&desc, nullptr, &_perFrameConstantBuffer);

    desc.ByteWidth = sizeof(CameraConstantBuffer);
    _device->CreateBuffer(&desc, nullptr, &_cameraConstantBuffer);

    desc.ByteWidth = sizeof(LightConstantBuffer);
    _device->CreateBuffer(&desc, nullptr, &_lightConstantBuffer);

    desc.ByteWidth = sizeof(MaterialConstantBuffer);
    _device->CreateBuffer(&desc, nullptr, &_materialConstantBuffer);*/

    desc.ByteWidth = sizeof(PerObjectConstantBuffer);
    _device->CreateBuffer(&desc, nullptr, &_perObjectConstantBuffer);

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

void InstanceRenderer::AddInstance(const InstanceConstantBuffer& instanceData, int poolKey)
{
    auto it = _instancePools.find(poolKey);
    if (it != _instancePools.end())
    {
        it->second.instances.push_back(instanceData);
        it->second.instanceCount++;
    }
}

void InstanceRenderer::UpdateInstanceData(int poolKey, int instanceIndex, const InstanceConstantBuffer& newData)
{
    if (_instancePools.find(poolKey) != _instancePools.end())
    {
        if (instanceIndex >= 0 && instanceIndex < _instancePools[poolKey].instances.size())
        {
            _instancePools[poolKey].instances[instanceIndex] = newData;
        }
    }
}

void InstanceRenderer::RemoveInstance(int poolKey, int instanceIndex)
{
    if (_instancePools.find(poolKey) != _instancePools.end())
    {
        if (instanceIndex >= 0 && instanceIndex < _instancePools[poolKey].instanceCount)
        {
            _instancePools[poolKey].instances.erase(_instancePools[poolKey].instances.begin() + instanceIndex);
            _instancePools[poolKey].instanceCount--;
        }
    }
}

void InstanceRenderer::RemoveAllInstances()
{
    for (auto& instancePoolPair : _instancePools)
    {
        InstancePool& instancePool = instancePoolPair.second;
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