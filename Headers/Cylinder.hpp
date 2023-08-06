#pragma once
#include <vector>
#include <cmath>
#include <d3d11_2.h>
#include "Object3D.hpp"
#include "VertexType.hpp"
#include "Constants.hpp"

class Cylinder : public Object3D
{
private:
    ID3D11Buffer* _vertexBuffer = nullptr;
    ID3D11Buffer* _indexBuffer = nullptr;
    ID3D11Buffer* _instanceBuffer = nullptr;
    std::vector<DirectX::XMFLOAT4X4> _instanceTransforms;
    bool _caps;

public:

    Cylinder(const bool caps = true);
    Cylinder(const DirectX::XMFLOAT3& position, const bool caps = true);
    Cylinder(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& rotation, const bool caps = true);
    Cylinder(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& rotation, const DirectX::XMFLOAT3& scale, const bool caps = true);

    virtual ~Cylinder();

    void GenerateCylinderVertices(float radius, float height, int numSlices, std::vector<VertexPositionNormalUv>& vertices);
    void GenerateCylinderIndices(int numSlices, std::vector<uint32_t>& indices);

    bool Initialize(ID3D11Device* device) override;

    void Update(const float deltaTime) override;

    void Render(ID3D11DeviceContext* deviceContext, ID3D11Buffer* perObjectConstantBuffer) override;
};



