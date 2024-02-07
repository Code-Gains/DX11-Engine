#include "IntVectors.hpp"

Int3::Int3(const int x, const int y, const int z): x(x), y(y), z(z)
{
}

Int3 Int3::operator+(const Int3& other) const
{
	return Int3(x + other.x, y + other.y, z + other.z);
}

Int3 Int3::operator-(const Int3& other) const
{
	return Int3(x - other.x, y - other.y, z - other.z);
}