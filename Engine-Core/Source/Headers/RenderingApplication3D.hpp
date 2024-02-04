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

class GraphicsComponent
{
    HWND _window;
    WRL::ComPtr<ID3D11Device> _device = nullptr;
    WRL::ComPtr<ID3D11DeviceContext> _deviceContext = nullptr;
    int32_t _windowWidth;
    int32_t _windowHeight;
public:
    GraphicsComponent() {}
    GraphicsComponent(HWND window, ID3D11Device* device, ID3D11DeviceContext* deviceContext, int32_t windowWidth, int32_t windowHeight) :
        _window(window), _device(device), _deviceContext(deviceContext), _windowWidth(windowWidth), _windowHeight(windowHeight)
    {
    }

    HWND GetWindow() { return _window; }
    ID3D11Device* GetDevice() { return _device.Get(); }
    ID3D11DeviceContext* GetDeviceContext() { return _deviceContext.Get(); }
    int32_t GetWindowWidth() { return _windowWidth; }
    int32_t GetWindowHeight() { return _windowHeight; }

    void SetWindowWidth(int32_t width) { _windowWidth = width; }
    void SetWindowHeight(int32_t height) { _windowHeight = height; }
};

enum EngineModuleType
{
    Graphics
};

class IEngineModule
{
public:
    virtual bool Initialize() = 0;
    virtual bool Load() = 0;
    virtual void Cleanup() = 0;
    virtual void Render() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void PeriodicUpdate(float deltaTime) = 0;

    virtual GraphicsComponent* GetGraphicsComponent() const { return nullptr; }
    virtual EngineModuleType GetEngineModuleType() const = 0;

    virtual ~IEngineModule() = default;
};

class RenderingApplication3D final : public Application
{
public:
    RenderingApplication3D(const std::string& title);
    ~RenderingApplication3D() override;

    bool isApplicationMinimized = false;

    ID3D11Device* GetApplicationDevice();
    ID3D11DeviceContext* GetApplicationDeviceContext();
    HWND GetApplicationWindow();

    void AddEngineModule(std::unique_ptr<IEngineModule>&& engineModule);
    void AddEngineModules(std::vector<std::unique_ptr<IEngineModule>>&& engineModules);

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