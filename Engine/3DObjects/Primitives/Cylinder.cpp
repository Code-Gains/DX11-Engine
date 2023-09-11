#include "Cylinder.hpp"

Cylinder::Cylinder(): _caps(true)
{
    _vertices = GenerateVertices(0.5f, 1, 30); // parameterise this
    _indices = GenerateIndices(30);
}

Cylinder::Cylinder(const bool caps): _caps(caps)
{
    _vertices = GenerateVertices(0.5f, 1, 30); // parameterise this
    _indices = GenerateIndices(30);
}

Cylinder::Cylinder(const DirectX::XMFLOAT3& position, const bool caps): Cylinder(position, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, caps)
{
    _vertices = GenerateVertices(0.5f, 1, 30); // parameterise this
    _indices = GenerateIndices(30);
}


Cylinder::Cylinder(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& rotation, const bool caps): Cylinder(position, rotation, { 1.0f, 1.0f, 1.0f }, caps)
{
    _vertices = GenerateVertices(0.5f, 1, 30); // parameterise this
    _indices = GenerateIndices(30);
}


Cylinder::Cylinder(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& rotation, const DirectX::XMFLOAT3& scale, const bool caps): _caps(caps)
{
    transform.position = position;
    transform.rotation = rotation;
    transform.scale = scale;
    _vertices = GenerateVertices(0.5f, 1, 30); // parameterise this
    _indices = GenerateIndices(30);
}
Cylinder::~Cylinder()
{
}

std::vector<VertexPositionNormalUv> Cylinder::GenerateVertices(float radius, float height, int numSlices) const
{
    std::vector<VertexPositionNormalUv> vertices;

    // Generate cap vertices and side vertices
    for (int i = 0; i <= numSlices; ++i) {
        float phi = i * 2.0f * M_PI / numSlices;
        float sinPhi = sin(phi);
        float cosPhi = cos(phi);

        if (_caps)
        {
            vertices.push_back({
                {radius * cosPhi, height / 2, radius * sinPhi},  // Position
                {0, 1, 0},                                       // Normal
                {cosPhi, sinPhi}                                  // Uv
            });

            // Bottom cap
            vertices.push_back({
                {radius * cosPhi, -height / 2, radius * sinPhi}, // Position
                {0, -1, 0},                                      // Normal
                {cosPhi, sinPhi}                                  // Uv
            });
        }

        // Side top
        vertices.push_back({
            {radius * cosPhi, height / 2, radius * sinPhi},  // Position
            {cosPhi, 0, sinPhi},                             // Normal
            {(float)i / numSlices, 1.0f}                      // Uv
        });

        // Side bottom
        vertices.push_back({
            {radius * cosPhi, -height / 2, radius * sinPhi}, // Position
            {cosPhi, 0, sinPhi},                             // Normal
            {(float)i / numSlices, 0.0f}                      // Uv
        });
    }
    return vertices;
}

std::vector<UINT> Cylinder::GenerateIndices(int numSlices) const
{
    std::vector<UINT> indices;

    int vertexOffset = (_caps) ? 4 : 2;  // offset 4 if caps are enabled, else 2

    for (int i = 0; i < numSlices; ++i) {
        // Side
        indices.push_back(i * vertexOffset);
        indices.push_back(((i + 1) % numSlices) * vertexOffset);
        indices.push_back(i * vertexOffset + 1);

        indices.push_back(i * vertexOffset + 1);
        indices.push_back(((i + 1) % numSlices) * vertexOffset);
        indices.push_back(((i + 1) % numSlices) * vertexOffset + 1);

        if (_caps)
        {
            // Top cap
            indices.push_back(i * 4);
            indices.push_back(((i + 1) % numSlices) * 4);
            indices.push_back(numSlices * 4);  // Center vertex

            // Bottom cap
            indices.push_back(i * 4 + 1);
            indices.push_back(numSlices * 4 + 1);  // Center vertex
            indices.push_back(((i + 1) % numSlices) * 4 + 1);
        }
    }
    return indices;
}

std::vector<VertexPositionNormalUv> Cylinder::GetVertices() const
{
    if (_vertices.size() < 1)
        std::cout << "Calling before Initialize()" << std::endl;
    return _vertices;
}

std::vector<UINT> Cylinder::GetIndices() const
{
    return _indices;
}

bool Cylinder::Initialize(ID3D11Device* device)
{
    D3D11_BUFFER_DESC vertexBufferDesc;
    ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
    vertexBufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexPositionNormalUv) * _vertices.size();
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vertexData;
    vertexData.pSysMem = _vertices.data();
    HRESULT hr = device->CreateBuffer(&vertexBufferDesc, &vertexData, _vertexBuffer.GetAddressOf());
    if (FAILED(hr)) return false;

    D3D11_BUFFER_DESC indexBufferDesc;
    ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(UINT) * _indices.size();
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA indexData;
    indexData.pSysMem = _indices.data();
    hr = device->CreateBuffer(&indexBufferDesc, &indexData, _indexBuffer.GetAddressOf());
    if (FAILED(hr)) return false;

    return true;
}

void Cylinder::Update(float deltaTime)
{

}

void Cylinder::Render(ID3D11DeviceContext* deviceContext, ID3D11Buffer* perObjectConstantBuffer, ID3D11Buffer* instanceConstantBuffer)
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    DirectX::XMMATRIX modelMatrix = transform.GetWorldMatrix();
    DirectX::XMMATRIX normalMatrix = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, modelMatrix));

    DirectX::XMFLOAT4X4 modelMatrixToPass;
    DirectX::XMFLOAT4X4 normalMatrixToPass;

    XMStoreFloat4x4(&modelMatrixToPass, modelMatrix);
    XMStoreFloat4x4(&normalMatrixToPass, normalMatrix);

    deviceContext->Map(perObjectConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    memcpy(mappedResource.pData, &modelMatrixToPass, sizeof(modelMatrixToPass));
    memcpy((char*)mappedResource.pData + sizeof(modelMatrixToPass), &normalMatrixToPass, sizeof(normalMatrixToPass));
    deviceContext->Unmap(perObjectConstantBuffer, 0);

    UINT stride = sizeof(VertexPositionNormalUv);
    UINT offset = 0;
    deviceContext->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);
    deviceContext->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

    int numSlices = 30;

    int numIndices = numSlices * 6; // for the side indices

    if (_caps) {
        numIndices += numSlices * 6; // add indices for top and bottom caps
    }

    deviceContext->DrawIndexed(numIndices, 0, 0);
}

