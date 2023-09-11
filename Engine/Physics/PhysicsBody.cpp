#include "PhysicsBody.hpp"
#include "Transform.hpp"
#include <d3d11_2.h>

PhysicsBody::PhysicsBody(float mass, const DirectX::XMFLOAT3& velocity, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& rotation, const DirectX::XMFLOAT3& scale)
	: _mass(mass), _velocity(velocity), _acceleration( {0.0f, 0.0f, 0.0f} )
{
	transform.position = position;
	transform.rotation = rotation;
	transform.scale = scale;
}

void PhysicsBody::ApplyForce(const DirectX::XMFLOAT3& force)
{
	_acceleration.x += (force.x / _mass);
	_acceleration.y += (force.y / _mass);
	_acceleration.z += (force.z / _mass);
}

void PhysicsBody::Update(float deltaTime)
{
	transform.position.x += _velocity.x * deltaTime;
	transform.position.y += _velocity.y * deltaTime;
	transform.position.z += _velocity.z * deltaTime;
}
