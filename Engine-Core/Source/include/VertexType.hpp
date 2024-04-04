#pragma once
#include <DirectXMath.h>
#include <cereal/cereal.hpp>

enum class VertexType
{
    PositionNormalUv,
    PositionNormalUvHeight
};

using Position = DirectX::XMFLOAT3;
using Normal = DirectX::XMFLOAT3;
using Uv = DirectX::XMFLOAT2;


struct VertexPositionNormalUv
{
    // 32 bytes
    Position position; // 12
    Normal normal; // 12
    Uv uv; // 8
};

struct VertexPositionNormalUvHeight
{
    // 48 bytes
    Position position; // 12
    Normal normal; // 12
    Uv uv; // 8
    float height; // 4

    float padding[3]; // 12
};