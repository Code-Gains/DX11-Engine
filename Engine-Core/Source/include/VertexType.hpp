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
    // 32 bytes
    Position position; // 12
    Normal normal; // 12
    Uv uv; // 8

    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(CEREAL_NVP(position), CEREAL_NVP(normal), CEREAL_NVP(uv));
    }
};

struct VertexPositionNormalUvHeight
{
    // 48 bytes
    Position position; // 12
    Normal normal; // 12
    Uv uv; // 8
    float height; // 4

    float padding[3]; // 12

    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(CEREAL_NVP(position), CEREAL_NVP(normal), CEREAL_NVP(uv), CEREAL_NVP(height));
    }
};