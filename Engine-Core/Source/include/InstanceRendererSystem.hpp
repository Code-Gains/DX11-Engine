#pragma once
#include "Definitions.hpp"
#include "ConstantBufferDefinitions.hpp"
#include "VertexType.hpp"
#include "Logging.hpp"
#include "MeshComponent.hpp"
#include "MaterialComponent.hpp"
#include "TransformComponent.hpp"
#include "ECS.hpp";

#include <vector>
#include <d3d11_2.h>
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


class InstanceRendererSystem : public ISystem
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

        void Clear()
        {
            instances.clear();
            entityIdToInstanceIndex.clear();
            instanceCount = 0;
        }
    };

private:
    // ECS
    ECS* _ecs;

    // Instanced Rendering Resources
    std::unordered_map<int, InstancePool> _instancePools;
    int _nextPoolId = 10000; // allocate 10000 to non user meshes TODO FIX
    int _batchSize = 256;

    // Graphics Resources
    WRL::ComPtr<ID3D11Device> _device = nullptr;
    WRL::ComPtr<ID3D11DeviceContext> _deviceContext = nullptr;

    WRL::ComPtr<ID3D11Buffer> _perFrameConstantBuffer = nullptr;
    WRL::ComPtr<ID3D11Buffer> _cameraConstantBuffer = nullptr;
    WRL::ComPtr<ID3D11Buffer> _lightConstantBuffer = nullptr;
    WRL::ComPtr<ID3D11Buffer> _instanceConstantBuffer = nullptr;

    void CreateConstantBuffers();

public:
    InstanceRendererSystem() {}
    ~InstanceRendererSystem() {}
    InstanceRendererSystem(int batchSize = 10);
    InstanceRendererSystem(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ECS* ecs, int batchSize = 10);

    void AddInstance(int poolKey, int instanceId, const InstanceConstantBuffer& instanceData);
    void UpdateInstanceData(int poolKey, int instanceId, const InstanceConstantBuffer& newData);
    void RemoveInstance(int poolKey, int instanceId);
    void UpdateDirtyInstances();

    void RemoveAllInstances();

    void Render() override {};
    void Update(float deltaTime) override {};
    void PeriodicUpdate(float deltaTime) override {};

    void Initialize();


    // TODO fix to remove any references to entities
   /* void RenderingApplication3D::UpdateRenderableInstanceData(int poolKey, int instanceId, const InstanceConstantBuffer& newData)
    {
        if (_instancePools.find(poolKey) != _instancePools.end())
        {
            auto& entityIdToInstances = _instancePools[poolKey].entityIdToInstanceIndex;
            if (entityIdToInstances.find(instanceId) != entityIdToInstances.end())
            {
                auto instanceIndex = entityIdToInstances[instanceId];
                _instancePools[poolKey].instances[instanceIndex] = newData;
                return;
            }
            AddRenderableInstance(poolKey, instanceId, newData);
        }
    }*/

    /*void RenderingApplication3D::RemoveRenderableInstance(int poolKey, int entityId)
    {
        if (_instancePools.find(poolKey) != _instancePools.end())
        {
            auto& entityIdToInstance = _instancePools[poolKey].entityIdToInstanceIndex;
            auto& instances = _instancePools[poolKey].instances;
            auto& instanceCount = _instancePools[poolKey].instanceCount;
            if (entityIdToInstance.find(entityId) != entityIdToInstance.end())
            {
                auto instanceIndex = entityIdToInstance[entityId];
                for (auto& pair : entityIdToInstance)
                {
                    if (pair.second > instanceIndex)
                    {
                        pair.second--;
                    }
                }
                entityIdToInstance.erase(entityId);
                instances.erase(instances.begin() + instanceIndex);
                _instancePools[poolKey].instanceCount--;
            }
        }
    }

    void RenderingApplication3D::RemoveAllRenderableInstances()
    {
        for (auto& instancePoolPair : _instancePools)
        {
            InstanceRendererSystem::InstancePool& instancePool = instancePoolPair.second;
            instancePool.entityIdToInstanceIndex.clear();
            instancePool.instances.clear();
            instancePool.instanceCount = 0;
        }
    }

    void RenderingApplication3D::ClearAllInstancePools()
    {
        for (auto& instancePool : _instancePools)
        {
            instancePool.second.Clear();
        }
    }*/


    void LinkEngineInstancePools()
    {
        auto cubeMesh = MeshComponent::GeneratePrimitiveMeshComponent(PrimitiveGeometryType3D::Cube);
        int cubeIndex = cubeMesh.GetInstancePoolIndex();
        InstancePool cubePool =
            CreateInstancePool<VertexPositionNormalUv>(cubeIndex, cubeMesh);
        _instancePools[cubeIndex] = cubePool;

        auto sphereMesh = MeshComponent::GeneratePrimitiveMeshComponent(PrimitiveGeometryType3D::Sphere);
        int sphereIndex = sphereMesh.GetInstancePoolIndex();
        InstancePool spherePool =
            CreateInstancePool<VertexPositionNormalUv>(sphereIndex, sphereMesh);
        _instancePools[sphereIndex] = spherePool;

        auto cylinderMesh = MeshComponent::GeneratePrimitiveMeshComponent(PrimitiveGeometryType3D::Cylinder);
        int cylinderIndex = cylinderMesh.GetInstancePoolIndex();
        InstancePool cylinderPool =
            CreateInstancePool<VertexPositionNormalUv>(cylinderIndex, cylinderMesh);
        _instancePools[cylinderIndex] = cylinderPool;

        auto pipeMesh = MeshComponent::GeneratePrimitiveMeshComponent(PrimitiveGeometryType3D::Pipe);
        int pipeIndex = pipeMesh.GetInstancePoolIndex();
        InstanceRendererSystem::InstancePool pipePool =
            CreateInstancePool<VertexPositionNormalUv>(pipeIndex, pipeMesh);
        _instancePools[pipeIndex] = pipePool;

        auto terrainChunkMesh = MeshComponent::GenerateTerrainMeshComponent(PrimitiveGeometryType3D::TerrainChunk);
        int terrainChunkIndex = terrainChunkMesh.GetInstancePoolIndex();
        InstancePool terrainChunkPool =
            CreateInstancePool<VertexPositionNormalUv>(terrainChunkIndex, terrainChunkMesh);
        _instancePools[terrainChunkIndex] = terrainChunkPool;
    }

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
    void RenderInstances(
        const PerFrameConstantBuffer& perFrameConstantBuffer,
        const CameraConstantBuffer& cameraConstantBufferData,
        const LightConstantBuffer& lightConstantBufferData
    )
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

        ID3D11Buffer* constantPerFrameBuffers[3] = 
        {
            _perFrameConstantBuffer.Get(),
            _cameraConstantBuffer.Get(),
            _lightConstantBuffer.Get(),
        };

        ID3D11Buffer* constantPerObjectBuffers[1] =
        {
            _instanceConstantBuffer.Get()
        };

        _deviceContext->VSSetConstantBuffers(0, 3, constantPerFrameBuffers);
        _deviceContext->VSSetConstantBuffers(3, 1, constantPerObjectBuffers);

        _deviceContext->PSSetConstantBuffers(0, 3, constantPerFrameBuffers);
        _deviceContext->PSSetConstantBuffers(3, 1, constantPerObjectBuffers);

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