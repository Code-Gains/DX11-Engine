#pragma once

class System {
public:
	virtual ~System() = default;

	virtual void Render() = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void PeriodicUpdate(float deltaTime) = 0;
};