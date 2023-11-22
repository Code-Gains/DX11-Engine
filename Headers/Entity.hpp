#pragma once
class Entity
{
	int _id = 0;
public:
	Entity();
	Entity(int id);
	int GetId() const;

	template <typename Archive>
	void serialize(Archive& archive)
	{
		archive(_id);
	}
};

