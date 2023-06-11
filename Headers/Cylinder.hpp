#pragma once
#include <Object3D.hpp>
#include <d3d11_2.h>
#include <VertexType.hpp>
#include <vector>
#include <cmath>
#include <Constants.hpp>

class Cylinder : public Object3D
{
private:
    ID3D11Buffer* _vertexBuffer = nullptr;
    ID3D11Buffer* _indexBuffer = nullptr;

    bool _caps;

public:

    Cylinder(const bool caps = true);
    Cylinder(const DirectX::XMFLOAT3& position, const bool caps = true);
    Cylinder(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& rotation, const bool caps = true);
    Cylinder(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& rotation, const DirectX::XMFLOAT3& scale, const bool caps = true);

    virtual ~Cylinder();

    void GenerateCylinderVertices(float radius, float height, int numSlices, std::vector<VertexPositionNormalColorUv>& vertices);
    void GenerateCylinderIndices(int numSlices, std::vector<uint32_t>& indices);

    bool Initialize(ID3D11Device* device) override;

    void Update(const float deltaTime) override;

    void Render(ID3D11DeviceContext* deviceContext, ID3D11Buffer* perObjectConstantBuffer) override;
};



