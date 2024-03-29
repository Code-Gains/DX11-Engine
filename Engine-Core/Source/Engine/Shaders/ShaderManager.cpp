#include "ShaderManager.hpp"

ShaderManager::ShaderManager(ID3D11Device* device) : _device(device)
{
}

bool ShaderManager::LoadShaderCollection(const std::wstring& id, const ShaderCollectionDescriptor& descriptor)
{
	auto collection = ShaderCollection::CreateShaderCollection(descriptor, _device);
	//TODO ADD VALIDATION
	_shaderCollections[id] = std::make_unique<ShaderCollection>(std::move(collection));
	return true;
}

ShaderCollection* ShaderManager::GetShaderCollection(const std::wstring& id)
{
	auto it = _shaderCollections.find(id);
	if (it != _shaderCollections.end())
		return it->second.get();

	return nullptr;
}

void ShaderManager::ApplyToContext(const std::wstring& id, ID3D11DeviceContext* context)
{
	auto it = _shaderCollections.find(id);
	if (it != _shaderCollections.end())
	{
		it->second->ApplyToContext(context);
		return;
	}

	std::cerr << "Could not find the requested shader collection!\n";
}

std::wstring ShaderManager::GetCurrentShaderId() const
{
	return _currentShaderId;
}

void ShaderManager::Destroy()
{
	for (auto& pair : _shaderCollections)
	{
		if (pair.second)
			pair.second->Destroy();
	}
}


