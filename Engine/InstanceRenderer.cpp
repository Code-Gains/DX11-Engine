#include "InstanceRenderer.hpp"
#include <Cube.hpp>

InstanceRenderer::InstanceRenderer()
{

}



//template <typename VertexType, size_t VertexCount, size_t IndexCount>
//bool InstanceRenderer::InitializeVertexBufferPool(ID3D11Device* device, size_t bufferKey, const std::array<VertexType, VertexCount>& vertices, std::array<UINT, IndexCount>& indices)
//{
//    // Init Buffer Descriptions
//    D3D11_BUFFER_DESC vertexBufferDesc;
//    ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
//    vertexBufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
//    vertexBufferDesc.ByteWidth = sizeof(VertexPositionNormalUv) * vertices.size();
//    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//    vertexBufferDesc.CPUAccessFlags = 0;
//    vertexBufferDesc.MiscFlags = 0;
//
//    D3D11_BUFFER_DESC indexBufferDesc;
//    ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
//    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
//    indexBufferDesc.ByteWidth = sizeof(uint32_t) * indices.size();
//    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
//
//    // Create Buffers
//    VertexBufferPool newPool;
//
//    D3D11_SUBRESOURCE_DATA vertexData;
//    ZeroMemory(&vertexData, sizeof(vertexData));
//    vertexData.pSysMem = vertices.data();
//    HRESULT hr = device->CreateBuffer(&vertexBufferDesc, &vertexData, newPool.vertexBuffer.GetAddressOf());
//    if (FAILED(hr))
//    {
//        std::cerr << "Could not create Vertex Buffer for a pool!\n";
//        return false;
//    }
//
//    D3D11_SUBRESOURCE_DATA indexData;
//    ZeroMemory(&indexData, sizeof(indexData));
//    indexData.pSysMem = indices.data();
//    hr = device->CreateBuffer(&indexBufferDesc, &indexData, newPool.indexBuffer.GetAddressOf());
//    if (FAILED(hr))
//    {
//        std::cerr << "Could not create Index Buffer for a pool!\n";
//        return false;
//    }
//
//    // Set Pool
//    _vertexBufferPools[bufferKey] = newPool;
//    return true;
//}

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

void InstanceRenderer::RenderInstances(ID3D11DeviceContext* deviceContext, ID3D11Buffer* perObjectConstantBuffer, ID3D11Buffer* instanceConstantBuffer)
{
    for (const auto& vertexBufferPair : _vertexBufferPools)
    {
        const VertexBufferPool& vertexBufferPool = vertexBufferPair.second;
        //std::cout << vertexBufferPool.vertexCount << std::endl;
        //for (const auto& instance : vertexBufferPool.instances)
        //{
        //    // update cbuffers with instance data
        //}
        auto cube = std::make_unique<Cube>(DirectX::XMFLOAT3{ 0, 0, 0 });
        D3D11_MAPPED_SUBRESOURCE perObjectMappedResource;
        DirectX::XMMATRIX modelMatrix = cube->transform.GetWorldMatrix();
        DirectX::XMMATRIX normalMatrix = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, modelMatrix));

        DirectX::XMFLOAT4X4 modelMatrixToPass;
        DirectX::XMFLOAT4X4 normalMatrixToPass;

        XMStoreFloat4x4(&modelMatrixToPass, modelMatrix);
        XMStoreFloat4x4(&normalMatrixToPass, modelMatrix);

        deviceContext->Map(perObjectConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &perObjectMappedResource);
        memcpy(perObjectMappedResource.pData, &modelMatrixToPass, sizeof(modelMatrixToPass));
        memcpy((char*)perObjectMappedResource.pData + sizeof(modelMatrixToPass), &normalMatrixToPass, sizeof(normalMatrixToPass));
        deviceContext->Unmap(perObjectConstantBuffer, 0);

        size_t batchSize = 1024; // hardcode to test
        size_t instancesRendered = 0;
        while(instancesRendered < vertexBufferPool.instanceCount)
        {
            size_t instancesToRender = min(batchSize, vertexBufferPool.instanceCount - instancesRendered);

            D3D11_MAPPED_SUBRESOURCE instanceMappedResource;

            deviceContext->Map(instanceConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &instanceMappedResource);
            memcpy(instanceMappedResource.pData, vertexBufferPool.instances.data() + instancesRendered, sizeof(DirectX::XMMATRIX) * instancesToRender);
            deviceContext->Unmap(instanceConstantBuffer, 0);

            UINT stride = sizeof(VertexPositionNormalUv);
            UINT offset = 0;

            deviceContext->IASetVertexBuffers(0, 1, vertexBufferPool.vertexBuffer.GetAddressOf(), &stride, &offset);
            deviceContext->IASetIndexBuffer(vertexBufferPool.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
            deviceContext->DrawIndexedInstanced(vertexBufferPool.indexCount, instancesToRender, 0, 0, 0);
            instancesRendered += instancesToRender;
        }
    }
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