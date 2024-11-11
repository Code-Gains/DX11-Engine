#pragma once

#include <vector>

class Heightmap
{
	std::vector<std::vector<float>> _heights;
	unsigned int _width;
	unsigned int _height;

public:
	Heightmap(unsigned int width, unsigned int height);
	Heightmap(const std::vector<std::vector<float>>& heights);

	float GetPointHeight(unsigned int x, unsigned int y) const;
	void SetHeight(unsigned int x, unsigned int y, float height);

	unsigned int GetWidth() const;
	unsigned int GetHeight() const;

	void SaveToFile() const;
	void LoadFromFile();

};