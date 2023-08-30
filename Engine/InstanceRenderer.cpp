#include "InstanceRenderer.hpp"
#include <Cube.hpp>

InstanceRenderer::InstanceRenderer(int batchSize) : _batchSize(batchSize)
{

}


void InstanceRenderer::AddInstance(const InstanceConstantBuffer& instanceData, int bufferKey)
{
    auto it = _instancePools.find(bufferKey);
    if (it != _instancePools.end())
    {
        it->second.instances.push_back(instanceData);
        it->second.instanceCount++;
    }
}

void InstanceRenderer::UpdateInstanceData(int bufferKey, int instanceIndex, const InstanceConstantBuffer& newData)
{
    if (_instancePools.find(bufferKey) != _instancePools.end())
    {
        if (instanceIndex >= 0 && instanceIndex < _instancePools[bufferKey].instances.size())
        {
            _instancePools[bufferKey].instances[instanceIndex] = newData;
        }
    }
}

void InstanceRenderer::RemoveInstance(int bufferKey, int instanceIndex)
{
    if (_instancePools.find(bufferKey) != _instancePools.end())
    {
        if (instanceIndex >= 0 && instanceIndex < _instancePools[bufferKey].instanceCount)
        {
            _instancePools[bufferKey].instances.erase(_instancePools[bufferKey].instances.begin() + instanceIndex);
            _instancePools[bufferKey].instanceCount--;
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