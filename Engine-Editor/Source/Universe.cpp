#include "Universe.hpp"


Universe::Universe()
{
}

Universe::Universe(HWND win32Window, ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: _win32Window(win32Window), _device(device), _deviceContext(deviceContext)
{
}

Universe::~Universe()
{
}

void Universe::Update(float deltaTime)
{
	/*for (auto& pair : _worlds)
	{
		pair.second.Update(deltaTime);
	}*/
	_world.Update(deltaTime);
}

void Universe::PeriodicUpdate(float deltaTime)
{
	/*for (auto& pair : _worlds)
	{
		pair.second.PeriodicUpdate(deltaTime);
	}*/
	_world.PeriodicUpdate(deltaTime);
}

void Universe::Render()
{
	/*for (auto& pair : _worlds)
	{
		pair.second.Render();
	}*/
	_world.Render();
}

void Universe::UpdateViewportDimensions(int32_t width, int32_t height)
{
	_viewportWidth = width;
	_viewportHeight = height;
	/*for (auto& pair : _worlds)
	{
		pair.second.UpdateViewportDimensions(width, height);
	}*/
	_world.UpdateViewportDimensions(width, height);
}

bool Universe::LoadNewWorld()
{
	_world = World();
	_world.Initialize(this, _win32Window, _device.Get(), _deviceContext.Get());
	_world.UpdateViewportDimensions(_viewportWidth, _viewportHeight);
	_world.LoadWorld();
	return true;
}

bool Universe::LoadWorldSingle(std::string filePath)
{
	std::ifstream is(filePath);
	if (!is.is_open())
		return false;  // Failed to open file

	{
		cereal::JSONInputArchive archive(is);
		archive(CEREAL_NVP(_world));  // Deserializes the world
	}

	return true;  // Successfully deserialized
}

bool Universe::SaveWorld(std::string filePath)
{
	std::ofstream os(filePath);
	if (!os.is_open())
		return false;  // Failed to open file

	{
		cereal::JSONOutputArchive archive(os);
		archive(CEREAL_NVP(_world));  // Serializes the world
	}

	return true;  // Successfully serialized
}
