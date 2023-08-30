#include <Cube.hpp>

Cube::Cube(const DirectX::XMFLOAT3& position) : Cube(position, {0.0f, 0.0f, 0.0f}, { 1.0f, 1.0f, 1.0f })
{
}

Cube::Cube(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& rotation) : Cube(position, rotation, { 1.0f, 1.0f, 1.0f })
{
}

Cube::Cube(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& rotation, const DirectX::XMFLOAT3& scale)
{
    transform.position = position;
    transform.rotation = rotation;
    transform.scale = scale;
}

Cube::~Cube()
{
    /*if (_vertexBuffer)
    {
        _vertexBuffer->Release();
        _vertexBuffer = nullptr;
    }*/
    /*if (_indexBuffer)
    {
        _indexBuffer->Release();
        _indexBuffer = nullptr;
    }*/
}

std::vector<VertexPositionNormalUv> Cube::GetVertices()
{
    return std::vector<VertexPositionNormalUv> {
        // Front
        VertexPositionNormalUv{ Position{ -0.5f, -0.5f, 0.5f }, Normal{ 0.0f, 0.0f, 1.0f }, Uv{0.0f, 1.0f} },
        VertexPositionNormalUv{ Position{  0.5f, -0.5f, 0.5f }, Normal{ 0.0f, 0.0f, 1.0f }, Uv{1.0f, 1.0f} },
        VertexPositionNormalUv{ Position{ -0.5f,  0.5f, 0.5f }, Normal{ 0.0f, 0.0f, 1.0f }, Uv{0.0f, 0.0f} },
        VertexPositionNormalUv{ Position{  0.5f,  0.5f, 0.5f }, Normal{ 0.0f, 0.0f, 1.0f }, Uv{1.0f, 0.0f} },

        // Back
        VertexPositionNormalUv{ Position{ -0.5f, -0.5f, -0.5f }, Normal{ 0.0f, 0.0f, -1.0f }, Uv{0.0f, 1.0f} },
        VertexPositionNormalUv{ Position{  0.5f, -0.5f, -0.5f }, Normal{ 0.0f, 0.0f, -1.0f }, Uv{1.0f, 1.0f} },
        VertexPositionNormalUv{ Position{ -0.5f,  0.5f, -0.5f }, Normal{ 0.0f, 0.0f, -1.0f }, Uv{0.0f, 0.0f} },
        VertexPositionNormalUv{ Position{  0.5f,  0.5f, -0.5f }, Normal{ 0.0f, 0.0f, -1.0f }, Uv{1.0f, 0.0f} },

        // Top
        VertexPositionNormalUv{ Position{ -0.5f, 0.5f, 0.5f },  Normal{  0.0f, 1.0f, 0.0f }, Uv{0.0f, 1.0f} },
        VertexPositionNormalUv{ Position{  0.5f, 0.5f, 0.5f },  Normal{  0.0f, 1.0f, 0.0f }, Uv{1.0f, 1.0f} },
        VertexPositionNormalUv{ Position{ -0.5f, 0.5f, -0.5f }, Normal{ 0.0f, 1.0f, 0.0f },  Uv{0.0f, 0.0f} },
        VertexPositionNormalUv{ Position{  0.5f, 0.5f, -0.5f }, Normal{ 0.0f, 1.0f, 0.0f },  Uv{1.0f, 0.0f} },

        // Bottom
        VertexPositionNormalUv{ Position{ -0.5f, -0.5f, 0.5f },  Normal{  0.0f, -1.0f, 0.0f }, Uv{0.0f, 1.0f} },
        VertexPositionNormalUv{ Position{  0.5f, -0.5f, 0.5f },  Normal{  0.0f, -1.0f, 0.0f }, Uv{1.0f, 1.0f} },
        VertexPositionNormalUv{ Position{ -0.5f, -0.5f, -0.5f }, Normal{ 0.0f, -1.0f, 0.0f },  Uv{0.0f, 0.0f} },
        VertexPositionNormalUv{ Position{  0.5f, -0.5f, -0.5f }, Normal{ 0.0f, -1.0f, 0.0f },  Uv{1.0f, 0.0f} },

        // Left
        VertexPositionNormalUv{ Position{ -0.5f, -0.5f, -0.5f }, Normal{ -1.0f, 0.0f, 0.0f },  Uv{0.0f, 1.0f} },
        VertexPositionNormalUv{ Position{ -0.5f, -0.5f, 0.5f },  Normal{  -1.0f, 0.0f, 0.0f }, Uv{1.0f, 1.0f} },
        VertexPositionNormalUv{ Position{ -0.5f,  0.5f, -0.5f }, Normal{ -1.0f, 0.0f, 0.0f },  Uv{0.0f, 0.0f} },
        VertexPositionNormalUv{ Position{ -0.5f,  0.5f, 0.5f },  Normal{  -1.0f, 0.0f, 0.0f }, Uv{1.0f, 0.0f} },

        // Right
        VertexPositionNormalUv{ Position{ 0.5f, -0.5f, -0.5f }, Normal{ 1.0f, 0.0f, 0.0f },  Uv{0.0f, 1.0f} },
        VertexPositionNormalUv{ Position{ 0.5f, -0.5f, 0.5f },  Normal{  1.0f, 0.0f, 0.0f }, Uv{1.0f, 1.0f} },
        VertexPositionNormalUv{ Position{ 0.5f,  0.5f, -0.5f }, Normal{ 1.0f, 0.0f, 0.0f },  Uv{0.0f, 0.0f} },
        VertexPositionNormalUv{ Position{ 0.5f,  0.5f, 0.5f },  Normal{  1.0f, 0.0f, 0.0f }, Uv{1.0f, 0.0f} },
    };
}

std::vector<UINT> Cube::GetIndices()
{
    return {
        // Front
        0, 1, 2,
        2, 1, 3,

        // Back
        4, 5, 6,
        6, 5, 7,

        // Top
        8, 9, 10,
        10, 9, 11,

        // Bottom
        12, 13, 14,
        14, 13, 15,

        // Left
        16, 17, 18,
        18, 17, 19,

        // Right
        20, 21, 22,
        22, 21, 23,
    };
}


//static const D3D11_INPUT_ELEMENT_DESC  s_inputElementDesc[] =
//{
//        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
//        { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
//        { "SV_InstanceID",   0, DXGI_FORMAT_R32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
//};

bool Cube::Initialize(ID3D11Device* device)
{
    // Create and initialize the vertex buffer
    _vertices =  GetVertices();
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

void Cube::Update(float deltaTime)
{
 
}

void Cube::Render(ID3D11DeviceContext* deviceContext, ID3D11Buffer* perObjectConstantBuffer, ID3D11Buffer* instanceConstantBuffer)
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