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
	std::vector<TerrainComponent> _terrainComponents;
	int _nextComponentId = 1;

	// Storage -> Component Relations
	// entityId -> componentIndex
	std::unordered_map<int, int> _transformComponentIndices;
	std::unordered_map<int, int> _meshComponentIndices;
	std::unordered_map<int, int> _materialComponentIndices;
	std::unordered_map<int, int> _lightComponentIndices;
	std::unordered_map<int, int> _cameraComponentIndices;
	std::unordered_map<int, int> _terrainComponentIndices;

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

	int GetNextEntityId() const;
	int GetNextComponentId() const;

	void IncrementEntityId();
	
	// Entity-Component relations
	void AddEntity(Entity entityId);
	void RemoveEntity(int id);

	void AddComponent(int entityId, const TransformComponent& component);
	void AddComponent(int entityId, const MeshComponent& component);
	void AddComponent(int entityId, const MaterialComponent& component);
	void AddComponent(int entityId, const LightComponent& component);
	void AddComponent(int entityId, const CameraComponent& component);
	void AddComponent(int entityId, const TerrainComponent& component);

	void RemoveTransformComponent(int entityId);
	void RemoveMeshComponent(int entityId);
	void RemoveMaterialComponent(int entityId);
	void RemoveLightComponent(int entityId);
	void RemoveCameraComponent(int entityId);
	void RemoveTerrainComponent(int entityId);

	TransformComponent* GetTransformComponent(int entityId);
	MeshComponent* GetMeshComponent(int entityId);
	MaterialComponent* GetMaterialComponent(int entityId);
	//LightComponent* GetLightComponent(int entityId);
	//CameraComponent* GetCameraComponent(int entityId);

	// Instance Rendering System Methods
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

	// ----- Serialization ----- //

	template <typename Archive>
	void save(Archive& archive) const
	{
		// Entities
		archive(CEREAL_NVP(_entities), CEREAL_NVP(_nextEntityId));

		// Components
		archive(
			CEREAL_NVP(_transformComponents),
			CEREAL_NVP(_meshComponents),
			CEREAL_NVP(_materialComponents),
			CEREAL_NVP(_nextComponentId)
		);

		// Indices
		archive(
			CEREAL_NVP(_transformComponentIndices),
			CEREAL_NVP(_meshComponentIndices),
			CEREAL_NVP(_materialComponentIndices)
		);

		// UI
		archive(
			CEREAL_NVP(_worldHierarchy)
		);
	}

	template <typename Archive>
	void load(Archive& archive)
	{
		// Entities
		archive(CEREAL_NVP(_entities), CEREAL_NVP(_nextEntityId));

		// Components
		archive(
			CEREAL_NVP(_transformComponents),
			CEREAL_NVP(_meshComponents),
			CEREAL_NVP(_materialComponents),
			CEREAL_NVP(_nextComponentId)
		);

		// Indices
		archive(
			CEREAL_NVP(_transformComponentIndices),
			CEREAL_NVP(_meshComponentIndices),
			CEREAL_NVP(_materialComponentIndices)
		);

		// UI
		archive(
			CEREAL_NVP(_worldHierarchy)
		);

		FinalizeLoading();
	}
};