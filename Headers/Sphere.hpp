#pragma once
#include <Object3D.hpp>
#include <d3d11_2.h>
#include <vector>
#include <cmath>
#include "Constants.hpp"
#include "VertexType.hpp"

class Sphere : public Object3D
{
private:
    ID3D11Buffer* _vertexBuffer;
    ID3D11Buffer* _indexBuffer;

public:
    Sphere();

    Sphere(DirectX::XMFLOAT3 position);

    virtual ~Sphere();

    void GenerateSphereVertices(float radius, int numSlices, int numStacks, std::vector<VertexPositionNormalColorUv>& vertices);
    void GenerateSphereIndices(int numSlices, int numStacks, std::vector<uint32_t>& indices);

    bool Initialize(ID3D11Device* device) override;

    void Update(const float deltaTime) override;

    void Render(ID3D11DeviceContext* deviceContext, ID3D11Buffer* perObjectConstantBuffer) override;
};

