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
	//std::cout <<  _acceleration.x << std::endl;
}

void PhysicsBody::SetAcceleration(const DirectX::XMFLOAT3& acceleration)
{
	_acceleration = acceleration;
	//std::cout <<  _acceleration.x << std::endl;
}

void PhysicsBody::UpdatePhysics(float deltaTime)
{
	//static float timeLeft = 1.0f / 60;
	//if(timeLeft < 0)
	//std::cout << _velocity.x << std::endl;
	_velocity.x += _acceleration.x;
	_velocity.y += _acceleration.y;
	_velocity.z += _acceleration.z;

	transform.position.x += _velocity.x * deltaTime;
	transform.position.y += _velocity.y * deltaTime;
	transform.position.z += _velocity.z * deltaTime;
}

void PhysicsBody::SetVelocity(const DirectX::XMFLOAT3 velocity)
{
	_velocity = velocity;
}
