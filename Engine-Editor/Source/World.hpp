#pragma once
#include <unordered_map>
#include <unordered_set>
#include <memory>

#include <d3d11_2.h>
#include <DirectXMath.h>

#include <cereal/cereal.hpp>
#include <cereal/types/unordered_map.hpp>

#include "Entity.hpp"

#include "TransformComponent.hpp"
#include "MaterialComponent.hpp"
#include "LightComponent.hpp"
#include "CameraComponent.hpp"
#include "MeshComponent.hpp"

#include "InstanceRendererSystem.hpp"

#include "VertexType.hpp"
#include "Constants.hpp"
#include "WorldHierarchy.hpp"
#include "RenderingApplication3D.hpp"

class Universe; // forward declaration

class World
{
	// Application State
	HWND _win32Window;

	int32_t _viewportWidth;
	int32_t _viewportHeight;

	RenderingApplication3D* _renderingApplication;
	Universe* _universe;

	//ECS _ecs;

	//std::vector<Archetype> _archetypes;

	// thread pool
	// system -> update ->>> system requests world for a thread to do job on archetype combo

	// Ok so we need to change this into archetypes first :)
	// Base IComponentVector class (virtual - add, remove, link, do other stuff)
	// ComponentVector Container class - implement interfaces
	// class ComponentVector {vector<T> add remove link with entity};
	// archetype1 <transform, mesh, material>
	// archetype2 <transform, mesh, material, physics>

	// foreach archetype {get all transforms -> System} 

	// entity -> some archetype
	// class Archetype 
	// {
	//	    std::unordered_map<std::type_index -> ComponentVector*>
	// };
	// 
	// std::unordered_map<int -> Archetype> 
	// 
	// query by Archetype by int
	// 
	// Archetype contains a map of type-index -> pointer to component arrays
	// Archetypes each have their own arrays, and if we want to querry specific components, we iterate through all archetypes
	// in the system. Since there usually would not be many combinations this theoretically should be extremely fast
	// and all the memory would still be contiguous
	// When creating entities we would first identify what archetype we want to use, if it exists, then cool



	// Registry <template> to register components in std::unordered_map<componentTypeIndex -> IComponentContainer*>
	// Archetype manager (when creating entities with components manage archetypes, create new ones if that combo does not exist)
	// 

	// Systems

	// UI
	WorldHierarchy _worldHierarchy;

public:
	// World loading and application management
	World();
	~World();

	bool Initialize(RenderingApplication3D* renderingApplication, Universe* universe, HWND win32Window, ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	void UpdateViewportDimensions(int32_t width, int32_t height);
	bool LoadWorld(std::string filePath = "");
	bool PrepareLoading();
	bool FinalizeLoading();
	bool SaveWorld(std::string filePath);

	// Loops
	void Update(float deltaTime);
	void PeriodicUpdate(float deltaTime);
	void Render();

	// ECS
	void DestroyEntity(Entity entity);
	
	// Entity-Component relations
	Entity CreateEntity();

	template<typename TComponent>
	void AddComponent(Entity entity, const TComponent& component) const
	{
		_renderingApplication->AddComponent(entity, component);
	}

	template<typename TComponent>
	void RemoveComponent(Entity entity) const
	{
		_renderingApplication->RemoveComponent<TComponent>(entity);
	}

	// ----- Instance Rendering System Methods -----
	void AddRenderableInstance(int poolKey, int entityId, const InstanceConstantBuffer& instanceData);
	void UpdateRenderableInstanceData(int poolKey, int instanceIndex, const InstanceConstantBuffer& newData);
	void RemoveRenderableInstance(int poolKey, int entityId);

	std::vector<int> GetRenderableEntities(
		const std::unordered_map<int, int>& transformIndices,
		const std::unordered_map<int, int>& meshIndices,
		const std::unordered_map<int, int>& materialIndices
	) const;

	// ----- Serialization -----

	template <typename Archive>
	void save(Archive& archive) const
	{

	}

	template <typename Archive>
	void load(Archive& archive)
	{

		FinalizeLoading();
	}
};