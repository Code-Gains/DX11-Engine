#include "Heightmap.hpp"

Heightmap::Heightmap(unsigned int width, unsigned int height)
	: _width(width), _height(height)
{
	_heights.resize(_width);
	for (unsigned int i = 0; i < _width; ++i) {
		_heights[i].resize(_height, 0.0f);
	}
}

Heightmap::Heightmap(const std::vector<std::vector<float>>& heights)
{
	_heights = heights;
}


// TODO Implement exceptions
float Heightmap::GetHeight(unsigned int x, unsigned int y) const
{
	//if(x < _width && y < _height)
	return _heights[x][y];
}

void Heightmap::SetHeight(unsigned int x, unsigned int y, float height)
{
	_heights[x][y] = height;
}

void Heightmap::SaveToFile() const
{
}

void Heightmap::LoadFromFile()
{
}
