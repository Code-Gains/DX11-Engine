#include "Rectangle3D.hpp"


Rectangle3D::Rectangle3D() : PhysicsBody(
    1.0f,
    DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f, },
    DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f, },
    DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f, },
    DirectX::XMFLOAT3{ 1.0f, 1.0f, 0.0f, })
{
}

Rectangle3D::Rectangle3D(const DirectX::XMFLOAT3& position) : Rectangle3D(position, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f })
{
}

Rectangle3D::Rectangle3D(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& rotation) : Rectangle3D(position, rotation, { 1.0f, 1.0f, 1.0f })
{
}

Rectangle3D::Rectangle3D(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& rotation, const DirectX::XMFLOAT3& scale) : PhysicsBody(
    1.0f,
    DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f, },
    position,
    rotation,
    scale)
{
    transform.position = position;
    transform.rotation = rotation;
    transform.scale = scale;
}

Rectangle3D::~Rectangle3D()
{
}

std::vector<VertexPositionNormalUv> Rectangle3D::GetVertices()
{
    return std::vector<VertexPositionNormalUv> {
        // Front
        VertexPositionNormalUv{ Position{ -0.5f, -0.5f, 0.0f }, Normal{ 0.0f, 0.0f, 1.0f }, Uv{0.0f, 1.0f} },
        VertexPositionNormalUv{ Position{  0.5f, -0.5f, 0.0f }, Normal{ 0.0f, 0.0f, 1.0f }, Uv{1.0f, 1.0f} },
        VertexPositionNormalUv{ Position{ -0.5f,  0.5f, 0.0f }, Normal{ 0.0f, 0.0f, 1.0f }, Uv{0.0f, 0.0f} },
        VertexPositionNormalUv{ Position{  0.5f,  0.5f, 0.0f }, Normal{ 0.0f, 0.0f, 1.0f }, Uv{1.0f, 0.0f} },

        // Back
        VertexPositionNormalUv{ Position{ -0.5f, -0.5f, 0.0f }, Normal{ 0.0f, 0.0f, -1.0f }, Uv{0.0f, 1.0f} },
        VertexPositionNormalUv{ Position{  0.5f, -0.5f, 0.0f }, Normal{ 0.0f, 0.0f, -1.0f }, Uv{1.0f, 1.0f} },
        VertexPositionNormalUv{ Position{ -0.5f,  0.5f, 0.0f }, Normal{ 0.0f, 0.0f, -1.0f }, Uv{0.0f, 0.0f} },
        VertexPositionNormalUv{ Position{  0.5f,  0.5f, 0.0f }, Normal{ 0.0f, 0.0f, -1.0f }, Uv{1.0f, 0.0f} },

    };
}

std::vector<UINT> Rectangle3D::GetIndices()
{
    return {
        // Front
        0, 1, 2,
        2, 1, 3,

        // Back
        4, 5, 6,
        6, 5, 7,
    };
}

bool Rectangle3D::Initialize(ID3D11Device* device)
{
    // Create and initialize the vertex buffer
    _vertices = GetVertices();
    D3D11_BUFFER_DESC vertexBufferDesc;
    ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
    vertexBufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexPositionNormalUv) * _vertices.size();
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;  // Don't set D3D11_CPU_ACCESS_WRITE for a D3D11_USAGE_DEFAULT resource
    vertexBufferDesc.MiscFlags = 0;  // We don't need any flags for now https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ne-d3d11-d3d11_resource_misc_flag


    D3D11_SUBRESOURCE_DATA vertexData;
    vertexData.pSysMem = _vertices.data();
    HRESULT hr = device->CreateBuffer(&vertexBufferDesc, &vertexData, _vertexBuffer.GetAddressOf());
    if (FAILED(hr)) return false;

    // Create and initialize the index buffer
    _indices = GetIndices();
    D3D11_BUFFER_DESC indexBufferDesc;
    ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(uint32_t) * _indices.size();
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA indexData;
    indexData.pSysMem = _indices.data();
    hr = device->CreateBuffer(&indexBufferDesc, &indexData, _indexBuffer.GetAddressOf());
    if (FAILED(hr)) return false;

    return true;

}

void Rectangle3D::Update(float deltaTime)
{
    
}

void Rectangle3D::Render(ID3D11DeviceContext* deviceContext, ID3D11Buffer* perObjectConstantBuffer, ID3D11Buffer* instanceConstantBuffer)
{

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    DirectX::XMMATRIX modelMatrix = transform.GetWorldMatrix();
    DirectX::XMMATRIX normalMatrix = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, modelMatrix));

    DirectX::XMFLOAT4X4 modelMatrixToPass;
    DirectX::XMFLOAT4X4 normalMatrixToPass;

    XMStoreFloat4x4(&modelMatrixToPass, modelMatrix);
    XMStoreFloat4x4(&normalMatrixToPass, modelMatrix);


    deviceContext->Map(perObjectConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    memcpy(mappedResource.pData, &modelMatrixToPass, sizeof(modelMatrixToPass));
    memcpy((char*)mappedResource.pData + sizeof(modelMatrixToPass), &normalMatrixToPass, sizeof(normalMatrixToPass));
    deviceContext->Unmap(perObjectConstantBuffer, 0);

    // Set the vertex and index buffers, and draw the cube
    UINT stride = sizeof(VertexPositionNormalUv);
    UINT offset = 0;
    deviceContext->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);
    deviceContext->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    deviceContext->DrawIndexed(_indices.size(), 0, 0);
}