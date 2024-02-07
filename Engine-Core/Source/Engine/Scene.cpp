#include "Scene.hpp"
#include <iostream>

Scene::Scene() {}


Scene::~Scene() {}

void Scene::AddObject(std::unique_ptr<Object3D> object)
{
	_objects.push_back(std::move(object));
}
void Scene::RemoveObject(Object3D* object)
{
	_objects.erase(
		std::remove_if(
			_objects.begin(),
			_objects.end(),
			[object](const std::unique_ptr<Object3D>& unique_ptr)
			{
				return unique_ptr.get() == object;
			}),
		_objects.end()
	);
}

const std::vector<std::unique_ptr<Object3D>>& Scene::GetObjects() const
{
	return _objects;
}

//const std::vector<Object3D*> Scene::GetAllObjects() const {
//	std::vector<Object3D*> allObjects;
//
//	for (const auto& object : _objects) {
//		auto objectObjects = object->GetAllObjects();
//		allObjects.insert(allObjects.end(), objectObjects.begin(), objectObjects.end());
//	}
//
//	return allObjects;
//}

int Scene::GetOwnershipCount() const
{
	int totalCount = 0;
	for (auto& object : _objects)
	{
		totalCount += object->GetOwnershipCount();
	}
	return totalCount;
}

void Scene::Update(float _deltaTime)
{
	for (auto& object : _objects)
	{
		object->Update(_deltaTime);
	}
}

void Scene::PeriodicUpdate(float _deltaTime)
{
	for (auto& object : _objects)
	{
		object->PeriodicUpdate(_deltaTime);
	}
}

void Scene::Render(ID3D11DeviceContext* deviceContext, ID3D11Buffer* perObjectConstantBuffer, ID3D11Buffer* instanceConstantBuffer)
{
	for (auto& object : _objects)
	{
		object->Render(deviceContext, perObjectConstantBuffer, instanceConstantBuffer);
	}
}