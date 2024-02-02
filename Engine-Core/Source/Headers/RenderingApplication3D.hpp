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
#include "Scene.hpp"
#include "ShaderCollection.hpp"
#include "ConstantBufferDefinitions.hpp"

//#include "World.hpp"
//#include "Universe.hpp"
#include "ResourceMonitor.hpp"

class IEngineModule
{
public:
    virtual bool Initialize() = 0;
    virtual bool Load() = 0;
    virtual void Cleanup() = 0;
    virtual void Render() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void PeriodicUpdate(float deltaTime) = 0;
};

class RenderingApplication3D final : public Application
{
public:
    RenderingApplication3D(const std::string& title, std::vector<std::unique_ptr<IEngineModule>>& engineModules);
    ~RenderingApplication3D() override;

    bool isApplicationMinimized = false;

protected:
    bool Initialize() override;
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
    void CreateConstantBuffers();
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

    ShaderCollection _shaderCollection;

    std::vector<std::unique_ptr<IEngineModule>> _engineModules;

    //Universe _universe; // #TODO move outside and allow to subscribe with IEngineModule interface
    //World _world;
    Scene _scene;
};