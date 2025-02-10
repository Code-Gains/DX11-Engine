#pragma once

class GraphicsContext
{
public:
    virtual ~GraphicsContext() = default;

    virtual bool Initialize() = 0;
    virtual void Cleanup() = 0;
    virtual void Render() = 0;
};