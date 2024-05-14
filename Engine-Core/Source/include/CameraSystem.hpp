#include "CameraComponent.hpp"
#include "ECS.hpp";
#include "Constants.hpp"

class CameraSystem : ISystem
{
	ECS* _ecs;
public:
	CameraSystem();
	CameraSystem(ECS* ecs);
	~CameraSystem();

	void Render() override;
	void Update(float deltaTime) override;
	void PeriodicUpdate(float deltaTime) override;
};