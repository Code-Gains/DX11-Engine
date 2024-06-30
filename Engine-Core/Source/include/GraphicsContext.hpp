#pragma once

class IGraphicsContext
{
public:
    virtual ~IGraphicsContext() = default;

    virtual bool Initialize() = 0;
    virtual void Cleanup() = 0;
    virtual void Render() = 0;
};