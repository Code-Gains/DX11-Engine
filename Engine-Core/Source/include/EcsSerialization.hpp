#pragma once
#include <bitset>
#include <cereal/cereal.hpp>

namespace cereal
{
	template <class Archive, size_t N>
	void save(Archive& archive, std::bitset<N> const& bitset)
	{
		std::string bitString = bitset.to_string();
		archive(bitString);
	}

	template <class Archive, size_t N>
	void load(Archive& archive, std::bitset<N> const& bitset)
	{
		std::string bitString;
		archive(bitString);
		bitset = std::bitset<N>(bitString);
	}
}