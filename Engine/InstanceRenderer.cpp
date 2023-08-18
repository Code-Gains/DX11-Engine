#include "InstanceRenderer.hpp"
#include <Cube.hpp>

InstanceRenderer::InstanceRenderer(int batchSize) : _batchSize(batchSize)
{

}


void InstanceRenderer::AddInstance(const InstanceConstantBuffer& instanceData, size_t bufferKey)
{
    auto it = _vertexBufferPools.find(bufferKey);
    if (it != _vertexBufferPools.end())
    {
        it->second.instances.push_back(instanceData);
        it->second.instanceCount++;
    }
}

void InstanceRenderer::UpdateInstanceData(int instanceIndex, const InstanceConstantBuffer& newData)
{

}

void InstanceRenderer::RemoveInstance(int instanceIndex)
{

}

int InstanceRenderer::GetOwnershipCount() const
{
    int totalCount = 0;
    for (auto poolPair : _vertexBufferPools)
    {
        totalCount += poolPair.second.instanceCount;
    }
    return totalCount;
}