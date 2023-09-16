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
#include <Cube.hpp>


struct InstanceConstantBuffer
{
    DirectX::XMMATRIX worldMatrix;
};


class InstanceRenderer : IDebuggable
{
public:
    struct InstancePool
    {
        WRL::ComPtr<ID3D11Buffer> vertexBuffer = nullptr;
        UINT vertexCount = 0;

        WRL::ComPtr<ID3D11Buffer> indexBuffer = nullptr;
        UINT indexCount = 0;

        std::vector<InstanceConstantBuffer> instances;
        UINT instanceCount = 0;
    };

private:
    std::unordered_map<int, InstancePool> _instancePools;
    int _batchSize = 256;
public:
    InstanceRenderer(int batchSize = 10);
    void AddInstance(const InstanceConstantBuffer& instanceData, int poolKey);
    void UpdateInstanceData(int poolKey, int instanceIndex, const InstanceConstantBuffer& newData);
    void RemoveInstance(int poolKey, int instanceIndex);
    int GetOwnershipCount() const override;


    void RemoveAllInstances();

    template <typename TVertexType>
    bool InitializeInstancePool(ID3D11Device* device, int poolKey, const std::vector<TVertexType>& vertices, const std::vector<UINT>& indices)
    {
        D3D11_BUFFER_DESC vertexBufferDesc;
        ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
        vertexBufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
        vertexBufferDesc.ByteWidth = sizeof(TVertexType) * vertices.size();
        vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vertexBufferDesc.CPUAccessFlags = 0;
        vertexBufferDesc.MiscFlags = 0;

        D3D11_BUFFER_DESC indexBufferDesc;
        ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
        indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        indexBufferDesc.ByteWidth = sizeof(UINT) * indices.size();
        indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

        // Create buffers
        InstancePool newPool;

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

        // Set pool
        newPool.vertexCount = vertices.size();
        newPool.indexCount = indices.size();
        _instancePools[poolKey] = newPool;
        return true;
    }

    template<typename TVertexType>
    void RenderInstances(ID3D11DeviceContext* deviceContext, ID3D11Buffer* perObjectConstantBuffer, ID3D11Buffer* instanceConstantBuffer)
    {
        for (const auto& instancePoolPair : _instancePools)
        {
            const InstancePool& instancePool = instancePoolPair.second;

            // if we ever needed, bind the PerObjectData here

            int instancesRendered = 0;
            while (instancesRendered < instancePool.instanceCount)
            {
                int instancesToRender = min(_batchSize, instancePool.instanceCount - instancesRendered);

                // Binding per instance data
                D3D11_MAPPED_SUBRESOURCE instanceMappedResource;
                HRESULT hrInstance =  deviceContext->Map(instanceConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &instanceMappedResource);
                if (FAILED(hrInstance))
                {
                    std::cerr << "Could not map instance data!\n";
                    return;
                }

                memcpy(instanceMappedResource.pData, instancePool.instances.data() + instancesRendered, sizeof(DirectX::XMMATRIX) * instancesToRender);
                deviceContext->Unmap(instanceConstantBuffer, 0);

                // Draw
                UINT stride = sizeof(TVertexType);
                UINT offset = 0;

                deviceContext->IASetVertexBuffers(0, 1, instancePool.vertexBuffer.GetAddressOf(), &stride, &offset);
                deviceContext->IASetIndexBuffer(instancePool.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
                deviceContext->DrawIndexedInstanced(instancePool.indexCount, instancesToRender, 0, 0, 0);

                instancesRendered += instancesToRender;
            }
        }
    }
};