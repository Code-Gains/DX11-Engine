#pragma once
#include <DirectXMath.h>
#include "ECS.hpp"

struct DirectionalLightConstantBuffer
{
	DirectX::XMFLOAT4 direction;
	DirectX::XMFLOAT4 ambient;
	DirectX::XMFLOAT4 diffuse;
	DirectX::XMFLOAT4 specular;

	DirectionalLightConstantBuffer();
	DirectionalLightConstantBuffer(
		const DirectX::XMFLOAT4& direction,
		const DirectX::XMFLOAT4& ambient,
		const DirectX::XMFLOAT4& diffuse,
		const DirectX::XMFLOAT4& specular
	);
};

class DirectionalLightComponent : public IComponent
{
	DirectionalLightConstantBuffer _directionalLightConstantBuffer;

public:
	DirectionalLightComponent();
	DirectionalLightComponent(
		const DirectX::XMFLOAT4& direction,
		const DirectX::XMFLOAT4& ambient,
		const DirectX::XMFLOAT4& diffuse,
		const DirectX::XMFLOAT4& specular
	);

	DirectX::XMFLOAT4 GetDirection() const;
	DirectX::XMFLOAT4 GetAmbient() const;
	DirectX::XMFLOAT4 GetDiffuse() const;
	DirectX::XMFLOAT4 GetSpecular() const;

	DirectionalLightConstantBuffer GetConstantBuffer();

	void SetDirection(const DirectX::XMFLOAT4& direction);
	void SetAmbient(const DirectX::XMFLOAT4& ambient);
	void SetDiffuse(const DirectX::XMFLOAT4& diffuse);
	void SetSpecular(const DirectX::XMFLOAT4& specular);
};

