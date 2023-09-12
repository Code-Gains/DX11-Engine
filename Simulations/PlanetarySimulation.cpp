#include "PlanetarySimulation.hpp"

PlanetarySimulation::PlanetarySimulation(const WRL::ComPtr<ID3D11Device>& device) : _device(device)
{
    auto sphere = Sphere(DirectX::XMFLOAT3 {0, 0, 0}, DirectX::XMFLOAT3{ 0, 0, 0 }, DirectX::XMFLOAT3{ 3, 3, 3 });
	std::vector<VertexPositionNormalUv> vertices = sphere.GetVertices();
	std::vector<UINT> indices = sphere.GetIndices();

    _instanceRenderer.InitializeInstancePool(_device.Get(), 0, vertices, indices);
	_instanceRenderer.AddInstance(InstanceConstantBuffer(sphere.transform.GetWorldMatrix()), 0);
	_planets.push_back(sphere);

    auto rectangle = Rectangle3D(DirectX::XMFLOAT3{ 0, 4, 0 }, DirectX::XMFLOAT3{ 0, 0, 0 }, DirectX::XMFLOAT3{ 1, 1, 1 });
    vertices = rectangle.GetVertices();
    indices = rectangle.GetIndices();
    _instanceRenderer.InitializeInstancePool(_device.Get(), 1, vertices, indices);
    _instanceRenderer.AddInstance(InstanceConstantBuffer(rectangle.transform.GetWorldMatrix()), 1);
	rectangle.SetVelocity(DirectX::XMFLOAT3{ 4.5f, 0.0f, 0.0f });
	_bodies.push_back(rectangle);
}

PlanetarySimulation::~PlanetarySimulation()
{
}

bool PlanetarySimulation::Initialize(ID3D11Device* device)
{
	// 1 planet
	// 100k bodies
	return true;
}

void PlanetarySimulation::Update(float deltaTime)
{
	//std::cout << _bodies.size() << std::endl;
	/*float _rotationDelta = deltaTime * _bodyRotationSpeed;
	for (auto body : _bodies)
	{
		body.transform.rotation.x += _rotationDelta;
		body.transform.rotation.y += _rotationDelta;
		body.transform.rotation.z += _rotationDelta;
	}*/

	// simulate gravity
	int planetIndex = 0;
	for (const auto& planet : _planets)
	{
		int bodyIndex = 0;
		for (auto& body : _bodies)
		{
			float distance = GetDistance(planet.transform.position, body.transform.position);
			float forceMagnitude = GetForce(100000.0f, 1000.0f, GetDistance(planet.transform.position, body.transform.position));
			DirectX::XMVECTOR forceDirection = GetDirection(planet.transform.position, body.transform.position);

			DirectX::XMFLOAT3 force;
			DirectX::XMStoreFloat3(&force, forceDirection);
			force.x = force.x * forceMagnitude;
			force.y = force.y * forceMagnitude;
			force.z = force.z * forceMagnitude;
			std::cout << force.x << " " << force.y << " " << force.z << " " << std::endl;
			body.SetAcceleration(force);
			body.UpdatePhysics(deltaTime);
			//std::cout << body.transform.position.x << std::endl;
			_instanceRenderer.UpdateInstanceData(1, bodyIndex, InstanceConstantBuffer(body.transform.GetWorldMatrix()));
			bodyIndex++;
		}
		planetIndex++;
	}
		// loop through planets
		// calculate attraction between planet and other planets and change and change force
		// calculate attraction between [bodies and planet and other bodies and change force

}

void PlanetarySimulation::Render(ID3D11DeviceContext* deviceContext, ID3D11Buffer* perObjectConstantBuffer, ID3D11Buffer* instanceConstantBuffer)
{
	_instanceRenderer.RenderInstances<VertexPositionNormalUv>(deviceContext, perObjectConstantBuffer, instanceConstantBuffer);
}

int PlanetarySimulation::GetOwnershipCount() const
{
	return _planets.size() + _bodies.size();
}

float PlanetarySimulation::GetForce(float m1, float m2, float distance) const
{
	//F = GMm/R²
	return (Constants::G * m1 * m2) / (distance * distance);
}

float PlanetarySimulation::GetDistance(const DirectX::XMFLOAT3& p1, const DirectX::XMFLOAT3& p2) const
{
	return sqrt(pow((p2.x - p1.x), 2) + pow((p2.y - p1.y), 2) + pow((p2.z - p1.z), 2));
}

DirectX::XMVECTOR PlanetarySimulation::GetDirection(const DirectX::XMFLOAT3& p1, const DirectX::XMFLOAT3& p2) const
{
	DirectX::XMFLOAT3 distance3D = DirectX::XMFLOAT3{ p1.x - p2.x, p1.y - p2.y, p1.z - p2.z };
	DirectX::XMVECTOR direction = DirectX::XMLoadFloat3(&distance3D);
	return direction;
}

