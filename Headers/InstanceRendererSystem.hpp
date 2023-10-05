#pragma once
#include <vector>
#include <d3d11_2.h>
#include "Definitions.hpp"
#include "ConstantBufferDefinitions.hpp"
#include "VertexType.hpp"
#include "Logging.hpp"
#include "Cube.hpp"
#include "MeshComponent.hpp"
#include "MaterialComponent.hpp"

#include <unordered_map>
#include <map>
#include <iostream>
#include <array>
#include <algorithm>


struct InstanceConstantBuffer
{
    DirectX::XMMATRIX worldMatrix;
    MaterialConstantBuffer material;

    InstanceConstantBuffer();
    InstanceConstantBuffer(const DirectX::XMMATRIX& worldMatrix);
    InstanceConstantBuffer(const DirectX::XMMATRIX& worldMatrix, const MaterialConstantBuffer& materialConstantBuffer);
};


class InstanceRendererSystem : IDebuggable
{
public:
    struct InstancePool
    {
        WRL::ComPtr<ID3D11Buffer> vertexBuffer = nullptr;
        UINT vertexCount = 0;

        WRL::ComPtr<ID3D11Buffer> indexBuffer = nullptr;
        UINT indexCount = 0;

        std::vector<InstanceConstantBuffer> instances;
        std::unordered_map<int, size_t> entityIdToInstanceIndex;
        UINT instanceCount = 0;
    };

private:
    // Instanced Rendering Resources
    std::unordered_map<int, InstancePool> _instancePools;
    int _batchSize = 256;

    // Graphics Resources
    WRL::ComPtr<ID3D11Device> _device = nullptr;
    WRL::ComPtr<ID3D11DeviceContext> _deviceContext = nullptr;

    WRL::ComPtr<ID3D11Buffer> _perFrameConstantBuffer = nullptr;
    WRL::ComPtr<ID3D11Buffer> _cameraConstantBuffer = nullptr;
    WRL::ComPtr<ID3D11Buffer> _lightConstantBuffer = nullptr;
    WRL::ComPtr<ID3D11Buffer> _materialConstantBuffer = nullptr;
    WRL::ComPtr<ID3D11Buffer> _instanceConstantBuffer = nullptr;

    void CreateConstantBuffers();

public:
    InstanceRendererSystem(int batchSize = 10);
    InstanceRendererSystem(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int batchSize = 10);

    void AddInstance(int poolKey, int entityId, const InstanceConstantBuffer& instanceData);
    void UpdateInstanceData(int poolKey, int instanceIndex, const InstanceConstantBuffer& newData);
    void RemoveInstance(int poolKey, int instanceIndex);

    int GetOwnershipCount() const override;


    void RemoveAllInstances();

    template <typename TVertexType>
    bool InitializeInstancePool(int poolKey, const std::vector<TVertexType>& vertices, const std::vector<UINT>& indices)
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
        HRESULT hr = _device->CreateBuffer(&vertexBufferDesc, &vertexData, newPool.vertexBuffer.GetAddressOf());
        if (FAILED(hr))
        {
            std::cerr << "Could not create Vertex Buffer for a pool!\n";
            return false;
        }

        D3D11_SUBRESOURCE_DATA indexData;
        ZeroMemory(&indexData, sizeof(indexData));
        indexData.pSysMem = indices.data();
        hr = _device->CreateBuffer(&indexBufferDesc, &indexData, newPool.indexBuffer.GetAddressOf());
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

    template <typename TVertexType>
    InstancePool CreateInstancePool(int poolKey, const MeshComponent& meshComponent)
    {
        auto vertices = meshComponent.GetVertices();
        auto indices = meshComponent.GetIndices();

        D3D11_BUFFER_DESC vertexBufferDesc;
        ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
        vertexBufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
        vertexBufferDesc.ByteWidth = sizeof(TVertexType) * vertices.size(); // DANGEROUS, CHANGE TODO
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
        HRESULT hr = _device->CreateBuffer(&vertexBufferDesc, &vertexData, newPool.vertexBuffer.GetAddressOf());
        if (FAILED(hr))
        {
            std::cerr << "Could not create Vertex Buffer for a pool!\n";
            //return nullptr;
        }

        D3D11_SUBRESOURCE_DATA indexData;
        ZeroMemory(&indexData, sizeof(indexData));
        indexData.pSysMem = indices.data();
        hr = _device->CreateBuffer(&indexBufferDesc, &indexData, newPool.indexBuffer.GetAddressOf());
        if (FAILED(hr))
        {
            std::cerr << "Could not create Index Buffer for a pool!\n";
            //return nullptr;
        }

        // Set pool
        newPool.vertexCount = vertices.size();
        newPool.indexCount = indices.size();
        //_instancePools[poolKey] = newPool;
        return newPool;
    }

    template<typename TVertexType>
    void RenderInstances(const std::unordered_map<int, InstancePool>& instancePools, const PerFrameConstantBuffer& perFrameConstantBuffer, const CameraConstantBuffer& cameraConstantBufferData, const LightConstantBuffer& lightConstantBufferData, const MaterialConstantBuffer& materialConstantBufferData)
    {
        D3D11_MAPPED_SUBRESOURCE mappedResource;

        _deviceContext->Map(_perFrameConstantBuffer.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        memcpy(mappedResource.pData, &perFrameConstantBuffer, sizeof(PerFrameConstantBuffer));
        _deviceContext->Unmap(_perFrameConstantBuffer.Get(), 0);

        _deviceContext->Map(_cameraConstantBuffer.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        memcpy(mappedResource.pData, &cameraConstantBufferData, sizeof(CameraConstantBuffer));
        _deviceContext->Unmap(_cameraConstantBuffer.Get(), 0);

        _deviceContext->Map(_lightConstantBuffer.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        memcpy(mappedResource.pData, &lightConstantBufferData, sizeof(LightConstantBuffer));
        _deviceContext->Unmap(_lightConstantBuffer.Get(), 0);

        _deviceContext->Map(_materialConstantBuffer.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        memcpy(mappedResource.pData, &materialConstantBufferData, sizeof(MaterialConstantBuffer));
        _deviceContext->Unmap(_materialConstantBuffer.Get(), 0);

        ID3D11Buffer* constantPerFrameBuffers[4] = 
        {
            _perFrameConstantBuffer.Get(),
            _cameraConstantBuffer.Get(),
            _lightConstantBuffer.Get(),
            _materialConstantBuffer.Get()
        };

        ID3D11Buffer* constantPerObjectBuffers[1] =
        {
            _instanceConstantBuffer.Get()
        };

        _deviceContext->VSSetConstantBuffers(0, 4, constantPerFrameBuffers);
        _deviceContext->VSSetConstantBuffers(4, 1, constantPerObjectBuffers);

        _deviceContext->PSSetConstantBuffers(0, 4, constantPerFrameBuffers);
        _deviceContext->PSSetConstantBuffers(4, 1, constantPerObjectBuffers);

        for (const auto& instancePoolPair : instancePools)
        {
            const InstancePool& instancePool = instancePoolPair.second;

            // if we ever needed, bind the PerObjectData here

            int instancesRendered = 0;
            while (instancesRendered < instancePool.instanceCount)
            {
                int instancesToRender = min(_batchSize, instancePool.instanceCount - instancesRendered);

                // Binding per instance data
                D3D11_MAPPED_SUBRESOURCE instanceMappedResource;
                HRESULT hrInstance =  _deviceContext->Map(_instanceConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &instanceMappedResource);
                if (FAILED(hrInstance))
                {
                    std::cerr << "Could not map instance data!\n";
                    return;
                }

                memcpy(instanceMappedResource.pData, instancePool.instances.data() + instancesRendered, sizeof(InstanceConstantBuffer) * instancesToRender);
                _deviceContext->Unmap(_instanceConstantBuffer.Get(), 0);

                // Draw
                UINT stride = sizeof(TVertexType);
                UINT offset = 0;

                _deviceContext->IASetVertexBuffers(0, 1, instancePool.vertexBuffer.GetAddressOf(), &stride, &offset);
                _deviceContext->IASetIndexBuffer(instancePool.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
                _deviceContext->DrawIndexedInstanced(instancePool.indexCount, instancesToRender, 0, 0, 0);

                instancesRendered += instancesToRender;
            }
        }
    }
};