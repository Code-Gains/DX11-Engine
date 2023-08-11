#pragma once
#include <vector>
#include <d3d11_2.h>
#include "Definitions.hpp"
#include "ConstantBufferDefinitions.hpp"
#include "VertexType.hpp"
#include <unordered_map>


struct InstanceConstantBuffer
{
    DirectX::XMFLOAT4X4 worldMatrix;
};

class InstanceManager
{
public:
    struct VertexBufferPool
    {
        WRL::ComPtr<ID3D11Buffer> vertexBuffer;
        WRL::ComPtr<ID3D11Buffer> indexBuffer;
        UINT vertexCount;
        UINT indexCount;
        UINT instanceCount;
        std::vector<InstanceConstantBuffer> instances;
        std::vector<UINT> indices;
    };

private:
    std::unordered_map<size_t, VertexBufferPool> _vertexBufferPools;

public:
    InstanceManager();
    void InitializeVertexBufferPool(size_t bufferKey, const std::vector<VertexPositionNormalUv>& vertices, const std::vector<UINT>& indices);
    void AddInstance(const InstanceConstantBuffer& instanceData, size_t bufferKey);
    void UpdateInstanceData(int instanceIndex, const InstanceConstantBuffer& newData);
    void RemoveInstance(int instanceIndex);
    void RenderInstances(ID3D11DeviceContext* deviceContext);
};