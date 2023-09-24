#include "World.hpp"
#include <MeshComponent.hpp>

//Entity World::CreateEntity(const std::vector<int>& componentSet)
//{
//	int entityId = _nextId;
//	auto entity = Entity(entityId);
//	_entities[entityId] = ComponentStorageShelf(ComponentSet(componentSet));
//
//	_nextId++;
//	return entity;
//}
//
//int World::DeleteEntity(int id)
//{
//	_entities.erase(id);
//	return id;
//}
//
//bool World::HasEntityComponent(int entityId, int componentId) const
//{
//	auto entityIterator = _entities.find(entityId);
//
//	if (entityIterator == _entities.end())
//		return false;
//
//	int entityComponentSetId = entityIterator->first;
//	return _uniqueComponentSets.count(entityComponentSetId) != 0;
//}
//
//void World::UpdateEntities(float deltaTime)
//{
//}
//
//size_t World::GetEntityCount() const
//{
//	return _entities.size();
//}
//
//ComponentStorage::ComponentStorage()
//{
//}
//
//ComponentStorage::ComponentStorage(size_t componentSize, size_t componentCount) : _componentSize(componentSize), _componentCount(componentCount)
//{
//}
//
//std::shared_ptr<void> ComponentStorage::GetElements() const
//{
//	return _elements;
//}
//
//size_t ComponentStorage::GetComponentSize() const
//{
//	return _componentSize;
//}
//
//size_t ComponentStorage::GetComponentCount() const
//{
//	return _componentCount;
//}

World::World()
{
}

bool World::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	_instanceRenderer = InstanceRenderer(device, deviceContext);
	return true;
}

void World::Update(float deltaTime)
{
	_renderingSystem.Update(deltaTime);
}

void World::PeriodicUpdate(float deltaTime)
{
}

void World::Render(ID3D11DeviceContext* deviceContext, ID3D11Buffer* perObjectConstantBuffer, ID3D11Buffer* instanceConstantBuffer)
{
	//_renderingSystem.Render(deviceContext, perObjectConstantBuffer, instanceConstantBuffer,
		//_transformComponents, _meshComponents, _materialComponents, _lightComponents);
	_instanceRenderer.RenderInstances<VertexPositionNormalUv>();
}

bool World::LoadWorld(std::string fileName)
{
	if(fileName != "")
	{
	}

	// Create a few meshes, materials, a light source

	auto entity = Entity();

	auto transformComponent = TransformComponent();
	//transformComponent.AddEntity(entity);
	_transformComponents.push_back(transformComponent);

	auto cube = Cube();
	auto meshComponent = MeshComponent(cube.GetVertices(), cube.GetIndices());
	//meshComponent.AddEntity(entity);
	_meshComponents.push_back(meshComponent);

	DirectX::XMFLOAT4 ambient{ 1.0f, 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT4 diffuse{ 1.0f, 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT4 specular{ 1.0f, 1.0f, 1.0f, 1.0f };
	float shininess = 3;


	auto materialComponent = MaterialComponent(ambient, diffuse, specular, shininess);
	//materialComponent.AddEntity(entity);
	_materialComponents.push_back(materialComponent);



	// Set up instance renderer
	std::vector<VertexPositionNormalUv> vertices = cube.GetVertices();
	std::vector<UINT> indices = cube.GetIndices();

	_instanceRenderer.InitializeInstancePool(0, vertices, indices);
	_instanceRenderer.AddInstance(InstanceConstantBuffer(cube.transform.GetWorldMatrix()), 0);

	std::cout << entity.GetId() << std::endl;

	return false;
}
