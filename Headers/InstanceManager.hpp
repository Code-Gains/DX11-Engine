#pragma once
#include <vector>
#include <d3d11_2.h>
#include "Definitions.hpp"
#include "ConstantBufferDefinitions.hpp"
#include <unordered_map>

struct VertexBufferPool
{
    WRL::ComPtr<ID3D11Buffer> buffer;
    UINT vertexCount;
    UINT instanceCount;
    std::vector<InstanceConstantBuffer> instances;
};


class InstanceManager
{
    std::unordered_map<size_t, VertexBufferPool> _vertexBufferPools;
public:
    InstanceManager();
    void AddInstance(const InstanceConstantBuffer& instanceData);
    void UpdateInstanceData(int instanceIndex, const InstanceConstantBuffer& newData);
    void RemoveInstance(int instanceIndex);
    void RenderInstances(ID3D11DeviceContext* deviceContext, ID3D11Buffer* perObjectConstantBuffer);
};