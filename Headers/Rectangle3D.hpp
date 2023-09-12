#pragma once
#include "Object3D.hpp"
#include "Definitions.hpp"
#include <d3d11_2.h>
#include "VertexType.hpp"
#include <array>

class Rectangle3D : public Object3D
{
private:
    WRL::ComPtr<ID3D11Buffer> _vertexBuffer = nullptr;
    WRL::ComPtr<ID3D11Buffer> _indexBuffer = nullptr;
    std::vector<VertexPositionNormalUv> _vertices;
    std::vector<UINT> _indices;

public:
    Rectangle3D() {};

    Rectangle3D(const DirectX::XMFLOAT3& position);
    Rectangle3D(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& rotation);
    Rectangle3D(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& rotation, const DirectX::XMFLOAT3& scale);

    virtual ~Rectangle3D();

    bool Initialize(ID3D11Device* device) override;
    void Update(float deltaTime) override;
    void Render(ID3D11DeviceContext* deviceContext, ID3D11Buffer* perObjectConstantBuffer, ID3D11Buffer* instanceConstantBuffer) override;
    int GetOwnershipCount() const override { return 0; };

    std::vector<VertexPositionNormalUv> GetVertices();
    std::vector<UINT> GetIndices();

};

