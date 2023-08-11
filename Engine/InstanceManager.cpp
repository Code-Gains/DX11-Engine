#include "InstanceManager.hpp"

InstanceManager::InstanceManager()
{

}

void InstanceManager::InitializeVertexBufferPool(size_t bufferKey, const std::vector<VertexPositionNormalUv>& vertices, const std::vector<UINT>& indices)
{
    D3D11_BUFFER_DESC vertexBufferDesc;
    ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc)); // not sure if this is needed, I just created it
    vertexBufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexPositionNormalUv) * ARRAYSIZE(vertices);
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;

    VertexBufferPool newPool;

    HRESULT hr = device->CreateBuffer(&vertexBufferDesc, &vertexData, _vertexBuffer.GetAddressOf());
    if (FAILED(hr)) return false;

    _vertexBufferPools[bufferKey] = newPool;
}

void InstanceManager::AddInstance(const InstanceConstantBuffer& instanceData, size_t bufferKey)
{
    auto it = _vertexBufferPools.find(bufferKey);
    if (it != _vertexBufferPools.end())
    {
        it->second.instances.push_back(instanceData);
    }
    // I assume do nothing since all keys are created before running the actual loops?
}

void InstanceManager::UpdateInstanceData(int instanceIndex, const InstanceConstantBuffer& newData)
{

}

void InstanceManager::RemoveInstance(int instanceIndex)
{

}

void InstanceManager::RenderInstances(ID3D11DeviceContext* deviceContext)
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