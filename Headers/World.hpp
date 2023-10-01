#pragma once
//#include <unordered_map>
//#include <unordered_set>
//#include <memory>
//
////#include "Entity.hpp"
////#include "Component.hpp"
////
//#include "TransformComponent.hpp"
//#include "MaterialComponent.hpp"
//#include "LightComponent.hpp"
//#include "CameraComponent.hpp"
//#include "MeshComponent.hpp"
//
//#include "RenderingSystem.hpp"
//#include "MeshComponent.hpp"
//#include "MaterialComponent.hpp"
//#include "LightComponent.hpp"
//#include "CameraComponent.hpp"

/*
* Since it is not possible to determine what types would be contained in vectors at compile time
* we have to manage the data ourselves, essentially risking accessing memory out of bounds :)
*/
//class ComponentStorage
//{
//	// Store data without determining the type at compile time
//	// once needed perform a static cast to allow access for methods
//	// there is info that static casts might be optimized entirely by the compiler
//	std::shared_ptr<void> _elements;
//	size_t _componentSize;
//	size_t _componentCount;
//
//public:
//	ComponentStorage();
//	ComponentStorage(size_t componentSize, size_t componentCount);
//
//	std::shared_ptr<void> GetElements() const;
//	size_t GetComponentSize() const;
//	size_t GetComponentCount() const;
//};


//class ComponentStorageShelf
//{
//	ComponentSet _componentSet;
//	size_t _shelf;
//public:
//	ComponentStorageShelf() {};
//	ComponentStorageShelf(ComponentSet componentSet) : _componentSet(componentSet) {};
//};
//
///*
//* Should be a singleton class responsible for all entity/component management?
//*/
//class World
//{
//	// Relations
//	std::unordered_map<int, ComponentStorageShelf> _entities; // entities are sets of components
//	std::unordered_map<int, std::vector<int>> _components; // what sets components belong to
//	std::unordered_set<int> _uniqueComponentSets;
//
//	//Storage
//
//	// we will want to use a structure that is continuous in memory and can be sorted easily
//	/*template<typename ComponentType>
//	std::unordered_map<int, std::vector<ComponentType>> _components;*/
//	// Can't determine what will be inside of the vectors so can't compile
//
//	// This is going to be a nightmare i can feel it
//	//std::unordered_map<int, ComponentStorage> _componentData; 
//	std::vector<ComponentSet> _componentSetData; // all current component set variations
//
//	// Indexing
//	int _nextId = 1;
//public:
//	Entity CreateEntity(const std::vector<int>& componentSet);
//	int DeleteEntity(int id);
//	bool HasEntityComponent(int entityId, int componentId) const;
//	void UpdateEntities(float deltaTime);
//
//	size_t GetEntityCount() const;
//};


#include <unordered_map>
#include <unordered_set>
#include <memory>

#include <d3d11_2.h>
#include <DirectXMath.h>

#include "Entity.hpp"

#include "TransformComponent.hpp"
#include "MaterialComponent.hpp"
#include "LightComponent.hpp"
#include "CameraComponent.hpp"
#include "MeshComponent.hpp"
#include "RenderingSystem.hpp"
#include "InstanceRenderer.hpp"

#include "Cube.hpp"
#include "VertexType.hpp"

#include "MeshComponent.hpp"
#include "Constants.hpp"

//struct EntityGroup {
//	std::vector<>
//};

class World
{
	// Application State
	HWND _win32Window;

	int32_t _viewportWidth;
	int32_t _viewportHeight;

	// Entities
	std::vector<Entity> _entities;
	int _nextEntityId = 1;

	// Storage
	std::unordered_map<int, InstanceRenderer::InstancePool> _instancePools;

	//std::vector<WRL::ComPtr<ID3D11Buffer>> _vertexBuffers;
	//std::vector<WRL::ComPtr<ID3D11Buffer>> _indexBuffers;
	//std::vector<std::vector<InstanceConstantBuffer>> _instances;

	//UINT vertexCount = 0;
	//WRL::ComPtr<ID3D11Buffer> indexBuffer = nullptr;
	//UINT indexCount = 0;
	//std::vector<InstanceConstantBuffer> instances;
	//UINT instanceCount = 0;

	// Storage -> Component Data
	std::vector<TransformComponent> _transformComponents;
	std::vector<MeshComponent> _meshComponents;
	std::vector<MaterialComponent> _materialComponents;
	std::vector<LightComponent> _lightComponents;
	std::vector<CameraComponent> _cameraComponents;

	// Storage -> Component Relations
	// entityId -> componentIndex
	std::unordered_map<int, int> _transformComponentIndices;
	std::unordered_map<int, int> _meshComponentIndices;
	std::unordered_map<int, int> _materialComponentIndices;
	std::unordered_map<int, int> _lightComponentIndices;
	std::unordered_map<int, int> _cameraComponentIndices;

	// Systems
	InstanceRenderer _instanceRenderer;


	// Rendering Data
	LightConstantBuffer _lightConstantBufferData;
	PerFrameConstantBuffer _perFrameConstantBufferData{};
	CameraConstantBuffer _cameraConstantBufferData{};

public:
	World();

	bool Initialize(HWND win32Window, ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	void UpdateViewportDimensions(int32_t width, int32_t height);
	bool LoadWorld(std::string fileName = "");

	void Update(float deltaTime);
	void PeriodicUpdate(float deltaTime);
	void Render();

	void AddRenderableInstanceInstance(const InstanceConstantBuffer& instanceData, int poolKey);
	
	std::vector<int> GetRenderableEntities(
		const std::unordered_map<int, int>& transformIndices,
		const std::unordered_map<int, int>& meshIndices,
		const std::unordered_map<int, int>& materialIndices
	) const;

	Entity CreateEntity();

	// Temporary templating

	void AddComponent(int entityId, const TransformComponent& component);
	void AddComponent(int entityId, const MeshComponent& component);
	void AddComponent(int entityId, const MaterialComponent& component);
	void AddComponent(int entityId, const LightComponent& component);
	void AddComponent(int entityId, const CameraComponent& component);
};


//template<>
//void World::AddComponent<TransformComponent>(int entityId, const TransformComponent& component)
//{
//	_transformComponents[entityId] = component;
//}
//
//template<>
//void World::AddComponent<MeshComponent>(int entityId, const MeshComponent& component)
//{
//	_meshComponents[entityId] = component;
//}
//
//template<>
//void World::AddComponent<MaterialComponent>(int entityId, const MaterialComponent& component)
//{
//	_materialComponents[entityId] = component;
//}
//
//template<>
//void World::AddComponent<LightComponent>(int entityId, const LightComponent& component)
//{
//	_lightComponents[entityId] = component;
//}
//
//template<>
//void World::AddComponent<CameraComponent>(int entityId, const CameraComponent& component)
//{
//	_cameraComponents[entityId] = component;
//}

//template<typename TComponent>
//inline void World::AddComponent(int entityId, const TComponent& component)
//{
//	return; // probably useless?
//}
