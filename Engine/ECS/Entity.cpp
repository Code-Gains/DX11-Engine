#include "Entity.hpp"

Entity::Entity()
{
}

Entity::Entity(int id): _id(id)
{
}

int Entity::GetId() const
{
	return _id;
}
