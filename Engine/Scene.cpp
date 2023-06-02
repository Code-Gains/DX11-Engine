#include <Scene.hpp>

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

void Scene::Update(float _deltaTime)
{
	for (auto& object : _objects)
	{
		object->Update();
	}
}

void Scene::Render(ID3D11DeviceContext* deviceContext)
{
	for (auto& object : _objects)
	{
		object->Render(deviceContext);
	}
}