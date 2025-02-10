#pragma once
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

#include "ECS.hpp";
#include "ProfilerComponent.hpp"

class ProfilerSystem : public System
{
	ECS* _ecs = nullptr;
	bool _enabled = false;

public:
	ProfilerSystem();
	ProfilerSystem(ECS* ecs);
	~ProfilerSystem();

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
CEREAL_REGISTER_TYPE(ProfilerSystem)
CEREAL_REGISTER_POLYMORPHIC_RELATION(System, ProfilerSystem)