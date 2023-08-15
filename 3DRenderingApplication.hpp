#pragma once
#include <d3d11_2.h>

#include <memory>
#include <DirectXMath.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include "imgui_impl_win32.h"
#include <imgui_impl_dx11.h>

#include "Definitions.hpp"
#include "Application.hpp"
#include "ShaderCollection.hpp"
#include "Cube.hpp"
#include "Sphere.hpp"
#include "Cylinder.hpp"
#include "WindowsXpPipesSimulation.hpp"
#include "Scene.hpp"
#include "PhongMaterial.hpp"
#include "3DRenderingApplication.hpp"
#include "ShaderCollection.hpp"
#include "ConstantBufferDefinitions.hpp"
#include "InstanceRenderer.hpp"


class Rendering3DApplication final : public Application
{
public:
    Rendering3DApplication(const std::string& title);
    ~Rendering3DApplication() override;

protected:
    bool Initialize() override;
    bool Load() override;
    void OnResize(
        int32_t width,
        int32_t height) override;
    void Update() override;
    void Render() override;

private:

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
    WRL::ComPtr<ID3D11Buffer> _cubeVertices = nullptr;
    WRL::ComPtr<ID3D11Buffer> _cubeIndices = nullptr;
    WRL::ComPtr<ID3D11Debug> _debug = nullptr;

    WRL::ComPtr<ID3D11SamplerState> _linearSamplerState = nullptr;
    WRL::ComPtr<ID3D11ShaderResourceView> _textureSrv = nullptr;
    WRL::ComPtr<ID3D11ShaderResourceView> _fallbackTextureSrv = nullptr;

    WRL::ComPtr<ID3D11Buffer> _perFrameConstantBuffer = nullptr;
    WRL::ComPtr<ID3D11Buffer> _cameraConstantBuffer = nullptr;
    WRL::ComPtr<ID3D11Buffer> _lightConstantBuffer = nullptr;
    WRL::ComPtr<ID3D11Buffer> _materialConstantBuffer = nullptr;
    WRL::ComPtr<ID3D11Buffer> _perObjectConstantBuffer = nullptr;
    WRL::ComPtr<ID3D11Buffer> _instanceConstantBuffer = nullptr;

    ShaderCollection _shaderCollection;

    PerFrameConstantBuffer _perFrameConstantBufferData{};
    CameraConstantBuffer _cameraConstantBufferData{};
    LightConstantBuffer _lightConstantBufferData{};
    MaterialConstantBuffer _materialConstantBufferData{};
    PerObjectConstantBuffer _perObjectConstantBufferData{};
    InstanceConstantBuffer _instanceConstantBufferData{};

    Scene _scene;
    InstanceRenderer _instanceRenderer;
};