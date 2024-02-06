#pragma once
class IDebuggable
{
public:
    virtual int GetOwnershipCount() const = 0;
};