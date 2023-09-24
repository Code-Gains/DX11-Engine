#include "Entity.hpp"

Entity::Entity()
{
	static int _nextId = 1;
	_id = _nextId++;
}

int Entity::GetId() const
{
	return _id;
}
