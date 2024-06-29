#pragma once

class IMonitorable
{
public:
	virtual void RenderMonitorUI() const = 0;
	virtual ~IMonitorable() = default;
};