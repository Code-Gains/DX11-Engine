#pragma once
#include <d3d11_2.h>

#include <memory>
#include <DirectXMath.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

#include "Definitions.hpp"
#include "Application.hpp"
#include "ShaderCollection.hpp"
#include "ConstantBufferDefinitions.hpp"

#include "ResourceMonitor.hpp"
#include "InstanceRendererSystem.hpp"
#include "ShaderManager.hpp"
#include "ECS.hpp"



class IEngineModule
{
public:
    virtual bool Initialize() = 0;
    virtual bool Load() = 0;
    virtual void Cleanup() = 0;
    virtual void Render() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void PeriodicUpdate(float deltaTime) = 0;

    virtual ~IEngineModule() = default;
};

class RenderingApplication3D final : public Application
{
public:
    RenderingApplication3D(const std::string& title);
    ~RenderingApplication3D() override;

    bool Initialize() override;

    bool isApplicationMinimized = false;

    ID3D11Device* GetApplicationDevice();
    ID3D11DeviceContext* GetApplicationDeviceContext();
    HWND GetApplicationWindow();

    void AddEngineModule(std::unique_ptr<IEngineModule>&& engineModule);
    void AddEngineModules(std::vector<std::unique_ptr<IEngineModule>>&& engineModules);

    // Instance Rendering System
    void LinkEngineInstancePools();
    void LinkRenderableInstancePool(int index, const InstanceRendererSystem::InstancePool& instancePool);
    void LinkRenderableInstancePool(const InstanceRendererSystem::InstancePool& instancePool);
    void AddRenderableInstance(int poolKey, int entityId, const InstanceConstantBuffer& instanceData);
    void UpdateRenderableInstanceData(int poolKey, int instanceIndex, const InstanceConstantBuffer& newData);
    void RemoveRenderableInstance(int poolKey, int entityId);
    void RemoveAllRenderableInstances();
    void ClearAllInstancePools();

    // Renderer Constant Buffers
    void SetLightConstantBuffer(const LightConstantBuffer& lightBuffer);
    void SetCameraConstantBuffer(const DirectX::XMFLOAT3& cameraPosition);
    void SetPerFrameConstantBuffer(const DirectX::XMMATRIX& viewProjection);

    // ----- ECS -----
    
    // Entities
    Entity CreateEntity();

    // Components
    template<typename TComponent>
    void AddComponent(Entity entity, const TComponent& component) const
    {
        return _ecs.AddComponent(entity, component);
    }

    template<typename TComponent>
    void RemoveComponent(Entity entity) const
    {
        _ecs.RemoveComponent<TComponent>(entity);
    }

    // Systems


protected:
    bool Load() override;

    void OnResize(
        int32_t width,
        int32_t height) override;

    void Update() override;
    void PeriodicUpdate() override;
    void Render() override;

private:
    ResourceMonitor _resourceMonitor;

    void CreateRasterState();
    void CreateDepthStencilView();
    void CreateDepthState();
    bool CreateSwapchainResources();
    void DestroySwapchainResources();

    WRL::ComPtr<ID3D11Device> _device = nullptr;
    WRL::ComPtr<ID3D11DeviceContext> _deviceContext = nullptr;
    WRL::ComPtr<IDXGIFactory2> _dxgiFactory = nullptr;
    WRL::ComPtr<IDXGISwapChain1> _swapChain = nullptr;
    WRL::ComPtr<ID3D11RenderTargetView> _renderTarget = nullptr;
    WRL::ComPtr<ID3D11DepthStencilView> _depthTarget = nullptr;
    WRL::ComPtr<ID3D11RasterizerState> _rasterState = nullptr;
    WRL::ComPtr<ID3D11DepthStencilState> _depthState = nullptr;
    WRL::ComPtr<ID3D11Debug> _debug = nullptr;

    WRL::ComPtr<ID3D11SamplerState> _linearSamplerState = nullptr;
    WRL::ComPtr<ID3D11ShaderResourceView> _textureSrv = nullptr;
    WRL::ComPtr<ID3D11ShaderResourceView> _fallbackTextureSrv = nullptr;

    ShaderManager _shaderManager;

    std::vector<std::unique_ptr<IEngineModule>> _engineModules;

    ECS _ecs;

    // --- Rendering Systems --- //
    std::unordered_map<int, InstanceRendererSystem::InstancePool> _instancePools;
    int _nextPoolId = 10000; // allocate 10000 to non user meshes TODO FIX
    InstanceRendererSystem _instanceRenderer;

    // HLSL Constant Buffer Data
    LightConstantBuffer _lightConstantBufferData{};
    PerFrameConstantBuffer _perFrameConstantBufferData{};
    CameraConstantBuffer _cameraConstantBufferData{};
    // -------------------------//
};