#pragma once
#include <vector>
#include <cmath>
#include <d3d11_2.h>
#include "Object3D.hpp"
#include "VertexType.hpp"
#include "Constants.hpp"
#include "Definitions.hpp"

class Cylinder : public Object3D
{
private:
    WRL::ComPtr<ID3D11Buffer> _vertexBuffer = nullptr;
    WRL::ComPtr<ID3D11Buffer> _indexBuffer = nullptr;
    WRL::ComPtr<ID3D11Buffer> _instanceBuffer = nullptr;

    std::vector<VertexPositionNormalUv> _vertices;
    std::vector<UINT> _indices;

    bool _caps;

public:
    Cylinder();
    Cylinder(const bool caps = true);
    Cylinder(const DirectX::XMFLOAT3& position, const bool caps = true);
    Cylinder(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& rotation, const bool caps = true);
    Cylinder(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& rotation, const DirectX::XMFLOAT3& scale, const bool caps = true);

    virtual ~Cylinder();

    std::vector<VertexPositionNormalUv> GenerateVertices(float radius, float height, int numSlices) const;
    std::vector<UINT> GenerateIndices(int numSlices) const;

    std::vector<VertexPositionNormalUv> GetVertices() const;
    std::vector<UINT> GetIndices() const;

    bool Initialize(ID3D11Device* device) override;
    void Update(float deltaTime) override;
    void Render(ID3D11DeviceContext* deviceContext, ID3D11Buffer* perObjectConstantBuffer, ID3D11Buffer* instanceConstantBuffer) override;
    int GetOwnershipCount() const override { return 0; };
};



