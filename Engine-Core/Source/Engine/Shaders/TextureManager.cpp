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
    void* data = FreeImage_GetBits(bitmap);
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
    {
        std::cerr << "Failed to load image." << std::endl;
        return nullptr;
    }

    // Split the image into 6 faces
    std::vector<FIBITMAP*> bitmaps = SplitCubeImage(bitmap);
    FreeImage_Unload(bitmap);

    if (bitmaps.size() != 6)
    {
        std::cerr << "Failed to split image into 6 faces." << std::endl;
        return nullptr;
    }

    // Get the width and height of each face (all identical because of / division)
    unsigned int width = FreeImage_GetWidth(bitmaps[0]);
    unsigned int height = FreeImage_GetHeight(bitmaps[0]);
    unsigned int mipLevels = 1;

    unsigned pitch = FreeImage_GetPitch(bitmaps[0]);
    for (size_t i = 0; i < 6; ++i)
    {
        unsigned facePitch = FreeImage_GetPitch(bitmaps[i]);
        if (facePitch != pitch)
        {
            std::cerr << "Pitch mismatch on face " << i << ". Expected: " << pitch << ", Got: " << facePitch << std::endl;
            for (auto& bitmap : bitmaps)
            {
                FreeImage_Unload(bitmap);
            }
            return nullptr;
        }
    }

    // Describe the texture cube
    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width = width;  // Width of each face
    texDesc.Height = height;  // Height of each face
    texDesc.MipLevels = mipLevels;
    texDesc.ArraySize = 6;  // Six faces for the cube
    texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;  // Format
    texDesc.SampleDesc.Count = 1;  // No multi-sampling
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

    // Initialize the subresource data
    std::vector<D3D11_SUBRESOURCE_DATA> initData(6);
    for (size_t i = 0; i < 6; i++)
    {
        initData[i].pSysMem = FreeImage_GetBits(bitmaps[i]);
        initData[i].SysMemPitch = FreeImage_GetPitch(bitmaps[i]);
    }

    // Create the texture cube
    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
    HRESULT hr = device->CreateTexture2D(&texDesc, initData.data(), texture.GetAddressOf());

    if (FAILED(hr))
    {
        std::cerr << "Failed to create Texture2D. HRESULT: " << std::hex << hr << std::endl;

        // Clean up FreeImage bitmaps
        for (auto& bitmap : bitmaps)
        {
            FreeImage_Unload(bitmap);
        }

        return nullptr;
    }

    // Clean up FreeImage bitmaps
    for (auto& bitmap : bitmaps)
    {
        FreeImage_Unload(bitmap);
    }

    // Describe the shader resource view for the texture cube
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = texDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
    srvDesc.TextureCube.MipLevels = texDesc.MipLevels;
    srvDesc.TextureCube.MostDetailedMip = 0;

    // Create the shader resource view
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
    hr = device->CreateShaderResourceView(texture.Get(), &srvDesc, srv.GetAddressOf());

    if (FAILED(hr))
    {
        std::cerr << "Failed to create ShaderResourceView. HRESULT: " << std::hex << hr << std::endl;
        return nullptr;
    }

    return srv;
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

    std::vector<FIBITMAP*> faces;

    // x;y for each face (starting block)
    int faceCoordinates[6][2] =
    {
        {2, 1}, // +X
        {0, 1}, // -X
        {1, 2}, // +Y
        {1, 0}, // -Y
        {1, 1}, // +Z
        {3, 1}  // -Z
    };

    for (int i = 0; i < 6; i++)
    {
        int x = faceCoordinates[i][0] * faceWidth;
        int y = faceCoordinates[i][1] * faceHeight;

        // Create a new image for the face
        FIBITMAP* face = FreeImage_Allocate(faceWidth, faceHeight, FreeImage_GetBPP(image));
        if (!face)
        {
            std::cerr << "Failed to allocate memory for face " << i << std::endl;
            for (auto& bitmap : faces)
            {
                FreeImage_Unload(bitmap);
            }
            faces.clear();
            break;
        }

        // Copy the pixels manually
        for (unsigned int fy = 0; fy < faceHeight; ++fy)
        {
            for (unsigned int fx = 0; fx < faceWidth; ++fx)
            {
                RGBQUAD color;
                if (FreeImage_GetPixelColor(image, x + fx, y + fy, &color))
                {
                    FreeImage_SetPixelColor(face, fx, fy, &color);
                }
                else
                {
                    std::cerr << "Failed to get pixel color at (" << x + fx << ", " << y + fy << ")" << std::endl;
                }
            }
        }

        FreeImage_FlipVertical(face);
        faces.push_back(face);
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
