#include "InstanceRendererSystem.hpp"

void InstanceRendererSystem::CreateConstantBuffers()
{
    D3D11_BUFFER_DESC desc{};
    desc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    desc.ByteWidth = sizeof(PerFrameConstantBuffer);
    _device->CreateBuffer(&desc, nullptr, &_perFrameConstantBuffer);

    desc.ByteWidth = sizeof(CameraConstantBuffer);
    _device->CreateBuffer(&desc, nullptr, &_cameraConstantBuffer);

    desc.ByteWidth = sizeof(InstanceConstantBuffer) * _batchSize;
    _device->CreateBuffer(&desc, nullptr, &_instanceConstantBuffer);
}

InstanceRendererSystem::InstanceRendererSystem(int batchSize) : _batchSize(batchSize)
{

}

InstanceRendererSystem::InstanceRendererSystem(ID3D11Device* device,
    ID3D11DeviceContext* deviceContext,
    ShaderManager* shaderManager,
    ECS* ecs,
    int batchSize) : _device(device), _deviceContext(deviceContext), _shaderManager(shaderManager), _ecs(ecs)
{
    CreateConstantBuffers();
}

void InstanceRendererSystem::AddInstance(int poolKey, int instanceId, const InstanceConstantBuffer& instanceData)
{
    auto it = _instancePools.find(poolKey);
    if (it != _instancePools.end())
    {
        it->second.entityIdToInstanceIndex[instanceId] = it->second.instances.size();
        it->second.instances.push_back(instanceData);
        it->second.instanceCount++;
    }
}

void InstanceRendererSystem::UpdateInstanceData(int poolKey, int instanceId, const InstanceConstantBuffer& newData)
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
        AddInstance(poolKey, instanceId, newData);
    }
}

void InstanceRendererSystem::RemoveInstance(int poolKey, int entityId)
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

void InstanceRendererSystem::UpdateDirtyInstances()
{
    // querry returns a vector of tuples that contain component vectors
    auto componentQueryResult = _ecs->QueryComponentVectors<TransformComponent, MeshComponent<VertexPositionNormalUv>, MaterialComponent>();
    // iterate
    for (auto& tuple : componentQueryResult)
    {
        // will need to optimize TODO (or actually never mind I think, we need entity indexes anyway)
        auto& transforms = std::get<0>(tuple);
        auto& meshes = std::get<1>(tuple);
        auto& materials = std::get<2>(tuple);

        // all vectors should be the same length (ALWAYS!!! Otherwise it should result in a crash)

        auto& rawTransforms = *transforms->GetRawVector();
        auto& rawMeshes = *meshes->GetRawVector();
        auto& rawMaterials = *materials->GetRawVector();

        for (auto& entityToComponent : transforms->GetEntityToIndex())
        {
            auto id = entityToComponent.first;
            auto idx = entityToComponent.second;
            if (rawTransforms[idx].IsDirty() || rawMaterials[idx].IsDirty())
            {
                UpdateInstanceData(
                    rawMeshes[idx].GetInstancePoolIndex(),
                    id,
                    InstanceConstantBuffer(
                        rawTransforms[idx].GetWorldMatrix(),
                        rawMaterials[idx].GetMaterialConstantBuffer()
                    ));
                rawTransforms[idx].SetIsDirty(false);
                rawMaterials[idx].SetIsDirty(false);
            }
        }
    }
}

void InstanceRendererSystem::RemoveAllInstances()
{
    for (auto& instancePoolPair : _instancePools)
    {
        InstancePool& instancePool = instancePoolPair.second;
        instancePool.entityIdToInstanceIndex.clear();
        instancePool.instances.clear();
        instancePool.instanceCount = 0;
    }
}

void InstanceRendererSystem::Initialize()
{
    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    HRESULT hr = _device->CreateSamplerState(&sampDesc, &_samplerState);
    if (FAILED(hr))
    {
        std::cerr << "Failed to create default texture sampler.\n";
    }

    // Default depth-stencil state
    D3D11_DEPTH_STENCIL_DESC defaultDesc = {};
    defaultDesc.DepthEnable = TRUE;
    defaultDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    defaultDesc.DepthFunc = D3D11_COMPARISON_LESS;
    defaultDesc.StencilEnable = FALSE;
    hr = _device->CreateDepthStencilState(&defaultDesc, _defaultDepthStencilState.GetAddressOf());
    if (FAILED(hr))
    {
        std::cerr << "Failed to create default depth-stencil state.\n";
    }

    // No depth write depth-stencil state
    D3D11_DEPTH_STENCIL_DESC noDepthWriteDesc = {};
    noDepthWriteDesc.DepthEnable = TRUE;
    noDepthWriteDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    noDepthWriteDesc.DepthFunc = D3D11_COMPARISON_LESS;
    noDepthWriteDesc.StencilEnable = FALSE;
    hr = _device->CreateDepthStencilState(&noDepthWriteDesc, _noDepthStencilState.GetAddressOf());
    if (FAILED(hr))
    {
        std::cerr << "Failed to create no-depth-write depth-stencil state.\n";
    }
}

InstanceConstantBuffer::InstanceConstantBuffer()
{
}

InstanceConstantBuffer::InstanceConstantBuffer(const DirectX::XMMATRIX& worldMatrix) : worldMatrix(worldMatrix)
{
}

InstanceConstantBuffer::InstanceConstantBuffer(
    const DirectX::XMMATRIX& worldMatrix,
    const MaterialConstantBuffer& materialConstantBuffer) : worldMatrix(worldMatrix), material(materialConstantBuffer)
{
}

//InstanceConstantBuffer::InstanceConstantBuffer(const DirectX::XMMATRIX& worldMatrix, const MaterialConstantBuffer& materialConstantBuffer, const std::wstring& textureId)
//    : worldMatrix(worldMatrix), material(materialConstantBuffer), textureId(textureId)
//{
//}
