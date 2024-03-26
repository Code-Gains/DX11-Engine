#include "Universe.hpp"


Universe::Universe()
{
}

Universe::Universe(HWND win32Window, RenderingApplication3D* renderingApplication, ID3D11Device* device, ID3D11DeviceContext* deviceContext, int viewportWidth, int viewportHeight)
	: _win32Window(win32Window),
	_renderingApplication(renderingApplication),
	_device(device),
	_deviceContext(deviceContext),
	_viewportWidth(viewportWidth),
	_viewportHeight(viewportHeight)
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
	_viewportWidth = width;
	_viewportHeight = height;
	_world.UpdateViewportDimensions(width, height);
}

bool Universe::LoadNewWorld()
{
	_world.Initialize(_renderingApplication,this, _win32Window, _device.Get(), _deviceContext.Get());
	_world.UpdateViewportDimensions(_viewportWidth, _viewportHeight);
	_world.LoadWorld();
	return true;
}

bool Universe::LoadWorldSingle(std::string filePath)
{
	return _renderingApplication->LoadWorldSingle(filePath); 
}

bool Universe::SaveWorld(std::string filePath)
{
	return _renderingApplication->SaveWorld(filePath);
}
