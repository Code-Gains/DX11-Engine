#include "RenderingSystem.hpp"

RenderingSystem::RenderingSystem()
{
}

bool RenderingSystem::Initialize(ID3D11Device* device)
{
    // Create and initialize the vertex buffer
    //_vertices = GetVertices();
    D3D11_BUFFER_DESC vertexBufferDesc;
    ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
    vertexBufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexPositionNormalUv) * 24;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;  // Don't set D3D11_CPU_ACCESS_WRITE for a D3D11_USAGE_DEFAULT resource
    vertexBufferDesc.MiscFlags = 0;  // We don't need any flags for now https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ne-d3d11-d3d11_resource_misc_flag


    D3D11_SUBRESOURCE_DATA vertexData;
    vertexData.pSysMem = _vertices.data();
    HRESULT hr = device->CreateBuffer(&vertexBufferDesc, &vertexData, _vertexBuffer.GetAddressOf());
    if (FAILED(hr)) return false;

    // Create and initialize the index buffer
    //_indices = GetIndices();
    D3D11_BUFFER_DESC indexBufferDesc;
    ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(uint32_t) * 36;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA indexData;
    indexData.pSysMem = _indices.data();
    hr = device->CreateBuffer(&indexBufferDesc, &indexData, _indexBuffer.GetAddressOf());
    if (FAILED(hr)) return false;

    return true;
}

void RenderingSystem::Update(float deltaTime)
{
}


/*
* Gonna make an inneficient one to see how bad it is
*/
void RenderingSystem::Render(
	ID3D11DeviceContext* deviceContext,
	ID3D11Buffer* perObjectConstantBuffer,
	ID3D11Buffer* instanceConstantBuffer,
	const std::vector<TransformComponent>& _transformComponents,
	const std::vector<MeshComponent>& _meshComponents,
	const std::vector<MaterialComponent>& _materialComponents,
	const std::vector<LightComponent>& _lightComponents
)
{
	// receive transform, mesh, material data with some sort of group assigning
	// with groups bind resources and render
	// maybe actually do a submit method where we submit data into the rendering system?
}