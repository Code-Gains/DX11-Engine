#pragma once
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

#include "ECS.hpp";
#include "Constants.hpp"
#include "RenderingApplication3D.hpp"

class EntityMonitoringSystem : public System
{
	ECS* _ecs = nullptr;
	bool _enabled = false;
public:
	EntityMonitoringSystem();
	EntityMonitoringSystem(ECS* ecs);
	~EntityMonitoringSystem();

	void Render() override;
	void Update(float deltaTime) override;
	void PeriodicUpdate(float deltaTime) override;

	void Toggle();

	template<typename Archive>
	void serialize(Archive& archive)
	{

	}
};

#include <cereal/types/polymorphic.hpp>
CEREAL_REGISTER_TYPE(EntityMonitoringSystem)
CEREAL_REGISTER_POLYMORPHIC_RELATION(System, EntityMonitoringSystem)