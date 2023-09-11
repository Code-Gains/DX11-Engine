#pragma once
#include <Object3D.hpp>
#include <d3d11_2.h>
#include <vector>
#include <cmath>
#include "Constants.hpp"
#include "VertexType.hpp"
#include "Definitions.hpp"

class Sphere : public Object3D
{
private:
    WRL::ComPtr<ID3D11Buffer> _vertexBuffer = nullptr;
    WRL::ComPtr<ID3D11Buffer> _indexBuffer = nullptr;
    std::vector<VertexPositionNormalUv> _vertices;
    std::vector<UINT> _indices;

public:
    Sphere();

    Sphere(const DirectX::XMFLOAT3& position);
    Sphere(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& rotation);
    Sphere(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& rotation, const DirectX::XMFLOAT3& scale);

    virtual ~Sphere();


    std::vector<VertexPositionNormalUv> GenerateVertices(float radius, int numSlices, int numStacks) const;
    std::vector<UINT> GenerateIndices(int numSlices, int numStacks) const;

    std::vector<VertexPositionNormalUv> GetVertices() const;
    std::vector<UINT> GetIndices() const;

    bool Initialize(ID3D11Device* device) override;
    void Update(float deltaTime) override;
    void Render(ID3D11DeviceContext* deviceContext, ID3D11Buffer* perObjectConstantBuffer, ID3D11Buffer* instanceConstantBuffer) override;
    int GetOwnershipCount() const override { return 0; };
};

