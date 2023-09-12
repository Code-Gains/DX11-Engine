#pragma once
#include "Object3D.hpp"
class PhysicsBody : public Object3D
{
	float _mass;
	DirectX::XMFLOAT3 _velocity;
	DirectX::XMFLOAT3 _acceleration;

public:
	PhysicsBody(float mass, const DirectX::XMFLOAT3& velocity, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& rotation, const DirectX::XMFLOAT3& scale);

	void ApplyForce(const DirectX::XMFLOAT3& force);
	void SetAcceleration(const DirectX::XMFLOAT3& acceleration);
	void UpdatePhysics(float deltaTime);
	void SetVelocity(const DirectX::XMFLOAT3 velocity);
};

