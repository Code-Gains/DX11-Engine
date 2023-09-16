#include <Sphere.hpp>

Sphere::Sphere()
{
    _vertices = GenerateVertices(0.5f, 30, 30);
    _indices = GenerateIndices(30, 30);
}

Sphere::Sphere(const DirectX::XMFLOAT3& position) : Sphere(position, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f })
{
}

Sphere::Sphere(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& rotation): Sphere(position, rotation, DirectX::XMFLOAT3{ 1.0f, 1.0f, 1.0f })
{
}

Sphere::Sphere(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& rotation, const DirectX::XMFLOAT3& scale)
{
    transform.position = position;
    transform.rotation = rotation;
    transform.scale = scale;
    _vertices = GenerateVertices(0.5f, 30, 30);
    _indices = GenerateIndices(30, 30);
}

Sphere::~Sphere()
{
}

std::vector<VertexPositionNormalUv> Sphere::GenerateVertices(float radius, int numSlices, int numStacks) const
{
    std::vector<VertexPositionNormalUv> vertices;

    // Iterate over latitude and longitude
    for (int i = 0; i <= numStacks; ++i) {
        float theta = i * M_PI / numStacks;      // Range [0, PI]
        float sinTheta = sin(theta);
        float cosTheta = cos(theta);

        for (int j = 0; j <= numSlices; ++j) {
            float phi = j * 2.0f * M_PI / numSlices;   // Range [0, 2PI]
            float sinPhi = sin(phi);
            float cosPhi = cos(phi);

            // Convert polar coordinates to Cartesian (x, y, z)
            float x = cosPhi * sinTheta;
            float y = cosTheta;
            float z = sinPhi * sinTheta;

            // Compute the texture coordinates
            float u = 1.0f - (float)j / numSlices;
            float v = 1.0f - (float)i / numStacks;

            vertices.push_back({
                {x * radius, y * radius, z * radius}, // Position
                {x, y, z},                            // Normal
                {u, v}                                // Uv
            });
        }
    }
    return vertices;
}

std::vector<UINT> Sphere::GenerateIndices(int numSlices, int numStacks) const
{
    std::vector<UINT> indices;

    for (int i = 0; i < numStacks; ++i) {
        for (int j = 0; j < numSlices; ++j) {
            int first = (i * (numSlices + 1)) + j;
            int second = first + numSlices + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }

    return indices;
}

std::vector<VertexPositionNormalUv> Sphere::GetVertices() const
{
    return _vertices;
}

std::vector<UINT> Sphere::GetIndices() const
{
    return _indices;
}

bool Sphere::Initialize(ID3D11Device* device)
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

void Sphere::Update(float deltaTime)
{

}

void Sphere::Render(ID3D11DeviceContext* deviceContext, ID3D11Buffer* perObjectConstantBuffer, ID3D11Buffer* instanceConstantBuffer)
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
    int numStacks = 30;
    int numIndices = numSlices * numStacks * 6;
    deviceContext->DrawIndexed(numIndices, 0, 0);
}
