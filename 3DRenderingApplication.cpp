#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <DirectXColors.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>

#include <iostream>
#include "3DRenderingApplication.hpp"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "FreeImaged.lib")

template <UINT TDebugNameLength>
inline void SetDebugName(_In_ ID3D11DeviceChild* deviceResource, _In_z_ const char(&debugName)[TDebugNameLength])
{
    deviceResource->SetPrivateData(WKPDID_D3DDebugObjectName, TDebugNameLength - 1, debugName);
}

Rendering3DApplication::Rendering3DApplication(const std::string& title)
    : Application(title)
{
}

Rendering3DApplication::~Rendering3DApplication()
{
    _deviceContext->Flush();
    _textureSrv.Reset();
    _cubeIndices.Reset();
    _cubeVertices.Reset();
    _perFrameConstantBuffer.Reset();
    _perObjectConstantBuffer.Reset();
    _instanceConstantBuffer.Reset();
    _rasterState.Reset();
    _depthState.Reset();
    _depthTarget.Reset();
    _shaderCollection.Destroy();
    DestroySwapchainResources();
    _swapChain.Reset();
    _dxgiFactory.Reset();
    _deviceContext.Reset();
#if !defined(NDEBUG)
    _debug->ReportLiveDeviceObjects(D3D11_RLDO_FLAGS::D3D11_RLDO_DETAIL);
    _debug.Reset();
#endif
    _device.Reset();
}

bool Rendering3DApplication::Initialize()
{
    if (!Application::Initialize())
    {
        return false;
    }

    if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&_dxgiFactory))))
    {
        std::cerr << "DXGI: Failed to create factory\n";
        return false;
    }

    constexpr D3D_FEATURE_LEVEL deviceFeatureLevel = D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0;
    uint32_t deviceFlags = 0;
#if !defined(NDEBUG)
    deviceFlags |= D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_DEBUG;
#endif

    WRL::ComPtr<ID3D11DeviceContext> deviceContext;
    if (FAILED(D3D11CreateDevice(
        nullptr,
        D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        deviceFlags,
        &deviceFeatureLevel,
        1,
        D3D11_SDK_VERSION,
        &_device,
        nullptr,
        &deviceContext)))
    {
        std::cerr << "D3D11: Failed to create Device and Device Context\n";
        return false;
    }

#if !defined(NDEBUG)
    if (FAILED(_device.As(&_debug)))
    {
        std::cerr << "D3D11: Failed to get the debug layer from the device\n";
        return false;
    }
#endif

    constexpr char deviceName[] = "DEV_Main";
    _device->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(deviceName), deviceName);
    SetDebugName(deviceContext.Get(), "CTX_Main");

    _deviceContext = deviceContext;


    DXGI_SWAP_CHAIN_DESC1 swapChainDescriptor = {};
    swapChainDescriptor.Width = GetWindowWidth();
    swapChainDescriptor.Height = GetWindowHeight();
    swapChainDescriptor.Format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;
    swapChainDescriptor.SampleDesc.Count = 1;
    swapChainDescriptor.SampleDesc.Quality = 0;
    swapChainDescriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDescriptor.BufferCount = 2;
    swapChainDescriptor.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDescriptor.Scaling = DXGI_SCALING::DXGI_SCALING_STRETCH;
    swapChainDescriptor.Flags = {};

    DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapChainFullscreenDescriptor = {};
    swapChainFullscreenDescriptor.Windowed = true;

    if (FAILED(_dxgiFactory->CreateSwapChainForHwnd(
        _device.Get(),
        glfwGetWin32Window(GetWindow()),
        &swapChainDescriptor,
        &swapChainFullscreenDescriptor,
        nullptr,
        &_swapChain)))
    {
        std::cerr << "DXGI: Failed to create SwapChain\n";
        return false;
    }

    CreateSwapchainResources();
    CreateRasterState();
    CreateDepthStencilView();
    CreateDepthState();
    CreateConstantBuffers();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOther(_window, true);
    ImGui_ImplDX11_Init(_device.Get(), _deviceContext.Get());

    return true;
}

void Rendering3DApplication::CreateRasterState()
{
    D3D11_RASTERIZER_DESC rasterDesc{};
    rasterDesc.CullMode = D3D11_CULL_NONE;
    rasterDesc.FillMode = D3D11_FILL_SOLID;

    _device->CreateRasterizerState(&rasterDesc, &_rasterState);
}

void Rendering3DApplication::CreateDepthStencilView()
{
    D3D11_TEXTURE2D_DESC texDesc{};
    texDesc.Height = GetWindowHeight();
    texDesc.Width = GetWindowWidth();
    texDesc.ArraySize = 1;
    texDesc.SampleDesc.Count = 1;
    texDesc.MipLevels = 1;
    texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    texDesc.Format = DXGI_FORMAT_R32_TYPELESS;

    ID3D11Texture2D* texture = nullptr;
    if (FAILED(_device->CreateTexture2D(&texDesc, nullptr, &texture)))
    {
        std::cerr << "D3D11: Failed to create texture for DepthStencilView\n";
        return;
    }

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    if (FAILED(_device->CreateDepthStencilView(texture, &dsvDesc, &_depthTarget)))
    {
        std::cerr << "D3D11: Failed to create DepthStencilView\n";
        texture->Release();
        return;
    }

    texture->Release();
}

void Rendering3DApplication::CreateDepthState()
{
    D3D11_DEPTH_STENCIL_DESC depthDesc{};
    depthDesc.DepthEnable = TRUE;
    depthDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;
    depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

    _device->CreateDepthStencilState(&depthDesc, &_depthState);
}

void Rendering3DApplication::CreateConstantBuffers()
{
    D3D11_BUFFER_DESC desc{};
    desc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    desc.ByteWidth = sizeof(PerFrameConstantBuffer);
    _device->CreateBuffer(&desc, nullptr, &_perFrameConstantBuffer);

    desc.ByteWidth = sizeof(CameraConstantBuffer);
    _device->CreateBuffer(&desc, nullptr, &_cameraConstantBuffer);

    desc.ByteWidth = sizeof(LightConstantBuffer);
    _device->CreateBuffer(&desc, nullptr, &_lightConstantBuffer);

    desc.ByteWidth = sizeof(MaterialConstantBuffer);
    _device->CreateBuffer(&desc, nullptr, &_materialConstantBuffer);

    desc.ByteWidth = sizeof(PerObjectConstantBuffer);
    _device->CreateBuffer(&desc, nullptr, &_perObjectConstantBuffer);

    desc.ByteWidth = sizeof(InstanceConstantBuffer) * 256;
    _device->CreateBuffer(&desc, nullptr, &_instanceConstantBuffer);
}

bool Rendering3DApplication::Load()
{
    ShaderCollectionDescriptor shaderDescriptor = {};
    shaderDescriptor.VertexShaderFilePath = L"Assets/Shaders/Main.vs.hlsl";
    shaderDescriptor.PixelShaderFilePath = L"Assets/Shaders/Main.ps.hlsl";
    shaderDescriptor.VertexType = VertexType::PositionNormalUv;

    _shaderCollection = ShaderCollection::CreateShaderCollection(shaderDescriptor, _device.Get());

    auto simulation = std::make_unique<WindowsXpPipesSimulation>(_device, Int3(30, 30, 30), 1000.0f);
    _scene.AddObject(std::move(simulation));

   /*auto cubeTemp = std::make_unique<Cube>(DirectX::XMFLOAT3{ 0, 0, 0 });
    std::vector<VertexPositionNormalUv> vertices = cubeTemp->GetVertices();
    std::vector<UINT> indices = cubeTemp->GetIndices();
    DirectX::XMMATRIX modelMatrix = cubeTemp->transform.GetWorldMatrix();*/


    //_instanceRenderer.InitializeInstancePool(_device.Get(), 0, vertices, indices, modelMatrix);
    //int gridSize = 100;
    //for (int x = 0; x < gridSize; x++)
    //{
    //    for (int y = 0; y < gridSize; y++)
    //    {
    //        for (int z = 0; z < gridSize; z++)
    //        {
    //            auto cube = std::make_unique<Cube>(DirectX::XMFLOAT3(x * 1.1, y * 1.1, z * 1.1));
    //            //cube->Initialize(_device.Get());
    //            //_scene.AddObject(std::move(cube));
    //            _instanceRenderer.AddInstance(InstanceConstantBuffer(cube->transform.GetWorldMatrix()), 0);
    //        }
    //    }
    //}
    ////_instanceRenderer.AddInstance(InstanceConstantBuffer(cube->transform.GetWorldMatrix()), 0);
    return true;
}

bool Rendering3DApplication::CreateSwapchainResources()
{
    WRL::ComPtr<ID3D11Texture2D> backBuffer = nullptr;
    if (FAILED(_swapChain->GetBuffer(
        0,
        IID_PPV_ARGS(&backBuffer))))
    {
        std::cerr << "D3D11: Failed to get back buffer from swapchain\n";
        return false;
    }

    if (FAILED(_device->CreateRenderTargetView(
        backBuffer.Get(),
        nullptr,
        &_renderTarget)))
    {
        std::cerr << "D3D11: Failed to create rendertarget view from back buffer\n";
        return false;
    }

    return true;
}

void Rendering3DApplication::DestroySwapchainResources()
{
    _renderTarget.Reset();
}

void Rendering3DApplication::OnResize(const int32_t width, const int32_t height)
{
    Application::OnResize(width, height);

    if (width <= 0 || height <= 0)
        isApplicationMinimized = true;
    else
        isApplicationMinimized = false;

    Application::OnResize(width, height);

    ID3D11RenderTargetView* nullRTV = nullptr;
    _deviceContext->OMSetRenderTargets(1, &nullRTV, nullptr);
    _deviceContext->Flush();

    DestroySwapchainResources();

    if (FAILED(_swapChain->ResizeBuffers(
        0,
        width,
        height,
        DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM,
        0)))
    {
        std::cerr << "D3D11: Failed to recreate swapchain buffers\n";
        return;
    }

    CreateSwapchainResources();

    _depthTarget.Reset();
    CreateDepthStencilView();
}

void Rendering3DApplication::Update()
{
    /*if (isApplicationMinimized)
        return;*/

    Application::Update();

    using namespace DirectX;

    static XMFLOAT3 _cameraPosition = { -15.0f, 15.0f, 15.0f };

    XMVECTOR camPos = XMLoadFloat3(&_cameraPosition);

    XMMATRIX view = XMMatrixLookAtRH(camPos, { 15.0f, 15.0f, 15.0f }, { 0,1,0,1 });
    XMMATRIX proj = XMMatrixPerspectiveFovRH(Constants::DegreesToRadians(90),
        static_cast<float>(_width) / static_cast<float>(_height),
        0.1f,
        100.0f);
    XMMATRIX viewProjection = XMMatrixMultiply(view, proj);
    XMStoreFloat4x4(&_perFrameConstantBufferData.viewProjectionMatrix, viewProjection);

    _lightConstantBufferData.Position = { -5.0f, 15.0f, 15.0f, 0.0f };
    _lightConstantBufferData.Ambient = { 0.4f, 0.4f, 0.4f, 1.0f };
    _lightConstantBufferData.Diffuse = { 0.5f, 0.5f, 0.5f, 1.0f };
    _lightConstantBufferData.Specular = { 0.0f, 0.0f, 0.0f, 1.0f };

    _scene.Update(_deltaTime);


}

void Rendering3DApplication::Render()
{
    //if (isApplicationMinimized)
        //return;

    static int framesPassed = 0;
    static float totalDeltaTime = 0;

    framesPassed++;
    totalDeltaTime += _deltaTime;

    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplDX11_NewFrame();
    ImGui::NewFrame();

    static int counter = 0;
    ImGui::Begin("Debug Info");
    ImGui::Text("FPS: %.2f", 1 / _deltaTime);
    ImGui::Text("AVG_FPS: %.2f", framesPassed / totalDeltaTime);

    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc)))
    {
        SIZE_T virtualMemoryUsed = pmc.PrivateUsage;
        ImGui::Text("RAM: %.2f MB", virtualMemoryUsed / (1024.0f * 1024.0f));
    }

    ImGui::Text("Geometry instances: %d", _scene.GetOwnershipCount());
    ImGui::End();

    ImGui::Render();
    float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };

    ID3D11RenderTargetView* nullRTV = nullptr;

    //set to nullptr so we can clear properly
    _deviceContext->OMSetRenderTargets(1, &nullRTV, nullptr);

    _deviceContext->ClearRenderTargetView(_renderTarget.Get(), clearColor);
    _deviceContext->ClearDepthStencilView(_depthTarget.Get(), D3D11_CLEAR_FLAG::D3D11_CLEAR_DEPTH, 1.0f, 0);

    _deviceContext->OMSetRenderTargets(1, _renderTarget.GetAddressOf(), _depthTarget.Get());

    _deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    _shaderCollection.ApplyToContext(_deviceContext.Get());

    D3D11_VIEWPORT viewport = {
        0.0f,
        0.0f,
        static_cast<float>(GetWindowWidth()),
        static_cast<float>(GetWindowHeight()),
        0.0f,
        1.0f
    };

    _deviceContext->RSSetViewports(1, &viewport);
    _deviceContext->RSSetState(_rasterState.Get());
    _deviceContext->OMSetDepthStencilState(_depthState.Get(), 0);

    ID3D11Buffer* constantPerFrameBuffers[4] = 
    {
        _perFrameConstantBuffer.Get(),
        _cameraConstantBuffer.Get(),
        _lightConstantBuffer.Get(),
        _materialConstantBuffer.Get()
    };

    ID3D11Buffer* constantPerObjectBuffers[2] = 
    {
        _perObjectConstantBuffer.Get(),
        _instanceConstantBuffer.Get()
    };

    _deviceContext->VSSetConstantBuffers(0, 4, constantPerFrameBuffers);
    _deviceContext->VSSetConstantBuffers(4, 2, constantPerObjectBuffers);

    _deviceContext->PSSetConstantBuffers(0, 4, constantPerFrameBuffers);
    _deviceContext->PSSetConstantBuffers(4, 2, constantPerObjectBuffers);

    D3D11_MAPPED_SUBRESOURCE mappedResource;

    _deviceContext->Map(_perFrameConstantBuffer.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    memcpy(mappedResource.pData, &_perFrameConstantBufferData, sizeof(PerFrameConstantBuffer));
    _deviceContext->Unmap(_perFrameConstantBuffer.Get(), 0);

    _deviceContext->Map(_cameraConstantBuffer.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    memcpy(mappedResource.pData, &_cameraConstantBufferData, sizeof(CameraConstantBuffer));
    _deviceContext->Unmap(_cameraConstantBuffer.Get(), 0);

    _deviceContext->Map(_lightConstantBuffer.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    memcpy(mappedResource.pData, &_lightConstantBufferData, sizeof(LightConstantBuffer));
    _deviceContext->Unmap(_lightConstantBuffer.Get(), 0);

    _deviceContext->Map(_materialConstantBuffer.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    memcpy(mappedResource.pData, &_materialConstantBufferData, sizeof(MaterialConstantBuffer));
    _deviceContext->Unmap(_materialConstantBuffer.Get(), 0);

    _scene.Render(_deviceContext.Get(), _perObjectConstantBuffer.Get(), _instanceConstantBuffer.Get());
    _instanceRenderer.RenderInstances<VertexPositionNormalUv>(_deviceContext.Get(), _perObjectConstantBuffer.Get(), _instanceConstantBuffer.Get());

    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    _swapChain->Present(DXGI_SWAP_EFFECT_DISCARD, 0); // 1st param is sync interval aka VSYNC (1-4 modes), 0 present immediately.

}