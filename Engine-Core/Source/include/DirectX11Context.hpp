#pragma once
#include "GraphicsContext.hpp"
#include <d3d11_2.h>
#include <DirectXMath.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

#include "DirectionalLightComponent.hpp"

class DirectX11Context : public IGraphicsContext
{
public:
    ~DirectX11Context();
    virtual bool Initialize() override;
    virtual void Cleanup() override {};
    virtual void Render() override {};

    ID3D11Device* GetDevice() const { return _device.Get(); }
    ID3D11DeviceContext* GetDeviceContext() const { return _deviceContext.Get(); }
    IDXGISwapChain* GetSwapChain() const { return _swapChain.Get(); }
    HWND GetHwnd() const { return _hwnd; }
    ID3D11RenderTargetView* GetRenderTarget() const { return _renderTarget.Get(); };
    ID3D11DepthStencilView* GetDepthTarget() const { return _depthTarget.Get(); };
    ID3D11RasterizerState* GetRasterState() const { return _rasterState.Get(); };

    void OnResize(int32_t width, int32_t height);


    void SetHWND(HWND hwnd) { _hwnd = hwnd; }
    void SetWindow(GLFWwindow* window) { _window = window; }
    void SetWindowWidth(int32_t width) { _windowWidth = width; }
    void SetWindowHeight(int32_t height) { _windowHeight = height; }

    int32_t _windowWidth;
    int32_t _windowHeight;

private:
    bool CreateSwapchainResources();
    bool CreateRenderTargetView();
    void CreateRasterState();
    void CreateDepthStencilView();
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

    HWND _hwnd = nullptr;
    GLFWwindow* _window;
};