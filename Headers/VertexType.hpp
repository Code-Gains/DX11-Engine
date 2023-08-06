#pragma once
#include <DirectXMath.h>

enum class VertexType
{
    PositionNormalUv
};

using Position = DirectX::XMFLOAT3;
using Normal = DirectX::XMFLOAT3;
using Uv = DirectX::XMFLOAT2;


struct VertexPositionNormalUv
{
    Position position;
    Normal normal;
    Uv uv;
};