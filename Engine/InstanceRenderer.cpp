#include "InstanceRenderer.hpp"

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
    }
}

void InstanceRenderer::UpdateInstanceData(int instanceIndex, const InstanceConstantBuffer& newData)
{

}

void InstanceRenderer::RemoveInstance(int instanceIndex)
{

}

void InstanceRenderer::RenderInstances(ID3D11DeviceContext* deviceContext)
{
    for (const auto& vertexBufferPair : _vertexBufferPools)
    {
        const VertexBufferPool& vertexBufferPool = vertexBufferPair.second;
        for (const auto& instance : vertexBufferPool.instances)
        {
            // update cbuffers with instance data
        }
        // transfer cbuffer to GPU
        // draw instanced?
        deviceContext->DrawIndexedInstanced(vertexBufferPool.indexCount, vertexBufferPool.instanceCount, 0, 0, 0);
        // clean up
    }



    //D3D11_MAPPED_SUBRESOURCE mappedResource;
    //DirectX::XMMATRIX modelMatrix = transform.GetWorldMatrix();
    //DirectX::XMMATRIX normalMatrix = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, modelMatrix));

    //DirectX::XMFLOAT4X4 modelMatrixToPass;
    //DirectX::XMFLOAT4X4 normalMatrixToPass;

    //XMStoreFloat4x4(&modelMatrixToPass, modelMatrix);
    //XMStoreFloat4x4(&normalMatrixToPass, modelMatrix);


    //deviceContext->Map(perObjectConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    //memcpy(mappedResource.pData, &modelMatrixToPass, sizeof(modelMatrixToPass));
    //memcpy((char*)mappedResource.pData + sizeof(modelMatrixToPass), &normalMatrixToPass, sizeof(normalMatrixToPass));
    //deviceContext->Unmap(perObjectConstantBuffer, 0);

    //// Set the vertex and index buffers, and draw the cube
    //UINT stride = sizeof(VertexPositionNormalUv);
    //UINT offset = 0;
    //deviceContext->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);
    //deviceContext->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    //deviceContext->DrawIndexed(ARRAYSIZE(indices), 0, 0);
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