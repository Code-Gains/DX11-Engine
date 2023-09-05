#include "InstanceRenderer.hpp"
#include <Cube.hpp>

InstanceRenderer::InstanceRenderer(int batchSize) : _batchSize(batchSize)
{

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