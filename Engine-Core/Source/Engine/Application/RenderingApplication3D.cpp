#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <DirectXColors.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>

#include <iostream>
#include "RenderingApplication3D.hpp"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dxguid.lib")
//#pragma comment(lib, "FreeImaged.lib")

template <UINT TDebugNameLength>
inline void SetDebugName(_In_ ID3D11DeviceChild* deviceResource, _In_z_ const char(&debugName)[TDebugNameLength])
{
    deviceResource->SetPrivateData(WKPDID_D3DDebugObjectName, TDebugNameLength - 1, debugName);
}

RenderingApplication3D::RenderingApplication3D(const std::string& title)
    : Application(title)
{
}

RenderingApplication3D::~RenderingApplication3D()
{
    _deviceContext->Flush();
    _textureSrv.Reset();
    _rasterState.Reset();
    _depthState.Reset();
    _depthTarget.Reset();
    _shaderManager.Destroy();
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

ID3D11Device* RenderingApplication3D::GetApplicationDevice()
{
    return _device.Get();
}

ID3D11DeviceContext* RenderingApplication3D::GetApplicationDeviceContext()
{
    return _deviceContext.Get();
}

HWND RenderingApplication3D::GetApplicationWindow()
{
    return glfwGetWin32Window(GetWindow());
}

void RenderingApplication3D::AddEngineModule(std::unique_ptr<IEngineModule>&& engineModule)
{
    _engineModules.push_back(std::move(engineModule));
}

void RenderingApplication3D::AddEngineModules(std::vector<std::unique_ptr<IEngineModule>>&& engineModules)
{
    for (auto&& engineModule : engineModules)
    {
        _engineModules.push_back(std::move(engineModule));
    }
}

void RenderingApplication3D::AddRenderableInstance(int poolKey, int entityId, const InstanceConstantBuffer& instanceData)
{
    _instanceRenderer->AddInstance(poolKey, entityId, instanceData);
}

// TODO fix to remove any references to entities
void RenderingApplication3D::UpdateRenderableInstanceData(int poolKey, int instanceId, const InstanceConstantBuffer& newData)
{
    _instanceRenderer->UpdateInstanceData(poolKey, instanceId, newData);
}


//void RenderingApplication3D::RemoveAllRenderableInstances()
//{
//    for (auto& instancePoolPair : _instancePools)
//    {
//        InstanceRendererSystem::InstancePool& instancePool = instancePoolPair.second;
//        instancePool.entityIdToInstanceIndex.clear();
//        instancePool.instances.clear();
//        instancePool.instanceCount = 0;
//    }
//}

//void RenderingApplication3D::ClearAllInstancePools()
//{
//    for (auto& instancePool : _instancePools)
//    {
//        instancePool.second.Clear();
//    }
//}

void RenderingApplication3D::SetLightConstantBuffer(const LightConstantBuffer& lightBuffer)
{
    _lightConstantBufferData.Position = lightBuffer.Position;
    _lightConstantBufferData.Ambient = lightBuffer.Ambient;
    _lightConstantBufferData.Diffuse = lightBuffer.Diffuse;
    _lightConstantBufferData.Specular = lightBuffer.Specular;
}
void RenderingApplication3D::SetCameraConstantBuffer(const DirectX::XMFLOAT3& cameraPosition)
{
    _cameraConstantBufferData.cameraPosition = cameraPosition;
}
void RenderingApplication3D::SetPerFrameConstantBuffer(const DirectX::XMMATRIX& viewProjection)
{
    DirectX::XMStoreFloat4x4(&_perFrameConstantBufferData.viewProjectionMatrix, viewProjection);
}

Entity RenderingApplication3D::CreateEntity()
{
    return _ecs.CreateEntity();
}

void RenderingApplication3D::DestroyEntity(Entity entity)
{
    auto mesh = _ecs.GetComponent<MeshComponent<VertexPositionNormalUv>>(entity);
    _instanceRenderer->RemoveInstance(mesh->GetInstancePoolIndex(), entity);
    _ecs.DestroyEntity(entity);
}

bool RenderingApplication3D::Initialize()
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

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOther(_window, true);
    ImGui_ImplDX11_Init(_device.Get(), _deviceContext.Get());

    _resourceMonitor.Initialize(glfwGetWin32Window(GetWindow()), GetCurrentProcess());
    //LinkEngineInstancePools();

    return true;
}

void RenderingApplication3D::CreateRasterState()
{
    D3D11_RASTERIZER_DESC rasterDesc{};
    rasterDesc.CullMode = D3D11_CULL_BACK;
    rasterDesc.FillMode = D3D11_FILL_SOLID;

    _device->CreateRasterizerState(&rasterDesc, &_rasterState);
}

void RenderingApplication3D::CreateDepthStencilView()
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

void RenderingApplication3D::CreateDepthState()
{
    D3D11_DEPTH_STENCIL_DESC depthDesc{};
    depthDesc.DepthEnable = TRUE;
    depthDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;
    depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

    _device->CreateDepthStencilState(&depthDesc, &_depthState);
}

bool RenderingApplication3D::LoadWorldSingle(std::string filePath)
{
    std::ifstream is(filePath);
    if (!is.is_open())
        return false;  // Failed to open file
    {
        cereal::JSONInputArchive archive(is);
        archive(CEREAL_NVP(_ecs));
    }

    std::cout << "Finished saving from \"" << filePath << "\"\n";
    return true;  // Successfully deserialized
}

bool RenderingApplication3D::SaveWorld(std::string filePath)
{
    std::ofstream os(filePath);
    if (!os.is_open())
        return false;  // Failed to open file
    {
        cereal::JSONOutputArchive archive(os);
        archive(CEREAL_NVP(_ecs));
    }

    std::cout << "Finished loading to \"" << filePath << "\"\n";
    return true;  // Successfully serialized
}

bool RenderingApplication3D::Load()
{
    // TODO MOVE SHADER COLLECTION TO WORLD OR RENDERER
    ShaderCollectionDescriptor mainShaderDescriptor = {};
    mainShaderDescriptor.VertexShaderFilePath = L"Assets/Shaders/Main.vs.hlsl";
    mainShaderDescriptor.PixelShaderFilePath = L"Assets/Shaders/Main.ps.hlsl";
    mainShaderDescriptor.VertexType = VertexType::PositionNormalUv;

    ShaderCollectionDescriptor terrainShaderDescriptor = {};
    terrainShaderDescriptor.VertexShaderFilePath = L"Assets/Shaders/Terrain.vs.hlsl";
    terrainShaderDescriptor.PixelShaderFilePath = L"Assets/Shaders/Terrain.ps.hlsl";
    terrainShaderDescriptor.VertexType = VertexType::PositionNormalUv;

    _shaderManager = ShaderManager(_device.Get());
    _shaderManager.LoadShaderCollection(L"Main", mainShaderDescriptor);
    _shaderManager.LoadShaderCollection(L"Terrain", mainShaderDescriptor);

    //auto instanceRenderer = InstanceRendererSystem(_device.Get(), _deviceContext.Get());
    _ecs.AddSystem<ECSDebugger>(&_ecs);
    _ecs.AddSystem<InstanceRendererSystem>(_device.Get(), _deviceContext.Get(), &_shaderManager, &_ecs);
    _instanceRenderer = _ecs.GetSystem<InstanceRendererSystem>();
    _ecsDebugger = _ecs.GetSystem<ECSDebugger>();
    _instanceRenderer->LinkEngineInstancePools();

    std::cout << "Core Loading Complete!\n";
    return true;
}

bool RenderingApplication3D::CreateSwapchainResources()
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

void RenderingApplication3D::DestroySwapchainResources()
{
    _renderTarget.Reset();
}

void RenderingApplication3D::OnResize(const int32_t width, const int32_t height)
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
    //std::cout << "Application Resized! " << width << ";" << height << std::endl;
}

void RenderingApplication3D::Update()
{
    /*if (isApplicationMinimized)
        return;*/

    Application::Update();

    _resourceMonitor.Update(_deltaTime);
    _ecs.Update(_deltaTime);
    for (const auto& engineModule : _engineModules)
    {
        engineModule->Update(_periodicDeltaTime);
    }
}

void RenderingApplication3D::PeriodicUpdate()
{
    if (_periodicDeltaTime > _periodicUpdatePeriod)
    {
        _ecs.PeriodicUpdate(_deltaTime);
        for (const auto& engineModule : _engineModules)
        {
            engineModule->PeriodicUpdate(_periodicDeltaTime);
        }
        _periodicDeltaTime = 0;
    }
}

void RenderingApplication3D::Render()
{
    //if (isApplicationMinimized)
        //return;


    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplDX11_NewFrame();
    ImGui::NewFrame();
    _resourceMonitor.Render();

    float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };

    ID3D11RenderTargetView* nullRTV = nullptr;


    _deviceContext->OMSetRenderTargets(1, &nullRTV, nullptr);

    _deviceContext->ClearRenderTargetView(_renderTarget.Get(), clearColor);
    _deviceContext->ClearDepthStencilView(_depthTarget.Get(), D3D11_CLEAR_FLAG::D3D11_CLEAR_DEPTH, 1.0f, 0);

    _deviceContext->OMSetRenderTargets(1, _renderTarget.GetAddressOf(), _depthTarget.Get());

    _deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    //_shaderManager.ApplyToContext(L"Main", _deviceContext.Get());

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

    // :)
    //_instanceRenderer.RenderInstances<VertexPositionNormalUv>(_instancePools, _perFrameConstantBufferData, _cameraConstantBufferData, _lightConstantBufferData);
    //auto instanceRenderer = _ecs.GetSystem<InstanceRendererSystem>();
    _instanceRenderer->UpdateDirtyInstances();
    _instanceRenderer->RenderInstances<VertexPositionNormalUv>(_perFrameConstantBufferData, _cameraConstantBufferData, _lightConstantBufferData);
    //if(instanceRenderer)
        //std::cout << "instance renderer is active" << std::endl;

    _ecs.Render();

    for (const auto& engineModule : _engineModules)
    {
        engineModule->Render();
    }

    ImGui::End();
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // TODO Fix not working without changing NVIDIA 3D settings
    _swapChain->Present(0, 0); // 1st param is sync interval aka VSYNC (1-4 modes), 0 present immediately.
}
