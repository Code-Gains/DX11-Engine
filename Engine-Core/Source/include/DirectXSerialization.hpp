#pragma once
#include <DirectXMath.h>
#include <cereal/cereal.hpp>
namespace cereal
{
	template <class Archive>
	void serialize(Archive& archive, DirectX::XMFLOAT4& vector4)
	{
		archive(cereal::make_nvp("x", vector4.x), cereal::make_nvp("y", vector4.y), cereal::make_nvp("z", vector4.z), cereal::make_nvp("w", vector4.w));
	}

	template <class Archive>
	void serialize(Archive& archive, DirectX::XMFLOAT3& vector3)
	{
		archive(cereal::make_nvp("x", vector3.x), cereal::make_nvp("y", vector3.y), cereal::make_nvp("z", vector3.z));
	}

	template <class Archive>
	void serialize(Archive& archive, DirectX::XMFLOAT2& vector2)
	{
		archive(cereal::make_nvp("x", vector2.x), cereal::make_nvp("y", vector2.y));
	}
}

