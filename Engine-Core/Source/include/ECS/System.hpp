#pragma once
#include <cereal/cereal.hpp>

class System {
public:
	virtual ~System() = default;

	virtual void Render() = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void PeriodicUpdate(float deltaTime) = 0;

	template <class Archive>
	void serialize(Archive& archive) {}
};

#include <cereal/types/polymorphic.hpp>
CEREAL_REGISTER_TYPE(System)