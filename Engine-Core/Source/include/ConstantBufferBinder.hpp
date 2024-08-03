#pragma once
#include <d3d11_2.h>
#include "DirectionalLightComponent.hpp"

class ConstantBufferBinder
{
    ID3D11DeviceContext* _deviceContext = nullptr;
    ConstantBufferBinder();
public:
    ConstantBufferBinder(const ConstantBufferBinder&) = delete;
    ConstantBufferBinder& operator=(const ConstantBufferBinder&) = delete;

    static ConstantBufferBinder& GetInstance()
    {
        static ConstantBufferBinder instance;
        return instance;
    }
    void Initialize(ID3D11DeviceContext* deviceContext);

    template<typename T>
    void BindConstantBuffer(ID3D11Buffer* buffer, const T& data, UINT slot, bool isVertexShader = true, bool isPixelShader = true)
    {
        if (!_deviceContext) throw std::runtime_error("Device context not initialized");

        D3D11_MAPPED_SUBRESOURCE mappedResource;
        _deviceContext->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        memcpy(mappedResource.pData, &data, sizeof(T));
        _deviceContext->Unmap(buffer, 0);

        if (isVertexShader)
            _deviceContext->VSSetConstantBuffers(slot, 1, &buffer);
        if (isPixelShader)
            _deviceContext->PSSetConstantBuffers(slot, 1, &buffer);
    }
};