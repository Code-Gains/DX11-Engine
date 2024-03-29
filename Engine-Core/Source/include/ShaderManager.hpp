#pragma once
#include <d3d11_2.h>
#include <unordered_map>
#include <string>
#include <memory>
#include <ShaderCollection.hpp>


//#include "VertexType.hpp"
//#include "Definitions.hpp"
//#include <d3d11_2.h>
//#include <cstdint>
//#include <string>
//#include <unordered_map>

class ShaderManager {
	ID3D11Device* _device;
	std::wstring _currentShaderId;
	// Collection can be [pixel + vertex]
	std::unordered_map<std::wstring, std::unique_ptr<ShaderCollection>> _shaderCollections;

public:
	ShaderManager() {};
	ShaderManager(ID3D11Device* device);

	bool LoadShaderCollection(const std::wstring& id, const ShaderCollectionDescriptor& descriptor);
	ShaderCollection* GetShaderCollection(const std::wstring& id);

	void ApplyToContext(const std::wstring& id, ID3D11DeviceContext* context);

	std::wstring GetCurrentShaderId() const;

	void Destroy();
};