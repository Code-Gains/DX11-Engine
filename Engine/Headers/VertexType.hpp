#pragma once

#include <DirectXMath.h>

enum class VertexType
{
    PositionColor,
    PositionNormalColorUv
};

using Position = DirectX::XMFLOAT3;
using Normal = DirectX::XMFLOAT3;
using Color = DirectX::XMFLOAT3;
using Uv = DirectX::XMFLOAT2;

struct VertexPositionColor
{
    Position position;
    Color color;
};

struct VertexPositionNormalColorUv
{
    Position position;
    Normal normal;
    Color color;
    Uv uv;
};