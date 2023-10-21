#pragma once
#include <string>
class ISerializeable
{
public:
	virtual std::string Serialize() = 0;
};

