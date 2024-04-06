#include "TextureManager.hpp"

//std::wstring TextureManager::GenerateUniqueId(const std::wstring& baseId) const
//{
//    // TODO IMPLEMENT KEY CHECKING
//    return baseId;
//}
//
//std::wstring TextureManager::LoadTexture(ID3D11Device* device, const std::wstring& filePath)
//{
//    auto textureSrv = CreateTextureFromImage(device, filePath);
//
//    if (!textureSrv)
//        return L"";
//
//    std::wstring textureId = GenerateUniqueId(filePath);
//    _idToTexture[textureId] = textureSrv;
//
//    return textureId;
//}
//
//WRL::ComPtr<ID3D11ShaderResourceView> TextureManager::CreateTextureFromImage(ID3D11Device* device, const std::wstring& filePath)
//{
//    std::string narrowPath(filePath.begin(), filePath.end());
//
//    FREE_IMAGE_FORMAT format = FreeImage_GetFileType(narrowPath.c_str());
//    FIBITMAP* bitmap = FreeImage_Load(format, narrowPath.c_str());
//    if (!bitmap)
//        return nullptr;
//
//    FIBITMAP* bitmap32 = FreeImage_ConvertTo32Bits(bitmap);
//    FreeImage_Unload(bitmap); // no longer needed
//
//    bitmap = bitmap32;
//
//    unsigned int width = FreeImage_GetWidth(bitmap);
//    unsigned int height = FreeImage_GetHeight(bitmap);
//    void* data = FreeImage_GetBits(bitmap);
//
//    D3D11_TEXTURE2D_DESC texDesc = {};
//    texDesc.Width = width;
//    texDesc.Height = height;
//    texDesc.MipLevels = 1;
//    texDesc.ArraySize = 1;
//    texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
//    texDesc.SampleDesc.Count = 1;
//    texDesc.Usage = D3D11_USAGE_DEFAULT;
//    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
//    texDesc.CPUAccessFlags = 0;
//
//    D3D11_SUBRESOURCE_DATA initData = {};
//    initData.pSysMem = data;
//    initData.SysMemPitch = FreeImage_GetPitch(bitmap);
//
//    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
//    HRESULT hr = device->CreateTexture2D(&texDesc, &initData, texture.GetAddressOf());
//    FreeImage_Unload(bitmap);
//    if (FAILED(hr))
//        return nullptr;
//
//    // create Shader Resource View
//    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
//    hr = device->CreateShaderResourceView(texture.Get(), nullptr, srv.GetAddressOf());
//    if (FAILED(hr))
//        return nullptr;
//
//    return srv;
//}
//
//ID3D11ShaderResourceView* TextureManager::GetTexture(const std::wstring& id) const
//{
//    auto it = _idToTexture.find(id);
//    if (it != _idToTexture.end())
//        return it->second.Get();
//
//    return nullptr;
//}
