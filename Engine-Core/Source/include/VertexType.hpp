#pragma once
#include <DirectXMath.h>
#include <cereal/cereal.hpp>

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

    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(CEREAL_NVP(position), CEREAL_NVP(normal), CEREAL_NVP(uv));
    }
};