#pragma once
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

#include "InstanceRendererSystem.hpp"

#include "Cube.hpp"
#include "VertexType.hpp"
#include "Constants.hpp"

#include "WorldHierarchy.hpp"

// temp includes for demo
#include <random>

class World
{
	// Application State
	HWND _win32Window;

	int32_t _viewportWidth;
	int32_t _viewportHeight;

	// Memory management settings
	float _deadDataCompactionTreshold = 0.5f;

	// Entities
	std::vector<Entity> _entities;
	int _nextEntityId = 1;

	// Storage -> Component Data
	std::vector<TransformComponent> _transformComponents;
	std::vector<MeshComponent> _meshComponents;
	std::vector<MaterialComponent> _materialComponents;
	std::vector<LightComponent> _lightComponents;
	std::vector<CameraComponent> _cameraComponents;
	int _nextComponentId = 1;

	// Storage -> Component Cache
	std::unordered_map<int, InstanceRendererSystem::InstancePool> _instancePools;

	// Storage -> Component Relations
	// entityId -> componentIndex
	std::unordered_map<int, int> _transformComponentIndices;
	std::unordered_map<int, int> _meshComponentIndices;
	std::unordered_map<int, int> _materialComponentIndices;
	std::unordered_map<int, int> _lightComponentIndices;
	std::unordered_map<int, int> _cameraComponentIndices;

	std::vector<size_t> _freeTransforms;
	std::vector<size_t> _freeMeshes;
	std::vector<size_t> _freeMaterials;
	std::vector<size_t> _freeLights;
	std::vector<size_t> _freeCameras;

	// Systems
	InstanceRendererSystem _instanceRenderer;
	int _nextPoolId = 10000; // allocate 10000 to non user meshes TODO FIX

	// HLSL Constant Buffer Data
	LightConstantBuffer _lightConstantBufferData;
	PerFrameConstantBuffer _perFrameConstantBufferData{};
	CameraConstantBuffer _cameraConstantBufferData{};

	// UI
	WorldHierarchy _worldHierarchy;

public:
	// World loading and application management
	World();
	bool Initialize(HWND win32Window, ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	void UpdateViewportDimensions(int32_t width, int32_t height);
	bool LoadWorld(std::string fileName = "");

	// Loops
	void Update(float deltaTime);
	void PeriodicUpdate(float deltaTime);
	void Render();

	int GetNextEntityId() const;
	int GetNextComponentId() const;
	int GetNextPoolId() const;

	void IncrementEntityId();
	void IncrementPoolId();
	
	// Entity-Component relations
	void AddEntity(Entity entityId);
	Entity CreateEntity();
	void RemoveEntity(int id);

	void AddComponent(int entityId, const TransformComponent& component);
	void AddComponent(int entityId, const MeshComponent& component);
	void AddComponent(int entityId, const MaterialComponent& component);
	void AddComponent(int entityId, const LightComponent& component);
	void AddComponent(int entityId, const CameraComponent& component);

	void RemoveTransformComponent(int entityId);
	void RemoveMeshComponent(int entityId);
	void RemoveMaterialComponent(int entityId);
	void RemoveLightComponent(int entityId);
	void RemoveCameraComponent(int entityId);

	// Instance Rendering System

	void LinkEngineInstancePools();
	void LinkRenderableInstancePool(int index, const InstanceRendererSystem::InstancePool& instancePool);
	void LinkRenderableInstancePool(const InstanceRendererSystem::InstancePool& instancePool);
	void AddRenderableInstance(int poolKey, int entityId, const InstanceConstantBuffer& instanceData);
	void UpdateRenderableInstanceData(int poolKey, int instanceIndex, const InstanceConstantBuffer& newData);
	void RemoveRenderableInstance(int poolKey, int entityId);
	void RemoveAllRenderableInstances();

	void UpdateDirtyRenderableTransforms();

	std::vector<int> GetRenderableEntities(
		const std::unordered_map<int, int>& transformIndices,
		const std::unordered_map<int, int>& meshIndices,
		const std::unordered_map<int, int>& materialIndices
	) const;

};