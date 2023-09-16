#include "PlanetarySimulation.hpp"

PlanetarySimulation::PlanetarySimulation(const WRL::ComPtr<ID3D11Device>& device) : _device(device)
{
	Initialize(device.Get());
}

PlanetarySimulation::~PlanetarySimulation()
{

}

bool PlanetarySimulation::Initialize(ID3D11Device* device)
{
	// Set up instance renderer
	auto sphere = Sphere(DirectX::XMFLOAT3{ 0, 0, 0 }, DirectX::XMFLOAT3{ 0, 0, 0 }, DirectX::XMFLOAT3{ 30, 30, 30 });
	std::vector<VertexPositionNormalUv> vertices = sphere.GetVertices();
	std::vector<UINT> indices = sphere.GetIndices();

	_instanceRenderer.InitializeInstancePool(_device.Get(), 0, vertices, indices);
	_instanceRenderer.AddInstance(InstanceConstantBuffer(sphere.transform.GetWorldMatrix()), 0);
	_planets.push_back(sphere);

	auto rectangle = Rectangle3D(DirectX::XMFLOAT3{ 0, 30, 0 }, DirectX::XMFLOAT3{ 0, 0, 0 }, DirectX::XMFLOAT3{ 1, 1, 1 });
	vertices = rectangle.GetVertices();
	indices = rectangle.GetIndices();
	_instanceRenderer.InitializeInstancePool(_device.Get(), 1, vertices, indices);

	// Set up simulation

	float fixedDistance = 30.0f;
	int particleCount = 10000;
	DirectX::XMFLOAT3 center = sphere.transform.position;

	for (int particleIndex = 0; particleIndex < particleCount; particleIndex++)
	{
		DirectX::XMFLOAT3 randomPoint = GetRandomPointAtDistance(center, fixedDistance);

		//DirectX::XMFLOAT3 randomPoint(0, 0, 30);
		DirectX::XMFLOAT3 randomDirection = GetRandomDirectionAlongTheSurface(center, randomPoint);
		//std::cout << randomDirection.x << " " << randomDirection.y << " " << randomDirection.z << std::endl;
		//float velocityMagnitude = GetInitialVelocityMagnitude(1000000.0f, fixedDistance);
		//std::cout << velocityMagnitude << std::endl;
		float velocityMagnitude = 2.5f;
		rectangle = Rectangle3D(DirectX::XMFLOAT3{ randomPoint.x, randomPoint.y, randomPoint.z }, GetRandomRotation(), DirectX::XMFLOAT3{1, 1, 1});
		_instanceRenderer.AddInstance(InstanceConstantBuffer(rectangle.transform.GetWorldMatrix()), 1);
		DirectX::XMFLOAT3 initialVelocity { randomDirection.x * velocityMagnitude, randomDirection.y * velocityMagnitude, randomDirection.z * velocityMagnitude };
		//std::cout << initialVelocity.x << " " << initialVelocity.y << " " << initialVelocity.z << std::endl;
		rectangle.SetVelocity(initialVelocity);
		//rectangle.SetVelocity(DirectX::XMFLOAT3{ 0, 0, 0 });
		_bodies.push_back(rectangle);

	}

	// Sun
	// X amount of bodies
	// All at a fixed abount from the surface of the sphere in a random direction
	// Calculate and set the angular velocity based on the direction of travel
	// Done?

	/*_instanceRenderer.AddInstance(InstanceConstantBuffer(rectangle.transform.GetWorldMatrix()), 1);
	rectangle.SetVelocity(DirectX::XMFLOAT3{ 11.5f, 0.0f, 0.0f });
	_bodies.push_back(rectangle);*/
	return true;
}

void PlanetarySimulation::Update(float deltaTime)
{
	//static int skipsLeft = 1000;
	//if (skipsLeft > 0)
	//{
	//	//std::cout << deltaTime << std::endl;
	//	skipsLeft--;
	//	return;
	//}
	//std::cout << deltaTime<< std::endl;
	
	//exit(0);
	//std::cout << _bodies.size() << std::endl;

	// simulate gravity
		// loop through planets
		// calculate attraction between planet and other planets and change and change force
		// calculate attraction between [bodies and planet and other bodies and change force

}

void PlanetarySimulation::PeriodicUpdate(float deltaTime)
{
	//std::cout << "updating" << std::endl;
	int planetIndex = 0;
	for (const auto& planet : _planets)
	{
		int bodyIndex = 0;
		for (auto& body : _bodies)
		{
			float distance = GetDistance(planet.transform.position, body.transform.position);
			float forceMagnitude = GetForce(1000000.0f, 1000.0f, GetDistance(planet.transform.position, body.transform.position));
			DirectX::XMVECTOR forceDirection = GetDirection(planet.transform.position, body.transform.position);
			//std::cout << body.transform.position.x << " " << body.transform.position.y << " " << body.transform.position.z << std::endl;
			DirectX::XMFLOAT3 force;
			DirectX::XMStoreFloat3(&force, forceDirection);
			force.x = force.x * forceMagnitude;
			force.y = force.y * forceMagnitude;
			force.z = force.z * forceMagnitude;
			//std::cout << force.x << " " << force.y << " " << force.z << " " << std::endl;
			body.SetAcceleration(force);
			body.UpdatePhysics(deltaTime);
			body.Update(deltaTime);
			//std::cout << body.transform.position.x << std::endl;
			_instanceRenderer.UpdateInstanceData(1, bodyIndex, InstanceConstantBuffer(body.transform.GetWorldMatrix()));
			bodyIndex++;
		}
		planetIndex++;
	}
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

float PlanetarySimulation::GetInitialVelocityMagnitude(float mass, float distance) const
{
	// Calculate the gravitational acceleration at the given distance from the planet's center.
	float gravitationalAcceleration = Constants::G * mass / (distance * distance);

	// Calculate the initial velocity magnitude for a circular orbit at this distance.
	// This is based on the centripetal force formula: F = (mv^2) / r, where F is the gravitational force.
	float velocityMagnitude = sqrtf(gravitationalAcceleration * distance);

	return velocityMagnitude;
}

DirectX::XMFLOAT3 PlanetarySimulation::GetRandomPointAtDistance(const DirectX::XMFLOAT3& center, float distance) const
{
	DirectX::XMFLOAT3 randomDirection = GetRandomUnitVector();

	DirectX::XMFLOAT3 randomPoint(
		center.x + randomDirection.x * distance,
		center.y + randomDirection.y * distance,
		center.z + randomDirection.z * distance
	);

	return randomPoint;
}

DirectX::XMFLOAT3 PlanetarySimulation::GetRandomUnitVector() const
{
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);

	float x = distribution(mt);
	float y = distribution(mt);
	float z = distribution(mt);

	DirectX::XMFLOAT3 randomVector{ x, y, z };

	DirectX::XMVECTOR vec = DirectX::XMLoadFloat3(&randomVector);
	vec = DirectX::XMVector3Normalize(vec);
	DirectX::XMStoreFloat3(&randomVector, vec);

	return randomVector;
}

DirectX::XMFLOAT3 PlanetarySimulation::GetRandomDirectionAlongTheSurface(const DirectX::XMFLOAT3& center, const DirectX::XMFLOAT3& position) const {
	// Calculate the radius vector from center to position
	DirectX::XMFLOAT3 radiusVector;
	DirectX::XMStoreFloat3(&radiusVector, DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&position), DirectX::XMLoadFloat3(&center)));

	// Normalize the radius vector to get the unit normal vector
	DirectX::XMFLOAT3 unitNormal;
	DirectX::XMStoreFloat3(&unitNormal, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&radiusVector)));

	// Generate two random angles for spherical coordinates
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dist(0.0f, 1.0f);

	float theta = 2.0f * DirectX::XM_PI * dist(gen); // Random angle in radians (azimuthal angle)
	float phi = acosf(2.0f * dist(gen) - 1.0f);      // Random angle in radians (polar angle)

	// Calculate the random direction vector
	DirectX::XMFLOAT3 randomDirection;
	randomDirection.x = sinf(phi) * cosf(theta);
	randomDirection.y = sinf(phi) * sinf(theta);
	randomDirection.z = cosf(phi);

	// Ensure the direction is perpendicular to the radius vector by taking its cross product
	DirectX::XMFLOAT3 perpendicularDirection;
	DirectX::XMStoreFloat3(&perpendicularDirection, DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&radiusVector), DirectX::XMLoadFloat3(&randomDirection)));

	// Normalize the perpendicular direction
	DirectX::XMStoreFloat3(&perpendicularDirection, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&perpendicularDirection)));

	return perpendicularDirection;
}

DirectX::XMFLOAT3 PlanetarySimulation::GetRandomRotation() const
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dist(0.0f, DirectX::XM_2PI);

	float x = dist(gen);
	float y = dist(gen);
	float z = dist(gen);

	return DirectX::XMFLOAT3(x, y, z);
}


DirectX::XMVECTOR PlanetarySimulation::GetDirection(const DirectX::XMFLOAT3& p1, const DirectX::XMFLOAT3& p2) const
{
	DirectX::XMFLOAT3 distance3D = DirectX::XMFLOAT3{ p1.x - p2.x, p1.y - p2.y, p1.z - p2.z };
	DirectX::XMVECTOR direction = DirectX::XMLoadFloat3(&distance3D);
	return direction;
}

