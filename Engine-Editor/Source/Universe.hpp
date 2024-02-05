#pragma once
#include <vector>
#include <map>

#include <cereal/types/vector.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/binary.hpp>
#include <fstream>

#include "World.hpp"
#include "RenderingApplication3D.hpp"

class Universe : public IEngineModule
{
	HWND _win32Window;
	WRL::ComPtr<ID3D11Device> _device = nullptr;
	WRL::ComPtr<ID3D11DeviceContext> _deviceContext = nullptr;
	int _viewportWidth;
	int _viewportHeight;

	//std::map<int, World> _worlds;
	World _world;

	friend class cereal::access;
public:
	Universe();
	Universe(HWND win32Window, ID3D11Device* device, ID3D11DeviceContext* deviceContext, int viewportWidth, int viewportHeight);
	virtual ~Universe() = default;

	virtual bool Initialize() override { return 0; };
	virtual bool Load() override { return 0; }
	virtual void Cleanup() override {};


	virtual void Update(float deltaTime) override;
	virtual void PeriodicUpdate(float deltaTime) override;
	virtual void Render() override;

	void UpdateViewportDimensions(int32_t width, int32_t height);
	bool LoadWorldSingle(std::string filePath = "");
	bool LoadNewWorld();
	bool SaveWorld(std::string filePath);
};

