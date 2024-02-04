#include "Universe.hpp"


Universe::Universe()
{
}

Universe::Universe(HWND win32Window, ID3D11Device* device, ID3D11DeviceContext* deviceContext, int viewportWidth, int viewportHeight)
	: 
	_graphicsComponent(std::make_unique<GraphicsComponent>(win32Window, device, deviceContext, viewportWidth, viewportHeight))
{
	LoadNewWorld();
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
	_graphicsComponent->SetWindowWidth(width);
	_graphicsComponent->SetWindowHeight(height);
	/*for (auto& pair : _worlds)
	{
		pair.second.UpdateViewportDimensions(width, height);
	}*/
	_world.UpdateViewportDimensions(width, height);
}

bool Universe::LoadNewWorld()
{
	_world = World();
	_world.Initialize(this, _graphicsComponent->GetWindow(), _graphicsComponent->GetDevice(), _graphicsComponent->GetDeviceContext());
	_world.UpdateViewportDimensions(_graphicsComponent->GetWindowWidth(), _graphicsComponent->GetWindowHeight());
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
