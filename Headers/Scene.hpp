#pragma once
#include <memory>
#include <vector>
#include "Object3D.hpp"
#include "Logging.hpp"

class Scene : public IDebuggable
{
public:
	Scene();
	~Scene();


	void AddObject(std::unique_ptr<Object3D> object);
	void RemoveObject(Object3D* object);

	const std::vector<std::unique_ptr<Object3D>>& GetObjects() const;
	const std::vector<Object3D*> GetAllObjects() const;

	void Update(float _deltaTime);
	void Render(ID3D11DeviceContext* deviceContext, ID3D11Buffer* perObjectConstantBuffer, ID3D11Buffer* instanceConstantBuffer);
	int GetOwnershipCount() const override;

private:
	std::vector<std::unique_ptr<Object3D>> _objects;
};

