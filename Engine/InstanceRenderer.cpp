#include "InstanceRenderer.hpp"
#include <Cube.hpp>

InstanceRenderer::InstanceRenderer()
{

}


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