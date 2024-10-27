#include "EntityMonitoringSystem.hpp"

EntityMonitoringSystem::EntityMonitoringSystem()
{
}

EntityMonitoringSystem::EntityMonitoringSystem(ECS* ecs) : _ecs(ecs)
{
}

EntityMonitoringSystem::~EntityMonitoringSystem()
{
}

void EntityMonitoringSystem::Render()
{
	if (!_enabled)
		return;

	auto entityToArchetype = _ecs->GetAllEntities();
	ImGui::Begin("EMS");
	for (const auto& [entity, signature] : entityToArchetype)
	{
		ImGui::Text("E %d", entity);
		auto archetype = _ecs->GetEntityArchetype(entity);

		//auto x = archetype->GetComponent();
		//ImGui::BulletText("%s", ComponentRegistry::GetComponentName())
	}

	ImGui::End();
}

void EntityMonitoringSystem::Update(float deltaTime)
{
}

void EntityMonitoringSystem::PeriodicUpdate(float deltaTime)
{
}

void EntityMonitoringSystem::Toggle()
{
	_enabled = !_enabled;
}
