#pragma once
#include <memory>
#include <Object3D.hpp>
#include <vector>

class Scene
{
public:
	Scene();
	~Scene();


	void AddObject(std::unique_ptr<Object3D> object);
	void RemoveObject(Object3D* object);

	const std::vector<std::unique_ptr<Object3D>>& GetObjects() const;

	void Update(float _deltaTime);
	void Render(ID3D11DeviceContext* deviceContext);

private:
	std::vector<std::unique_ptr<Object3D>> _objects;
};

