#pragma once
#include <d3d11_2.h>
#include <DirectXMath.h>
#include <vector>
#include <array>

#include "Definitions.hpp"
#include "VertexType.hpp"

#include "TransformComponent.hpp"
#include "MeshComponent.hpp"
#include "MaterialComponent.hpp"
#include "LightComponent.hpp"



class RenderingSystem
{
	WRL::ComPtr<ID3D11Buffer> _vertexBuffer = nullptr;
	WRL::ComPtr<ID3D11Buffer> _indexBuffer = nullptr;
	std::vector<VertexPositionNormalUv> _vertices;
	std::vector<UINT> _indices;

public:
	RenderingSystem();

	bool Initialize(ID3D11Device* device);
	void Update(float deltaTime);
	void Render(
		ID3D11DeviceContext* deviceContext,
		ID3D11Buffer* perObjectConstantBuffer,
		ID3D11Buffer* instanceConstantBuffer,
		const std::vector<TransformComponent>& _transformComponents,
		const std::vector<MeshComponent>& _meshComponents,
		const std::vector<MaterialComponent>& _materialComponents,
		const std::vector<LightComponent>& _lightComponents
	);
};

