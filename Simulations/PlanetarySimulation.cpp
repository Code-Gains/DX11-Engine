#include "PlanetarySimulation.hpp"


PlanetarySimulation::PlanetarySimulation(
	ID3D11Device* device,
	ID3D11DeviceContext* deviceContext,
	float sunRadius,
	float particleRingRadius,
	int particleCount) :
		_device(device),
		_sunRadius(sunRadius),
		_particleRingRadius(particleRingRadius),
		_particleCount(particleCount)
{
	Initialize(device, deviceContext);
}

PlanetarySimulation::~PlanetarySimulation()
{
}

bool PlanetarySimulation::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{

	// Set up instance renderer
	_instanceRenderer = InstanceRenderer(device, deviceContext);

	auto sphere = Sphere(DirectX::XMFLOAT3{ 0, 0, 0 }, DirectX::XMFLOAT3{ 0, 0, 0 }, DirectX::XMFLOAT3(_sunRadius, _sunRadius, _sunRadius));
	std::vector<VertexPositionNormalUv> vertices = sphere.GetVertices();
	std::vector<UINT> indices = sphere.GetIndices();

	_instanceRenderer.InitializeInstancePool(0, vertices, indices);
	_instanceRenderer.AddInstance(0, 0, InstanceConstantBuffer(sphere.transform.GetWorldMatrix()));
	sphere.SetVelocity(DirectX::XMFLOAT3{ -40, 0, 0 });
	_planets.push_back(sphere);

	auto sphere2 = Sphere(DirectX::XMFLOAT3{ 0, 60, 0 }, DirectX::XMFLOAT3{ 0, 0, 0 }, DirectX::XMFLOAT3(_sunRadius, _sunRadius, _sunRadius));
	_instanceRenderer.AddInstance(0, 0, InstanceConstantBuffer(sphere2.transform.GetWorldMatrix()));
	sphere2.SetVelocity(DirectX::XMFLOAT3{ 40, 0, 0 });
	_planets.push_back(sphere2);

	auto sphere3 = Sphere(DirectX::XMFLOAT3{ 0, -120, 0 }, DirectX::XMFLOAT3{ 0, 0, 0 }, DirectX::XMFLOAT3(_sunRadius, _sunRadius, _sunRadius));
	_instanceRenderer.AddInstance(0, 0, InstanceConstantBuffer(sphere3.transform.GetWorldMatrix()));
	sphere3.SetVelocity(DirectX::XMFLOAT3{ 20, 0, 0 });
	_planets.push_back(sphere3);

	auto rectangle = Rectangle3D(DirectX::XMFLOAT3{ 0, 0, 0 }, DirectX::XMFLOAT3{ 0, 0, 0 }, DirectX::XMFLOAT3{ 1, 1, 1 });
	vertices = rectangle.GetVertices();
	indices = rectangle.GetIndices();
	_instanceRenderer.InitializeInstancePool(1, vertices, indices);

	// Set up simulation
	DirectX::XMFLOAT3 center = sphere.transform.position;

	for (int particleIndex = 0; particleIndex < _particleCount; particleIndex++)
	{
		DirectX::XMFLOAT3 randomPoint = GetRandomPointAtDistance(center, _particleRingRadius);
		DirectX::XMFLOAT3 randomDirection = GetRandomDirectionAlongTheSurface(center, randomPoint);
		//float velocityMagnitude = GetInitialVelocityMagnitude(1000000.0f, fixedDistance);
		float velocityMagnitude = 20.0f;

		rectangle = Rectangle3D(DirectX::XMFLOAT3(randomPoint.x, randomPoint.y, randomPoint.z), GetRandomRotation(), DirectX::XMFLOAT3{ 1, 1, 1 });
		_instanceRenderer.AddInstance(1, 0, InstanceConstantBuffer(rectangle.transform.GetWorldMatrix()));
		DirectX::XMFLOAT3 initialVelocity { randomDirection.x * velocityMagnitude, randomDirection.y * velocityMagnitude, randomDirection.z * velocityMagnitude };
		rectangle.SetVelocity(initialVelocity);

		_particles.push_back(rectangle);
	}

	return true;
}

bool PlanetarySimulation::Initialize(ID3D11Device* device)
{
	return false;
}

void PlanetarySimulation::Update(float deltaTime)
{
}

void PlanetarySimulation::PeriodicUpdate(float deltaTime)
{
	UpdateVelocities(deltaTime);
	UpdateTransformations(deltaTime);
}

void PlanetarySimulation::UpdateVelocities(float deltaTime)
{
	int planetIndex = 0;
	for (auto& planet : _planets)
	{
		for (int otherPlanetIndex = planetIndex + 1; otherPlanetIndex < _planets.size(); otherPlanetIndex++)
		{
			float distance = GetDistance(planet.transform.position, _planets[otherPlanetIndex].transform.position);
			float forceMagnitude = GetForce(_sunMass, _sunMass, GetDistance(planet.transform.position, _planets[otherPlanetIndex].transform.position));
			DirectX::XMVECTOR forceDirection = GetDirection(planet.transform.position, _planets[otherPlanetIndex].transform.position);
			DirectX::XMFLOAT3 velocity1;
			DirectX::XMFLOAT3 velocity2;
			DirectX::XMStoreFloat3(&velocity1, DirectX::XMVectorNegate(forceDirection));
			DirectX::XMStoreFloat3(&velocity2, forceDirection);

			velocity1.x *= forceMagnitude;
			velocity1.y *= forceMagnitude;
			velocity1.z *= forceMagnitude;

			velocity2.x *= forceMagnitude;
			velocity2.y *= forceMagnitude;
			velocity2.z *= forceMagnitude;

			//std::cout << velocity.x << std::endl;
			planet.AddVelocity(velocity1);
			_planets[otherPlanetIndex].AddVelocity(velocity2);
		}

		int particleIndex = 0;
		for (auto& particle : _particles)
		{
			float distance = GetDistance(planet.transform.position, particle.transform.position);
			float forceMagnitude = GetForce(_sunMass, _particleMass, GetDistance(planet.transform.position, particle.transform.position));
			DirectX::XMVECTOR forceDirection = GetDirection(planet.transform.position, particle.transform.position);
			DirectX::XMFLOAT3 velocity;
			DirectX::XMStoreFloat3(&velocity, forceDirection);

			velocity.x *= forceMagnitude;
			velocity.y *= forceMagnitude;
			velocity.z *= forceMagnitude;
			particle.AddVelocity(velocity);

			particleIndex++;
		}
		planetIndex++;
	}
}

void PlanetarySimulation::UpdateTransformations(float deltaTime)
{
	int planetIndex = 0;
	for (auto& planet : _planets)
	{
		planet.UpdatePhysics(deltaTime);
		planet.Update(deltaTime);

		_instanceRenderer.UpdateInstanceData(0, planetIndex, InstanceConstantBuffer(planet.transform.GetWorldMatrix()));

		int particleIndex = 0;
		for (auto& particle : _particles)
		{
			particle.UpdatePhysics(deltaTime);
			particle.Update(deltaTime);

			_instanceRenderer.UpdateInstanceData(1, particleIndex, InstanceConstantBuffer(particle.transform.GetWorldMatrix()));

			particleIndex++;
		}
		planetIndex++;
	}
}

void PlanetarySimulation::Render(ID3D11DeviceContext* deviceContext, ID3D11Buffer* perObjectConstantBuffer, ID3D11Buffer* instanceConstantBuffer)
{
	auto removeThisLater = std::unordered_map<int, InstanceRenderer::InstancePool>();
	_instanceRenderer.RenderInstances<VertexPositionNormalUv>(removeThisLater, PerFrameConstantBuffer(), CameraConstantBuffer(), LightConstantBuffer(), MaterialConstantBuffer());
}

int PlanetarySimulation::GetOwnershipCount() const
{
	return _planets.size() + _particles.size();
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
	// likely wrong
	float gravitationalAcceleration = Constants::G * mass / (distance * distance);
	float velocityMagnitude = sqrtf(gravitationalAcceleration * distance);

	return velocityMagnitude;
}

DirectX::XMFLOAT3 PlanetarySimulation::GetRandomPointAtDistance(const DirectX::XMFLOAT3& center, float distance) const
{
	DirectX::XMFLOAT3 randomDirection = GetRandomUnitVector();

	DirectX::XMFLOAT3 randomPoint {
		center.x + randomDirection.x * distance,
		center.y + randomDirection.y * distance,
		center.z + randomDirection.z * distance
	};

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

DirectX::XMFLOAT3 PlanetarySimulation::GetRandomDirectionAlongTheSurface(const DirectX::XMFLOAT3& center, const DirectX::XMFLOAT3& position) const
{
	// I dont know how this works, but I plan to find out
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

	return DirectX::XMFLOAT3{ x, y, z };
}

DirectX::XMVECTOR PlanetarySimulation::GetDirection(const DirectX::XMFLOAT3& p1, const DirectX::XMFLOAT3& p2) const
{
	DirectX::XMFLOAT3 distance3D = DirectX::XMFLOAT3{ p1.x - p2.x, p1.y - p2.y, p1.z - p2.z };
	DirectX::XMVECTOR direction = DirectX::XMLoadFloat3(&distance3D);
	return direction;
}

