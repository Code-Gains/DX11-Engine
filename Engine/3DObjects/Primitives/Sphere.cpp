#include <Sphere.hpp>

Sphere::Sphere()
{
    _vertexBuffer = nullptr;
    _indexBuffer = nullptr;
}

Sphere::Sphere(DirectX::XMFLOAT3 position)
{
    _vertexBuffer = nullptr;
    _indexBuffer = nullptr;
    transform.position = position;
}

Sphere::~Sphere()
{
    if (_vertexBuffer)
    {
        _vertexBuffer->Release();
        _vertexBuffer = nullptr;
    }
    if (_indexBuffer)
    {
        _indexBuffer->Release();
        _indexBuffer = nullptr;
    }
}

void Sphere::GenerateSphereVertices(float radius, int numSlices, int numStacks, std::vector<VertexPositionNormalUv>& vertices)
{
    vertices.clear();

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
}

void Sphere::GenerateSphereIndices(int numSlices, int numStacks, std::vector<uint32_t>&indices)
{
    indices.clear();

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
}

bool Sphere::Initialize(ID3D11Device* device)
{

    std::vector<VertexPositionNormalUv> vertices;
    GenerateSphereVertices(0.5f, 30, 30, vertices);

    std::vector<uint32_t> indices;
    GenerateSphereIndices(30, 30, indices);

    D3D11_BUFFER_DESC vertexBufferDesc;
    ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
    vertexBufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexPositionNormalUv) * vertices.size();
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vertexData;
    vertexData.pSysMem = vertices.data();
    HRESULT hr = device->CreateBuffer(&vertexBufferDesc, &vertexData, &_vertexBuffer);
    if (FAILED(hr)) return false;

    D3D11_BUFFER_DESC indexBufferDesc;
    ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(uint32_t) * indices.size();
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA indexData;
    indexData.pSysMem = indices.data();
    hr = device->CreateBuffer(&indexBufferDesc, &indexData, &_indexBuffer);
    if (FAILED(hr)) return false;

    return true;
}

void Sphere::Update(const float deltaTime)
{

}

void Sphere::Render(ID3D11DeviceContext* deviceContext, ID3D11Buffer* perObjectConstantBuffer)
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
    deviceContext->IASetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);
    deviceContext->IASetIndexBuffer(_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    int numSlices = 30;
    int numStacks = 30;
    int numIndices = numSlices * numStacks * 6;
    deviceContext->DrawIndexed(numIndices, 0, 0);
}
