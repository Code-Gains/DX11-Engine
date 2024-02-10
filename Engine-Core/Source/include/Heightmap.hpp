#pragma once

#include <vector>

class Heightmap
{
	std::vector<std::vector<float>> _heights;
	unsigned int _width;
	unsigned int _height;

public:
	Heightmap(unsigned int width, unsigned int height);

	float GetHeight(unsigned int x, unsigned int y) const;

	void SetHeight(unsigned int x, unsigned int y, float height);

	void SaveToFile() const;
	void LoadFromFile();

};