#include "World.hpp"
#include "WorldHierarchy.hpp"

WorldHierarchy::WorldHierarchy()
{
}

WorldHierarchy::WorldHierarchy(World* world) : _world(world)
{
}

void WorldHierarchy::Update(float deltaTime)
{
}

void WorldHierarchy::Render()
{
	ImGui::Begin("World Hierarchy");
	if (ImGui::BeginMenu("Add"))
	{
		if (ImGui::BeginMenu("Primitives 3D"))
		{
			if (ImGui::MenuItem("Cube"))
			{
				CreatePrimitiveGeometry3D(PrimitiveGeometryType3D::Cube, "Cube");
			}
			else if (ImGui::MenuItem("Sphere"))
			{
				CreatePrimitiveGeometry3D(PrimitiveGeometryType3D::Sphere, "Sphere");
			}
			else if (ImGui::MenuItem("Cylinder"))
			{
				CreatePrimitiveGeometry3D(PrimitiveGeometryType3D::Cylinder, "Cylinder");
			}
			else if (ImGui::MenuItem("Pipe"))
			{
				CreatePrimitiveGeometry3D(PrimitiveGeometryType3D::Pipe, "Pipe");
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenu();
	}
	static auto selected = _entityToName.end();
	bool deleteSelected = false;
	if (ImGui::Button("-"))
	{
		deleteSelected = true;
	}

	static DirectX::XMFLOAT3 selectedPosition = { 0.0f, 0.0f, 0.0f };
	static DirectX::XMFLOAT3 selectedRotation = { 0.0f, 0.0f, 0.0f };
	static DirectX::XMFLOAT3 selectedScale = { 1.0f, 1.0f, 1.0f };
	if (selected != _entityToName.end())
	{
		auto transform = _world->GetTransformComponent(selected->first);
		if (transform)
		{
			selectedPosition = transform->GetPosition();
			selectedRotation = transform->GetRotation();
			selectedScale = transform->GetScale();
		}
		if (ImGui::TreeNode("Transform"))
		{
			if (ImGui::DragFloat3("Position", &selectedPosition.x, 0.1f))
			{
				transform->SetPosition(selectedPosition);
			}
			if (ImGui::DragFloat3("Rotation", &selectedRotation.x, 0.1f))
			{
				transform->SetRotation(selectedRotation);
			}
			if (ImGui::DragFloat3("Scale", &selectedScale.x, 0.1f))
			{
				transform->SetScale(selectedScale);
			}
			ImGui::TreePop();

		}
	}

	ImGui::Text("Entities");
	if (ImGui::BeginTable("EntityTable", 1, ImGuiTableFlags_Borders))
	{
		auto entityToName = _entityToName.begin();
		for (auto& item : _entityToName)
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			auto name = item.second + " " + std::to_string(item.first);
			if(ImGui::Selectable(name.c_str(), entityToName == selected))
				selected = entityToName;
			entityToName++;
		}
		ImGui::EndTable();
	}
	ImGui::End();

	if (deleteSelected)
	{
		auto selectedEntityId = selected->first;
		auto mesh = _world->GetMeshComponent(selected->first);
		if(mesh)
			_world->RemoveRenderableInstance(mesh->GetInstancePoolIndex(), selectedEntityId);
		_world->RemoveTransformComponent(selectedEntityId);
		_world->RemoveMeshComponent(selectedEntityId);
		_world->RemoveMaterialComponent(selectedEntityId);
		_entityToName.erase(selected);
		selected = _entityToName.end();
	}
}

void WorldHierarchy::AddEntity(int entityId, std::string entityName)
{
	_entityToName[entityId] = entityName;
}

int WorldHierarchy::CreatePrimitiveGeometry3D(PrimitiveGeometryType3D type, std::string name)
{
	auto geometry = _world->CreateEntity(_world->GetNextEntityId());

	auto transform = TransformComponent(_world->GetNextComponentId());
	_world->AddComponent(geometry.GetId(), transform);

    auto mesh = MeshComponent::GetPrimitiveMeshComponent(_world->GetNextComponentId(), type);
	_world->AddComponent(geometry.GetId(), mesh);

    auto material = MaterialComponent::GetDefaultMaterialComponent(_world->GetNextComponentId());
	_world->AddComponent(geometry.GetId(), material);

    _world->AddEntity(geometry);
	AddEntity(geometry.GetId(), name);

	return geometry.GetId();
}

std::string WorldHierarchy::GetEntityName(int entityId) const
{
	return std::string();
}