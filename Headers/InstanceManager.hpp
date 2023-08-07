#pragma once
#include <vector>
#include <d3d11_2.h>
#include "ConstantBufferDefinitions.hpp"

class InstanceManager
{
    std::vector<InstanceConstantBuffer> _instances;
    ID3D11Buffer* _instanceConstantBuffer;
public:
    InstanceManager() {};
    ~InstanceManager() {};
    void AddInstance(const InstanceConstantBuffer& instanceData);
    void UpdateInstanceData(int instanceIndex, const InstanceConstantBuffer& newData);
    void RemoveInstance(int instanceIndex);
    void RenderInstances(ID3D11DeviceContext* deviceContext, ID3D11Buffer* perObjectConstantBuffer);
};