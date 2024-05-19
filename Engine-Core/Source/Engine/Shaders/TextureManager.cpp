#include "TextureManager.hpp"

std::wstring TextureManager::GenerateUniqueId(const std::wstring& baseId) const
{
    // TODO IMPLEMENT KEY CHECKING
    return baseId;
}

std::wstring TextureManager::LoadTexture(ID3D11Device* device, const std::wstring& filePath)
{
    auto textureSrv = CreateTextureFromImage(device, filePath);

    if (!textureSrv)
        return L"";

    auto textureId = GenerateUniqueId(filePath);
    _idToTexture[textureId] = textureSrv;
    /*auto texName = std::string(textureId.begin(), textureId.end());
    std::cout << "Loaded Texture: " << texName << std::endl;*/
    return textureId;
}

std::wstring TextureManager::CreateTextureNormalMapFromImage(ID3D11Device* device, const std::wstring& filePath)
{
    
    auto textureSrv = CreateNormalMapFromImage(device, filePath);

    if (!textureSrv)
        return L"";

    auto textureId =  L"normalmap-" + GenerateUniqueId(filePath);
    _idToTexture[textureId] = textureSrv;
    /*auto texName = std::string(textureId.begin(), textureId.end());
    std::cout << "Loaded Texture: " << texName << std::endl;*/

    return textureId;
}

std::wstring TextureManager::LoadTextureCubeFromSingleImage(ID3D11Device* device, const std::wstring& filePath)
{
    auto textureSrv = CreateTextureCubeFromSingleImage(device, filePath);

    if (!textureSrv)
        return L"";

    auto textureId = GenerateUniqueId(filePath);
    _idToTexture[textureId] = textureSrv;

    return textureId;
}

WRL::ComPtr<ID3D11ShaderResourceView> TextureManager::CreateTextureFromImage(ID3D11Device* device, const std::wstring& filePath)
{
    std::string narrowPath(filePath.begin(), filePath.end());

    FREE_IMAGE_FORMAT format = FreeImage_GetFileType(narrowPath.c_str());
    FIBITMAP* bitmap = FreeImage_Load(format, narrowPath.c_str());
    if (!bitmap)
        return nullptr;

    FIBITMAP* bitmap32 = FreeImage_ConvertTo32Bits(bitmap);
    FreeImage_Unload(bitmap); // no longer needed

    bitmap = bitmap32;

    unsigned int width = FreeImage_GetWidth(bitmap);
    unsigned int height = FreeImage_GetHeight(bitmap);
    void* data = FreeImage_GetBits(bitmap);

    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width = width;
    texDesc.Height = height;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    texDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = data;
    initData.SysMemPitch = FreeImage_GetPitch(bitmap);

    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
    HRESULT hr = device->CreateTexture2D(&texDesc, &initData, texture.GetAddressOf());

    FreeImage_Unload(bitmap);
    if (FAILED(hr))
        return nullptr;

    // create Shader Resource View
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
    hr = device->CreateShaderResourceView(texture.Get(), nullptr, srv.GetAddressOf());
    if (FAILED(hr))
        return nullptr;

    return srv;
}

WRL::ComPtr<ID3D11ShaderResourceView> TextureManager::CreateTextureCubeFromSingleImage(ID3D11Device* device, const std::wstring& filePath)
{
    std::string narrowPath(filePath.begin(), filePath.end());

    FREE_IMAGE_FORMAT format = FreeImage_GetFileType(narrowPath.c_str());
    FIBITMAP* bitmap = FreeImage_Load(format, narrowPath.c_str());
    if (!bitmap)
        return nullptr;

    std::vector<FIBITMAP*> bitmaps = SplitCubeImage(bitmap);
    FreeImage_Unload(bitmap);

    if (bitmaps.size() != 6)
        return nullptr;

    unsigned int width = FreeImage_GetWidth(bitmaps[0]);
    unsigned int height = FreeImage_GetHeight(bitmaps[0]);
    unsigned int mipLevels = 1;

    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width = width;
    texDesc.Height = height;
    texDesc.MipLevels = mipLevels;
    texDesc.ArraySize = 6; // 6 faces for the cube
    texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

    std::vector<D3D11_SUBRESOURCE_DATA> initData(6);
    for (size_t i = 0; i < 6; ++i)
    {
        initData[i].pSysMem = FreeImage_GetBits(bitmaps[i]);
        initData[i].SysMemPitch = FreeImage_GetPitch(bitmaps[i]);
    }

    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
    HRESULT hr = device->CreateTexture2D(&texDesc, &initData[0], texture.GetAddressOf());

    //
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
    hr = device->CreateShaderResourceView(texture.Get(), nullptr, srv.GetAddressOf());
    if (FAILED(hr))
        return nullptr;

    return srv;
    //

    /*for (auto& bitmap : bitmaps)
    {
        FreeImage_Unload(bitmap);
    }*/

    /*if (FAILED(hr))
        return nullptr;

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = texDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
    srvDesc.TextureCube.MipLevels = texDesc.MipLevels;
    srvDesc.TextureCube.MostDetailedMip = 0;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
    hr = device->CreateShaderResourceView(texture.Get(), &srvDesc, srv.GetAddressOf());

    if (FAILED(hr))
        return nullptr;

    return srv;*/
}

WRL::ComPtr<ID3D11ShaderResourceView> TextureManager::CreateNormalMapFromImage(ID3D11Device* device, const std::wstring& filePath)
{
    std::string narrowPath(filePath.begin(), filePath.end());

    FREE_IMAGE_FORMAT format = FreeImage_GetFileType(narrowPath.c_str());
    FIBITMAP* bitmap = FreeImage_Load(format, narrowPath.c_str());
    if (!bitmap)
        return nullptr;

    FIBITMAP* bitmap32 = FreeImage_ConvertTo32Bits(bitmap);
    FreeImage_Unload(bitmap); // no longer needed

    FIBITMAP* normalBitmap = GenerateNormalMapFromHeightmap(bitmap32);
    FreeImage_Unload(bitmap32); // no longer needed
    bitmap = normalBitmap;


    unsigned int width = FreeImage_GetWidth(bitmap);
    unsigned int height = FreeImage_GetHeight(bitmap);
    void* data = FreeImage_GetBits(bitmap);

    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width = width;
    texDesc.Height = height;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    texDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = data;
    initData.SysMemPitch = FreeImage_GetPitch(bitmap);

    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
    HRESULT hr = device->CreateTexture2D(&texDesc, &initData, texture.GetAddressOf());

    FreeImage_Unload(bitmap);
    if (FAILED(hr))
        return nullptr;

    // create Shader Resource View
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
    hr = device->CreateShaderResourceView(texture.Get(), nullptr, srv.GetAddressOf());
    if (FAILED(hr))
        return nullptr;

    return srv;
}

ID3D11ShaderResourceView* TextureManager::GetTexture(const std::wstring& id) const
{
    auto it = _idToTexture.find(id);
    if (it != _idToTexture.end())
        return it->second.Get();

    return nullptr;
}

std::vector<FIBITMAP*> TextureManager::SplitCubeImage(FIBITMAP* image)
{
    unsigned int width = FreeImage_GetWidth(image);
    unsigned int height = FreeImage_GetHeight(image);

    unsigned int faceWidth = width / 4;
    unsigned int faceHeight = height / 3;

    std::vector<FIBITMAP*> faces(6, nullptr);

    // x;y for each face (starting block)
    int faceCoordinates[6][2] =
    {
        {2, 1},
        {0, 1},
        {1, 0},
        {1, 2},
        {1, 1},
        {3, 1}
    };

    for (int i = 0; i < 6; i++)
    {
        int x = faceCoordinates[i][0] * faceWidth;
        int y = faceCoordinates[i][1] * faceWidth;

        // Make a subimage from the cross skybox image
        faces[i] = FreeImage_Copy(image, x, x + faceWidth, y, y + faceHeight);

        if (!faces[i])
        {
            for (int j = 0; j < i; j++)
            {
                FreeImage_Unload(faces[j]);
            }
            faces.clear();
            break;
        }
    }

    return faces;
}

FIBITMAP* TextureManager::GenerateNormalMapFromHeightmap(FIBITMAP* heightMap)
{
    unsigned int width = FreeImage_GetWidth(heightMap);
    unsigned int height = FreeImage_GetHeight(heightMap);
    FIBITMAP* normalMap = FreeImage_Allocate(width, height, 32); // TODO find out why there is alpha here

    // encodes normal
    RGBQUAD color;

    for (unsigned int y = 0; y < height; y++)
    {
        for (unsigned int x = 0; x < width; x++)
        {
            float heightLeft = (x > 0) ? GetPixelHeightNoWrap(x - 1, y, heightMap) : GetPixelHeightNoWrap(x, y, heightMap);
            float heightRight = (x < width - 1) ? GetPixelHeightNoWrap(x + 1, y, heightMap) : GetPixelHeightNoWrap(x, y, heightMap);
            float heightDown = (y > 0) ? GetPixelHeightNoWrap(x, y - 1, heightMap) : GetPixelHeightNoWrap(x, y, heightMap);
            float heightUp = (y < height - 1) ? GetPixelHeightNoWrap(x, y + 1, heightMap) : GetPixelHeightNoWrap(x, y, heightMap);

            // x and y of the texture maps to x and z in the coordinate system
            float slope3DX = heightLeft - heightRight;
            float slope3DZ = heightDown - heightUp;

            // normalizing this means that y will reduce from 1
            auto normal = DirectX::XMFLOAT3(slope3DX, 2.0f, slope3DZ);

            auto normalizedVector = DirectX::XMLoadFloat3(&normal);
            normalizedVector = DirectX::XMVector3Normalize(normalizedVector);
            DirectX::XMStoreFloat3(&normal, normalizedVector);

            color.rgbRed = static_cast<byte>((normal.x * 0.5f + 0.5f) * 255);
            color.rgbGreen = static_cast<byte>((normal.y * 0.5f + 0.5f) * 255);
            color.rgbBlue = static_cast<byte>((normal.z * 0.5f + 0.5f) * 255);

            FreeImage_SetPixelColor(normalMap, x, y, &color);
        }
    }

    return normalMap;
}

// Automatically clamps in the range of width height
float TextureManager::GetPixelHeightClamped(unsigned int x, unsigned int y, FIBITMAP* heightMap)
{
    unsigned int clampedX = std::max(static_cast<unsigned int>(0), std::min(x, FreeImage_GetWidth(heightMap) - 1));
    unsigned int clampedY = std::max(static_cast<unsigned int>(0), std::min(y, FreeImage_GetHeight(heightMap) - 1));

    RGBQUAD color;
    if (FreeImage_GetPixelColor(heightMap, clampedX, clampedY, &color))
    {
        // normalized, it does not matter which color we sample because heightmap is black+white meaning all are same
        return color.rgbRed / 255.0f;
    }

    return 0.0f;
}

float TextureManager::GetPixelHeightNoWrap(unsigned int x, unsigned int y, FIBITMAP* heightMap)
{
    unsigned int width = FreeImage_GetWidth(heightMap);
    unsigned int height = FreeImage_GetHeight(heightMap);

    if (x >= width || y >= height)
        return 0.0f;


    RGBQUAD color;
    if (FreeImage_GetPixelColor(heightMap, x, y, &color))
        return color.rgbRed / 255.0f;

    return 0.0f;
}
