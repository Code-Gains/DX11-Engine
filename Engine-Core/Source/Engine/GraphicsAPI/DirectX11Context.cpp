#include "DirectX11Context.hpp"

template <UINT TDebugNameLength>
inline void SetDebugName(_In_ ID3D11DeviceChild* deviceResource, _In_z_ const char(&debugName)[TDebugNameLength])
{
    deviceResource->SetPrivateData(WKPDID_D3DDebugObjectName, TDebugNameLength - 1, debugName);
}

DirectX11Context::~DirectX11Context()
{
    _deviceContext->Flush();
    _textureSrv.Reset();
    _rasterState.Reset();
    _depthState.Reset();
    _depthTarget.Reset();
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

bool DirectX11Context::Initialize()
{
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
    swapChainDescriptor.Width = _windowWidth;
    swapChainDescriptor.Height = _windowHeight;
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
        _hwnd, // Update this as necessary
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

    // ImGui Initialization
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOther(_window, false); // Update as necessary
    ImGui_ImplDX11_Init(_device.Get(), _deviceContext.Get());

    return true;
}

void DirectX11Context::OnResize(int32_t width, int32_t height)
{
    if (width <= 0 || height <= 0)
        return;
    _windowWidth = width;
    _windowHeight = height;

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

bool DirectX11Context::CreateSwapchainResources()
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

bool DirectX11Context::CreateRenderTargetView()
{
    return false;
}

void DirectX11Context::DestroySwapchainResources()
{
    _renderTarget.Reset();
}


void DirectX11Context::CreateRasterState()
{
    D3D11_RASTERIZER_DESC rasterDesc{};
    rasterDesc.CullMode = D3D11_CULL_BACK;
    rasterDesc.FillMode = D3D11_FILL_SOLID;

    _device->CreateRasterizerState(&rasterDesc, &_rasterState);
}

void DirectX11Context::CreateDepthStencilView()
{
    D3D11_TEXTURE2D_DESC texDesc{};
    texDesc.Width = _windowWidth;
    texDesc.Height = _windowHeight;
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
    HRESULT hr = _device->CreateDepthStencilView(texture, &dsvDesc, &_depthTarget);
    texture->Release();
    if (FAILED(hr))
    {
        std::cerr << "D3D11: Failed to create DepthStencilView\n";
        return;
    }
}
