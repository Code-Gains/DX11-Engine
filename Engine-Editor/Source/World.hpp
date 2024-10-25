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
#include "DirectionalLightComponent.hpp"
#include "ProfilerComponent.hpp"

#include "EditorUIManagerSystem.hpp"
#include "InstanceRendererSystem.hpp"
#include "LightingSystem.hpp"
#include "CameraSystem.hpp"
#include "EntityMonitoringSystem.hpp"
#include "ProfilerSystem.hpp"

#include "VertexType.hpp"
#include "Constants.hpp"
#include "WorldHierarchy.hpp"
#include "RenderingApplication3D.hpp"

#include "ProfilerWindow.hpp"

class Universe; // forward declaration

class World
{
	// Application State
	HWND _win32Window;

	int32_t _viewportWidth;
	int32_t _viewportHeight;

	RenderingApplication3D* _renderingApplication;
	Universe* _universe;

	// UI TODO MOVE OUT TO A SEPARATE ENGINE MODULE
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
	TComponent* GetComponent(Entity entity)
	{
		return _renderingApplication->GetComponent<TComponent>(entity);
	}

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

	std::vector<int> GetRenderableEntities(
		const std::unordered_map<int, int>& transformIndices,
		const std::unordered_map<int, int>& meshIndices,
		const std::unordered_map<int, int>& materialIndices
	) const;
};