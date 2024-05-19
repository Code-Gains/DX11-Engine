#pragma once
#include "Definitions.hpp"
#include "ConstantBufferDefinitions.hpp"
#include "VertexType.hpp"
#include "Logging.hpp"
#include "MeshComponent.hpp"
#include "MaterialComponent.hpp"
#include "TransformComponent.hpp"
#include "ShaderManager.hpp"
#include "ECS.hpp";

#include <vector>
#include <d3d11_2.h>
#include <unordered_map>
#include <map>
#include <iostream>
#include <array>
#include <algorithm>
#include <tuple>
#include <iostream>
#include <string>
#include <random>
#include "TextureManager.hpp"


class IVertexHandler
{
public:
    virtual ~IVertexHandler() = default;
    virtual void SetVertexBuffer(ID3D11DeviceContext* deviceContext, ID3D11Buffer* vertexBuffer) const = 0;
    virtual UINT GetStride() const = 0;
};

template <typename TVertex>
class VertexHandler : public IVertexHandler
{
public:
    void SetVertexBuffer(ID3D11DeviceContext* deviceContext, ID3D11Buffer* vertexBuffer) const override
    {
        UINT stride = sizeof(TVertex);
        UINT offset = 0;
        deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    }

    UINT GetStride() const override
    {
        return sizeof(TVertex);
    }
};

struct InstanceConstantBuffer
{
    DirectX::XMMATRIX worldMatrix;
    MaterialConstantBuffer material;

    InstanceConstantBuffer();
    InstanceConstantBuffer(const DirectX::XMMATRIX& worldMatrix);
    InstanceConstantBuffer(const DirectX::XMMATRIX& worldMatrix, const MaterialConstantBuffer& materialConstantBuffer);
    //InstanceConstantBuffer(const DirectX::XMMATRIX& worldMatrix, const MaterialConstantBuffer& materialConstantBuffer, const std::wstring& textureId);
};


class InstanceRendererSystem : public ISystem
{
public:
    struct InstancePool
    {
        std::unique_ptr<IVertexHandler> vertexHandler;
        std::wstring textureId;
        std::wstring normalMapTextureId;

        std::wstring shaderId;
        WRL::ComPtr<ID3D11Buffer> vertexBuffer = nullptr;
        UINT vertexCount = 0;

        WRL::ComPtr<ID3D11Buffer> indexBuffer = nullptr;
        UINT indexCount = 0;

        std::vector<InstanceConstantBuffer> instances;
        std::unordered_map<uint32_t, size_t> entityIdToInstanceIndex;
        UINT instanceCount = 0;

        void Clear()
        {
            instances.clear();
            entityIdToInstanceIndex.clear();
            instanceCount = 0;
        }

        InstancePool() {};

        // delete copy operators
        InstancePool(const InstancePool&) = delete;
        InstancePool& operator=(const InstancePool&) = delete;

        // move constructors
        InstancePool(InstancePool&&) noexcept = default;
        InstancePool& operator=(InstancePool&&) noexcept = default;
    };

private:
    ShaderManager* _shaderManager;
    TextureManager _textureManager;
    ECS* _ecs;

    // Instanced Rendering Resources
    std::unordered_map<int, InstancePool> _instancePools;
    int _nextPoolId = 10000; // allocate 10000 to non user meshes TODO FIX
    int _batchSize = 256;

    // Graphics Resources
    WRL::ComPtr<ID3D11Device> _device = nullptr;
    WRL::ComPtr<ID3D11DeviceContext> _deviceContext = nullptr;

    WRL::ComPtr<ID3D11ShaderResourceView> _textureView = nullptr;
    WRL::ComPtr<ID3D11SamplerState> _samplerState = nullptr;

    WRL::ComPtr<ID3D11Buffer> _perFrameConstantBuffer = nullptr;
    WRL::ComPtr<ID3D11Buffer> _cameraConstantBuffer = nullptr;
    WRL::ComPtr<ID3D11Buffer> _lightConstantBuffer = nullptr;
    WRL::ComPtr<ID3D11Buffer> _instanceConstantBuffer = nullptr;

    void CreateConstantBuffers();

public:
    InstanceRendererSystem() {}
    ~InstanceRendererSystem() {}
    InstanceRendererSystem(int batchSize = 10);
    InstanceRendererSystem(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ShaderManager* shaderManager, ECS* ecs, int batchSize = 10);

    void AddInstance(int poolKey, int instanceId, const InstanceConstantBuffer& instanceData);
    void UpdateInstanceData(int poolKey, int instanceId, const InstanceConstantBuffer& newData);
    void RemoveInstance(int poolKey, int instanceId);
    void UpdateDirtyInstances();

    void RemoveAllInstances();

    void Render() override {};
    void Update(float deltaTime) override {};
    void PeriodicUpdate(float deltaTime) override {};

    void Initialize();

    void LinkEngineInstancePools()
    {
        auto cubeMesh = MeshComponent<VertexPositionNormalUv>::GeneratePrimitiveMeshComponent(PrimitiveGeometryType3D::Cube);
        int cubeIndex = cubeMesh.GetInstancePoolIndex();
        InstancePool cubePool =
            CreateInstancePool<VertexPositionNormalUv>(cubeIndex, cubeMesh);
        cubePool.shaderId = L"Main";
        _instancePools[cubeIndex] = std::move(cubePool);

        auto skyboxMesh = MeshComponent<VertexPositionNormalUv>::GeneratePrimitiveMeshComponent(PrimitiveGeometryType3D::Skybox);
        int skyboxIndex = skyboxMesh.GetInstancePoolIndex();
        InstancePool skyboxPool =
            CreateInstancePool<VertexPositionNormalUv>(skyboxIndex, skyboxMesh);
        skyboxPool.shaderId = L"Skybox";
        auto skyboxTextureId = _textureManager.LoadTextureCubeFromSingleImage(_device.Get(), L"../../../../Assets/Textures/Skybox.png");
        // TODO fix spaghetti
        skyboxPool.textureId = skyboxTextureId;
        _instancePools[skyboxIndex] = std::move(skyboxPool);

        auto sphereMesh = MeshComponent<VertexPositionNormalUv>::GeneratePrimitiveMeshComponent(PrimitiveGeometryType3D::Sphere);
        int sphereIndex = sphereMesh.GetInstancePoolIndex();
        InstancePool spherePool =
            CreateInstancePool<VertexPositionNormalUv>(sphereIndex, sphereMesh);
        spherePool.shaderId = L"Main";
        _instancePools[sphereIndex] = std::move(spherePool);

        auto cylinderMesh = MeshComponent<VertexPositionNormalUv>::GeneratePrimitiveMeshComponent(PrimitiveGeometryType3D::Cylinder);
        int cylinderIndex = cylinderMesh.GetInstancePoolIndex();
        InstancePool cylinderPool =
            CreateInstancePool<VertexPositionNormalUv>(cylinderIndex, cylinderMesh);
        cylinderPool.shaderId = L"Main";
        _instancePools[cylinderIndex] = std::move(cylinderPool);

        auto pipeMesh = MeshComponent<VertexPositionNormalUv>::GeneratePrimitiveMeshComponent(PrimitiveGeometryType3D::Pipe);
        int pipeIndex = pipeMesh.GetInstancePoolIndex();
        InstanceRendererSystem::InstancePool pipePool =
            CreateInstancePool<VertexPositionNormalUv>(pipeIndex, pipeMesh);
        pipePool.shaderId = L"Main";
        _instancePools[pipeIndex] = std::move(pipePool);


        std::random_device rd;  // Obtain a random number from hardware
        std::mt19937 gen(rd()); // Seed the generator
        std::uniform_real_distribution<> distr(-1.0, 1.0); // Define the range

        Heightmap heightmap = Heightmap(256, 256);
        auto terrainChunkMesh = MeshComponent<VertexPositionNormalUv>::GenerateTerrainMeshComponent(PrimitiveGeometryType3D::TerrainChunk, heightmap);
        int terrainChunkIndex = terrainChunkMesh.GetInstancePoolIndex();
        InstancePool terrainChunkPool =
            CreateInstancePool<VertexPositionNormalUv>(terrainChunkIndex, terrainChunkMesh);
        terrainChunkPool.shaderId = L"Terrain";
        auto textureId = _textureManager.LoadTexture(_device.Get(), L"../../../../Assets/Textures/10x10.png");
        auto normalMapTextureId = _textureManager.CreateTextureNormalMapFromImage(_device.Get(), L"../../../../Assets/Textures/10x10.png");

        // TODO fix spaghetti
        terrainChunkPool.textureId = textureId;
        terrainChunkPool.normalMapTextureId = normalMapTextureId;
        _instancePools[terrainChunkIndex] = std::move(terrainChunkPool);
    }

    template <typename TVertex>
    InstancePool CreateInstancePool(int poolKey, const MeshComponent<TVertex>& meshComponent)
    {
        auto vertices = meshComponent.GetVertices();
        auto indices = meshComponent.GetIndices();

        D3D11_BUFFER_DESC vertexBufferDesc;
        ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
        vertexBufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
        vertexBufferDesc.ByteWidth = sizeof(TVertex) * vertices.size(); // DANGEROUS, CHANGE TODO
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
        newPool.vertexHandler = std::make_unique<VertexHandler<TVertex>>();
        newPool.vertexCount = vertices.size();
        newPool.indexCount = indices.size();
        return newPool;
    }

    void BindBuffersAndResources(
        const PerFrameConstantBuffer& perFrameConstantBuffer,
        const CameraConstantBuffer& cameraConstantBufferData,
        const LightConstantBuffer& lightConstantBufferData)
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

        _deviceContext->VSSetSamplers(0, 1, _samplerState.GetAddressOf());
        _deviceContext->PSSetSamplers(0, 1, _samplerState.GetAddressOf());

    }

    template<typename TVertexType>
    void RenderInstances(
        const PerFrameConstantBuffer& perFrameConstantBuffer,
        const CameraConstantBuffer& cameraConstantBufferData,
        const LightConstantBuffer& lightConstantBufferData
    )
    {
        BindBuffersAndResources(perFrameConstantBuffer, cameraConstantBufferData, lightConstantBufferData);

        for (const auto& instancePoolPair : _instancePools)
        {
            const InstancePool& instancePool = instancePoolPair.second;

            // Automatic shader switching
            if (instancePool.instanceCount != 0 && _shaderManager->GetCurrentShaderId() != instancePool.shaderId)
            {
                _shaderManager->ApplyToContext(instancePool.shaderId, _deviceContext.Get());
                /*auto shaderName = std::string(instancePool.shaderId.begin(), instancePool.shaderId.end());
                std::cout << shaderName << std::endl;*/
            }

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

                // Bind Textures TODO this will need adjustment to account for multiple textures ??
                // actually maybe not, it makes sense that you keep instances with different textures in
                // separate pools I think
                // While condition guarantees [0] access
                ID3D11ShaderResourceView* textureSrv = _textureManager.GetTexture(instancePool.textureId);
                ID3D11ShaderResourceView* normalMapTextureSrv = _textureManager.GetTexture(instancePool.normalMapTextureId);

                if (textureSrv)
                {
                    _deviceContext->VSSetShaderResources(0, 1, &textureSrv);
                    _deviceContext->PSSetShaderResources(0, 1, &textureSrv);
                }
                if (normalMapTextureSrv)
                {
                    _deviceContext->VSSetShaderResources(1, 1, &normalMapTextureSrv);
                    _deviceContext->PSSetShaderResources(1, 1, &normalMapTextureSrv);
                }
                
                // Automatically calculate stride and offset inside the instance pool and bind vertex buffer
                instancePool.vertexHandler->SetVertexBuffer(_deviceContext.Get(), instancePool.vertexBuffer.Get());

                // Other resource setting
                _deviceContext->IASetIndexBuffer(instancePool.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
                    
                // Draw
                _deviceContext->DrawIndexedInstanced(instancePool.indexCount, instancesToRender, 0, 0, 0);

                // For while loop condition
                instancesRendered += instancesToRender;
            }
        }
    }
};