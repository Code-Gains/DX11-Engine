#pragma once
#include <Object3D.hpp>
#include <d3d11_2.h>
#include <VertexType.hpp>
#include <vector>
#include <cmath>

class Cylinder : public Object3D
{
private:
    ID3D11Buffer* _vertexBuffer;
    ID3D11Buffer* _indexBuffer;

    bool _caps;

public:
    Cylinder(bool caps = true);

    Cylinder(DirectX::XMFLOAT3 position, bool caps = true);

    ~Cylinder();

    void GenerateCylinderVertices(float radius, float height, int numSlices, std::vector<VertexPositionNormalColorUv>& vertices);
    void GenerateCylinderIndices(int numSlices, std::vector<uint32_t>& indices);

    bool Initialize(ID3D11Device* device) override;

    void Update() override;

    void Render(ID3D11DeviceContext* deviceContext) override;
};



