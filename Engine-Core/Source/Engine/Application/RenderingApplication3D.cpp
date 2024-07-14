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
    _graphicsContext = std::make_unique<DirectX11Context>();
}

RenderingApplication3D::~RenderingApplication3D()
{
    //_deviceContext->Flush();
    //_textureSrv.Reset();
    //_rasterState.Reset();
    //_depthState.Reset();
    //_depthTarget.Reset();
    //_graphicsContext->Cleanup();
    _shaderManager.Destroy();
    //DestroySwapchainResources();
    //_swapChain.Reset();
    //_dxgiFactory.Reset();
    //_deviceContext.Reset();
//#if !defined(NDEBUG)
   // _debug->ReportLiveDeviceObjects(D3D11_RLDO_FLAGS::D3D11_RLDO_DETAIL);
    //_debug.Reset();
//#endif
    //_device.Reset();
}

ID3D11Device* RenderingApplication3D::GetApplicationDevice()
{
    return _graphicsContext->GetDevice();
}

ID3D11DeviceContext* RenderingApplication3D::GetApplicationDeviceContext()
{
    return _graphicsContext->GetDeviceContext();
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

void RenderingApplication3D::SetPerFrameConstantBuffer(const DirectX::XMMATRIX& viewProjection)
{
    DirectX::XMStoreFloat4x4(&_perFrameConstantBufferData.viewProjectionMatrix, viewProjection);
}

ECS* RenderingApplication3D::GetECS()
{ 
    return &_ecs;
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
    _resourceMonitor.Initialize(glfwGetWin32Window(GetWindow()), GetCurrentProcess());
    _graphicsContext->SetHWND(glfwGetWin32Window(GetWindow()));
    _graphicsContext->SetWindow(GetWindow());
    _graphicsContext->SetWindowWidth(GetWindowWidth());
    _graphicsContext->SetWindowHeight(GetWindowHeight());
    return _graphicsContext->Initialize();
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
    auto device = _graphicsContext->GetDevice();
    auto deviceContext = _graphicsContext->GetDeviceContext();

    // TODO MOVE SHADER COLLECTION TO WORLD OR RENDERER
    ShaderCollectionDescriptor mainShaderDescriptor = {};
    mainShaderDescriptor.VertexShaderFilePath = L"../../../../Assets/Shaders/Main.vs.hlsl";
    mainShaderDescriptor.PixelShaderFilePath = L"../../../../Assets/Shaders/Main.ps.hlsl";
    mainShaderDescriptor.VertexType = VertexType::PositionNormalUv;

    ShaderCollectionDescriptor terrainShaderDescriptor = {};
    terrainShaderDescriptor.VertexShaderFilePath = L"../../../../Assets/Shaders/Terrain.vs.hlsl";
    terrainShaderDescriptor.PixelShaderFilePath = L"../../../../Assets/Shaders/Terrain.ps.hlsl";
    terrainShaderDescriptor.VertexType = VertexType::PositionNormalUvHeight;

    ShaderCollectionDescriptor skyboxShaderDescriptor = {};
    skyboxShaderDescriptor.VertexShaderFilePath = L"../../../../Assets/Shaders/Skybox.vs.hlsl";
    skyboxShaderDescriptor.PixelShaderFilePath = L"../../../../Assets/Shaders/Skybox.ps.hlsl";
    skyboxShaderDescriptor.VertexType = VertexType::PositionNormalUv;

    _shaderManager = ShaderManager(device);
    _shaderManager.LoadShaderCollection(L"Main", mainShaderDescriptor);
    _shaderManager.LoadShaderCollection(L"Terrain", terrainShaderDescriptor);
    _shaderManager.LoadShaderCollection(L"Skybox", skyboxShaderDescriptor);

    // TODO MOVE OPTIONAL SYSTEMS OUT OF CORE
    _ecs.AddSystem<ECSDebugger>(&_ecs);
    _ecs.AddSystem<InstanceRendererSystem>(device, deviceContext, &_shaderManager, &_ecs);
    _instanceRenderer = _ecs.GetSystem<InstanceRendererSystem>();
    _instanceRenderer->Initialize();
    _ecsDebugger = _ecs.GetSystem<ECSDebugger>();
    _instanceRenderer->LinkEngineInstancePools();

    std::cout << "Core Loading Complete!\n";
    return true;
}

void RenderingApplication3D::OnResize(const int32_t width, const int32_t height)
{
    Application::OnResize(width, height);
    _graphicsContext->OnResize(width, height);
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

    auto deviceContext = _graphicsContext->GetDeviceContext();
    auto renderTarget = _graphicsContext->GetRenderTarget();
    auto depthTarget = _graphicsContext->GetDepthTarget();
    auto rasterState = _graphicsContext->GetRasterState();
    auto swapchain = _graphicsContext->GetSwapChain();

    deviceContext->OMSetRenderTargets(1, &nullRTV, nullptr);

    deviceContext->ClearRenderTargetView(renderTarget, clearColor);
    deviceContext->ClearDepthStencilView(depthTarget, D3D11_CLEAR_FLAG::D3D11_CLEAR_DEPTH, 1.0f, 0);

    deviceContext->OMSetRenderTargets(1, &renderTarget, depthTarget);

    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    D3D11_VIEWPORT viewport = {
        0.0f,
        0.0f,
        static_cast<float>(GetWindowWidth()),
        static_cast<float>(GetWindowHeight()),
        0.0f,
        1.0f
    };

    deviceContext->RSSetViewports(1, &viewport);
    deviceContext->RSSetState(rasterState);
    //_deviceContext->OMSetDepthStencilState(_depthState.Get(), 0);

    _instanceRenderer->UpdateDirtyInstances();

    static auto tempBuffer = DirectionalLightConstantBuffer
    (
        DirectX::XMFLOAT4(-0.5f, -1.0f, -0.5f, 0.0f),
        DirectX::XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f),
        DirectX::XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f),
        DirectX::XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f)
    );

    _instanceRenderer->RenderInstances<VertexPositionNormalUv>(_perFrameConstantBufferData, tempBuffer);

    _ecs.Render();

    for (const auto& engineModule : _engineModules)
    {
        engineModule->Render();
    }

    ImGui::End();
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // TODO Fix not working without changing NVIDIA 3D settings
    swapchain->Present(1, 0); // 1st param is sync interval aka VSYNC (1-4 modes), 0 present immediately.
}
