#include "ConstantBufferBinder.hpp"

ConstantBufferBinder::ConstantBufferBinder()
{
}

void ConstantBufferBinder::Initialize(ID3D11DeviceContext* deviceContext)
{
	_deviceContext = deviceContext;
}
