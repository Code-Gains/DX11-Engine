#include "LightingSystem.hpp"

LightingSystem::LightingSystem()
{
}

LightingSystem::LightingSystem(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ECS* ecs)
    : _device(device), _deviceContext(deviceContext), _ecs(ecs)
{
    InitializeBuffers();
}

void LightingSystem::InitializeBuffers()
{
    D3D11_BUFFER_DESC desc{};
    desc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    desc.ByteWidth = sizeof(DirectionalLightConstantBuffer);
    HRESULT hr = _device->CreateBuffer(&desc, nullptr, &_directionalLightConstantBuffer);
    if (FAILED(hr))
    {
        throw std::runtime_error("Failed to create directional light buffer");
    }
}

void LightingSystem::Render()
{
}

void LightingSystem::Update(float deltaTime)
{
    // querry returns a vector of tuples that contain component vectors
    auto componentQueryResult = _ecs->QueryComponentVectors<DirectionalLightComponent>();
    // iterate
    for (auto& tuple : componentQueryResult)
    {
        auto& directionalLights = std::get<0>(tuple);

        // We want to work with the std::vector
        auto& rawDirectionalLights = *directionalLights->GetRawVector();

        for (auto& entityToComponent : directionalLights->GetEntityToIndex())
        {
            auto id = entityToComponent.first;
            auto idx = entityToComponent.second;

            auto& directionalLight = rawDirectionalLights[idx];

            UpdateDirectionalLightBuffer(directionalLight.GetConstantBuffer());
        }
    }
}

void LightingSystem::PeriodicUpdate(float deltaTime)
{

}

void LightingSystem::UpdateDirectionalLightBuffer(const DirectionalLightConstantBuffer& directionalLightBuffer)
{
    //std::cout << "Setting DL buffer\n";
    //if (!_deviceContext)
    //{
    //    throw std::runtime_error("Device context is null");
    //}

    //if (!_directionalLightConstantBuffer)
    //{
    //    throw std::runtime_error("Directional light buffer is not initialized");
    //}

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT hr = _deviceContext->Map(_directionalLightConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(hr))
    {
        throw std::runtime_error("Failed to map directional light buffer");
    }

    memcpy(mappedResource.pData, &directionalLightBuffer, sizeof(DirectionalLightConstantBuffer));
    _deviceContext->Unmap(_directionalLightConstantBuffer.Get(), 0);

    // Bind the buffer to the pipeline
    _deviceContext->VSSetConstantBuffers(2, 1, _directionalLightConstantBuffer.GetAddressOf());
    _deviceContext->PSSetConstantBuffers(2, 1, _directionalLightConstantBuffer.GetAddressOf());
}
