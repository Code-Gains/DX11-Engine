#pragma once
#define NOMINMAX // prevents windows headers from creating min max macros
#include <string>
#include <unordered_map>
#include "Definitions.hpp"
#include <d3d11_2.h>
#include <DirectXMath.h>
#include <FreeImage.h>
#include <algorithm>
#include <iostream>

// Loads textures and generates unique wstring IDs associated with them
class TextureManager
{
	std::unordered_map<std::wstring, WRL::ComPtr<ID3D11ShaderResourceView>> _idToTexture;
	std::wstring GenerateUniqueId(const std::wstring& baseId) const;
public:
	std::wstring LoadTexture(ID3D11Device* device, const std::wstring& filePath);
	std::wstring LoadTextureCubeFromSingleImage(ID3D11Device* device, const std::wstring& filePath);
	std::wstring CreateTextureNormalMapFromImage(ID3D11Device* device, const std::wstring& filePath);
	WRL::ComPtr<ID3D11ShaderResourceView> CreateTextureFromImage(ID3D11Device* device, const std::wstring& filePath);
	WRL::ComPtr<ID3D11ShaderResourceView> CreateTextureCubeFromSingleImage(ID3D11Device* device, const std::wstring& filePath);
	WRL::ComPtr<ID3D11ShaderResourceView> CreateNormalMapFromImage(ID3D11Device* device, const std::wstring& filePath);
	ID3D11ShaderResourceView* GetTexture(const std::wstring& id) const;

	std::vector<FIBITMAP*> SplitCubeImage(FIBITMAP* image);

	FIBITMAP* GenerateNormalMapFromHeightmap(FIBITMAP* heightMap);
	float GetPixelHeightClamped(unsigned int x, unsigned int y, FIBITMAP* heightMap);
	float GetPixelHeightNoWrap(unsigned int x, unsigned int y, FIBITMAP* heightMap);
};