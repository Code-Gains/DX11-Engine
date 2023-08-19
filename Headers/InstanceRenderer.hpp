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
    struct VertexBufferPool
    {
        WRL::ComPtr<ID3D11Buffer> vertexBuffer = nullptr;
        UINT vertexCount = 0;

        WRL::ComPtr<ID3D11Buffer> indexBuffer = nullptr;
        UINT indexCount = 0;

        DirectX::XMMATRIX modelMatrix;

        std::vector<InstanceConstantBuffer> instances;
        UINT instanceCount = 0;
    };

private:
    std::unordered_map<size_t, VertexBufferPool> _vertexBufferPools;
    int _batchSize = 256;
public:
    InstanceRenderer(int batchSize = 256);
    void AddInstance(const InstanceConstantBuffer& instanceData, size_t bufferKey);
    void UpdateInstanceData(int instanceIndex, const InstanceConstantBuffer& newData);
    void RemoveInstance(int instanceIndex);
    int GetOwnershipCount() const override;


    void RemoveAllInstances();

    template <typename TVertexType>
    bool InitializeVertexBufferPool(ID3D11Device* device, size_t bufferKey, const std::vector<TVertexType>& vertices, const std::vector<UINT>& indices, const DirectX::XMMATRIX& modelMatrix)
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

        // Set pool
        newPool.vertexCount = vertices.size();
        newPool.indexCount = indices.size();
        newPool.modelMatrix = modelMatrix;
        _vertexBufferPools[bufferKey] = newPool;
        return true;
    }

    template<typename TVertexType>
    void RenderInstances(ID3D11DeviceContext* deviceContext, ID3D11Buffer* perObjectConstantBuffer, ID3D11Buffer* instanceConstantBuffer)
    {
        for (const auto& vertexBufferPair : _vertexBufferPools)
        {
            const VertexBufferPool& vertexBufferPool = vertexBufferPair.second;
            // FLOAT4X4 storage
            DirectX::XMMATRIX modelMatrix = vertexBufferPool.modelMatrix;
            DirectX::XMMATRIX normalMatrix = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, modelMatrix));

            DirectX::XMFLOAT4X4 modelMatrixToPass;
            DirectX::XMFLOAT4X4 normalMatrixToPass;

            XMStoreFloat4x4(&modelMatrixToPass, modelMatrix);
            XMStoreFloat4x4(&normalMatrixToPass, normalMatrix);

            // Binding per object data
            D3D11_MAPPED_SUBRESOURCE perObjectMappedResource;
            HRESULT hr = deviceContext->Map(perObjectConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &perObjectMappedResource);
            if (FAILED(hr))
            {
                std::cerr << "Could not map object data!\n";
                return;
            }
            memcpy(perObjectMappedResource.pData, &modelMatrixToPass, sizeof(modelMatrixToPass));
            memcpy((char*)perObjectMappedResource.pData + sizeof(modelMatrixToPass), &normalMatrixToPass, sizeof(normalMatrixToPass));
            deviceContext->Unmap(perObjectConstantBuffer, 0);

            int instancesRendered = 0;
            while (instancesRendered < vertexBufferPool.instanceCount)
            {
                int instancesToRender = min(_batchSize, vertexBufferPool.instanceCount - instancesRendered);

                // Binding per instance data
                D3D11_MAPPED_SUBRESOURCE instanceMappedResource;
                HRESULT hrInstance =  deviceContext->Map(instanceConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &instanceMappedResource);
                if (FAILED(hrInstance))
                {
                    std::cerr << "Could not map instance data!\n";
                    return;
                }
                memcpy(instanceMappedResource.pData, vertexBufferPool.instances.data() + instancesRendered, sizeof(DirectX::XMMATRIX) * instancesToRender);
                deviceContext->Unmap(instanceConstantBuffer, 0);

                // Draw
                UINT stride = sizeof(TVertexType);
                UINT offset = 0;

                deviceContext->IASetVertexBuffers(0, 1, vertexBufferPool.vertexBuffer.GetAddressOf(), &stride, &offset);
                deviceContext->IASetIndexBuffer(vertexBufferPool.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
                deviceContext->DrawIndexedInstanced(vertexBufferPool.indexCount, instancesToRender, 0, 0, 0);

                instancesRendered += instancesToRender;
            }
        }
    }
};