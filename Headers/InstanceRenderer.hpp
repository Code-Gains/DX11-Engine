#pragma once
#include <vector>
#include <d3d11_2.h>
#include "Definitions.hpp"
#include "ConstantBufferDefinitions.hpp"
#include "VertexType.hpp"
#include "Logging.hpp"
#include <unordered_map>
#include <iostream>
#include <array>
#include <algorithm>


struct InstanceConstantBuffer
{
    DirectX::XMMATRIX worldMatrix;
};


class InstanceRenderer : IDebuggable
{
public:
    struct VertexBufferPool
    {
        WRL::ComPtr<ID3D11Buffer> vertexBuffer = nullptr;
        WRL::ComPtr<ID3D11Buffer> indexBuffer = nullptr;
        UINT vertexCount = 0;
        UINT indexCount = 0;
        UINT instanceCount = 0;
        std::vector<InstanceConstantBuffer> instances;
        //std::vector<UINT> indices;
    };

private:
    std::unordered_map<size_t, VertexBufferPool> _vertexBufferPools;

public:
    InstanceRenderer();
    void AddInstance(const InstanceConstantBuffer& instanceData, size_t bufferKey);
    void UpdateInstanceData(int instanceIndex, const InstanceConstantBuffer& newData);
    void RemoveInstance(int instanceIndex);
    void RenderInstances(ID3D11DeviceContext* deviceContext, ID3D11Buffer* perObjectConstantBuffer, ID3D11Buffer* _instanceConstantBuffer);
    int GetOwnershipCount() const override;

    template <typename VertexType>
    bool InitializeVertexBufferPool(ID3D11Device* device, size_t bufferKey, const std::vector<VertexType>& vertices, std::vector<UINT>& indices)
    {
        std::cout << vertices.size() << std::endl;
        // Init Buffer Descriptions
        D3D11_BUFFER_DESC vertexBufferDesc;
        ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
        vertexBufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
        vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertices.size();
        vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vertexBufferDesc.CPUAccessFlags = 0;
        vertexBufferDesc.MiscFlags = 0;

        D3D11_BUFFER_DESC indexBufferDesc;
        ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
        indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        indexBufferDesc.ByteWidth = sizeof(uint32_t) * indices.size();
        indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

        // Create Buffers
        VertexBufferPool newPool;

        D3D11_SUBRESOURCE_DATA vertexData;
        ZeroMemory(&vertexData, sizeof(vertexData));
        vertexData.pSysMem = vertices.data();
        HRESULT hr = device->CreateBuffer(&vertexBufferDesc, &vertexData, newPool.vertexBuffer.GetAddressOf());
        if (FAILED(hr))
        {
            std::cerr << "Could not create Vertex Buffer for a pool!\n";
            return false;
        }

        D3D11_SUBRESOURCE_DATA indexData;
        ZeroMemory(&indexData, sizeof(indexData));
        indexData.pSysMem = indices.data();
        hr = device->CreateBuffer(&indexBufferDesc, &indexData, newPool.indexBuffer.GetAddressOf());
        if (FAILED(hr))
        {
            std::cerr << "Could not create Index Buffer for a pool!\n";
            return false;
        }

        // Set Pool
        newPool.vertexCount = vertices.size();
        newPool.indexCount = indices.size();
        _vertexBufferPools[bufferKey] = newPool;
        return true;
    }
};