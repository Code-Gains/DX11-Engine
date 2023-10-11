#pragma once
#include "Definitions.hpp"
#include <vector>
#include "InstanceRenderer.hpp"
#include "Sphere.hpp"
#include "Rectangle3D.hpp"
#include <d3d11_2.h>
#include "Constants.hpp"
#include "cmath"
#include <DirectXMath.h>
#include <random>

//#include <IntVectors.hpp>
//#include <memory>
//#include <iostream>
//#include <random>
//#include <memory>
//#include "Constants.hpp"
//#include "Sphere.hpp"
//#include "Cylinder.hpp"
//#include "Cube.hpp"
//#include "Object3D.hpp"
//#include "InstanceRenderer.hpp"

class PlanetarySimulation : public Object3D
{
	WRL::ComPtr<ID3D11Device> _device = nullptr;
    InstanceRenderer _instanceRenderer;
	std::vector<Sphere> _planets;
	std::vector<Rectangle3D> _particles;

    float _sunRadius;
    float _particleRingRadius;
    int _particleCount = 1000;
    float _sunMass = 1000000.0f;
    float _particleMass = 100000.0f;
    float _bodyRotationSpeed = 1.0f;


public:
    PlanetarySimulation(ID3D11Device* device, ID3D11DeviceContext* deviceContext, float sunRadius, float particleRingRadius, int particleCount);
    virtual ~PlanetarySimulation();

    bool Initialize(ID3D11Device* device) override;
    bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
    void Update(float deltaTime) override;
    void PeriodicUpdate(float deltaTime) override;
    void Render(ID3D11DeviceContext* deviceContext, ID3D11Buffer* perObjectConstantBuffer, ID3D11Buffer* instanceConstantBuffer) override;
    int GetOwnershipCount() const override;

    void UpdateVelocities(float deltaTime);
    void UpdateTransformations(float deltaTime);

    float GetForce(float m1, float m2, float distance) const;
    float GetDistance(const DirectX::XMFLOAT3& p1, const DirectX::XMFLOAT3& p2) const;
    float GetInitialVelocityMagnitude(float mass, float distance) const;
    DirectX::XMFLOAT3 GetRandomPointAtDistance(const DirectX::XMFLOAT3& center, float distance) const;
    DirectX::XMFLOAT3 GetRandomUnitVector() const;
    DirectX::XMFLOAT3 GetRandomDirectionAlongTheSurface(const DirectX::XMFLOAT3& center, const DirectX::XMFLOAT3& position) const;
    DirectX::XMFLOAT3 GetRandomRotation() const;
    DirectX::XMVECTOR GetDirection(const DirectX::XMFLOAT3& p1, const DirectX::XMFLOAT3& p2) const;

};