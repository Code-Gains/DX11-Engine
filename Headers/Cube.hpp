#pragma once
#include "Definitions.hpp"
#include <d3d11_2.h>
#include "Object3D.hpp"
#include "VertexType.hpp"
#include <array>

class Cube : public Object3D
{
private:
    WRL::ComPtr<ID3D11Buffer> _vertexBuffer = nullptr;
    WRL::ComPtr<ID3D11Buffer> _indexBuffer = nullptr;
    std::vector<VertexPositionNormalUv> _vertices;
    std::vector<UINT> _indices;

public:
    Cube() {};

    Cube(const DirectX::XMFLOAT3& position);
    Cube(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& rotation);
    Cube(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& rotation, const DirectX::XMFLOAT3& scale);

    virtual ~Cube();

    bool Initialize(ID3D11Device* device) override;

    void Update(float deltaTime) override;

    void Render(ID3D11DeviceContext* deviceContext, ID3D11Buffer* perObjectConstantBuffer) override;

    std::vector<VertexPositionNormalUv> GetVertices();
    std::vector<UINT> GetIndices();
};

