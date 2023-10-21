#pragma once
#include <vector>
#include <map>

#include <cereal/types/vector.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/archives/json.hpp>
#include <fstream>

#include "World.hpp"

class Universe
{
	HWND _win32Window;
	WRL::ComPtr<ID3D11Device> _device = nullptr;
	WRL::ComPtr<ID3D11DeviceContext> _deviceContext = nullptr;

	//std::map<int, World> _worlds;
	World _world;
	int _viewportWidth;
	int _viewportHeight;

	friend class cereal::access;
public:
	Universe();
	Universe(HWND win32Window, ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	~Universe();

	void Update(float deltaTime);
	void PeriodicUpdate(float deltaTime);
	void Render();


	void UpdateViewportDimensions(int32_t width, int32_t height);
	bool LoadWorld(std::string filePath = "");
	bool SaveWorld(std::string filePath);


};

