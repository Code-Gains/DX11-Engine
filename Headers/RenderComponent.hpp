#pragma once
#include "Component.hpp"
#include <d3d11_2.h>
#include <cstddef>

class RenderComponent : public Component {
public:
    ID3D11Buffer* vertexBuffer = nullptr;
    ID3D11Buffer* indexBuffer = nullptr;
    std::size_t numIndices = 0;

    RenderComponent(ID3D11Buffer* vertexBuffer, ID3D11Buffer* indexBuffer, u_int numIndices)
        : vertexBuffer(vertexBuffer), indexBuffer(indexBuffer), numIndices(numIndices) {}
};