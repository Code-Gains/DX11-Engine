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

void WorldHierarchy::Render(/*Pass Entity Data From World*/)
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
	if (ImGui::Button("-"))
	{
		/*std::cout << selected << std::endl;
		auto entityToName = _entityToName.find(selected);
		if (entityToName != _entityToName.end())
		if (entityToName != _entityToName.end())
		{*/
		auto selectedEntityId = selected->first;
		_world->RemoveRenderableInstance(0, selectedEntityId);
		_world->RemoveTransformComponent(selectedEntityId);
		_world->RemoveMeshComponent(selectedEntityId);
		_world->RemoveMaterialComponent(selectedEntityId);
		auto entityToName = _entityToName.find(selectedEntityId);
		if (entityToName != _entityToName.end())
		{
			_entityToName.erase(entityToName);
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
}

void WorldHierarchy::AddEntity(int entityId, std::string entityName)
{
	_entityToName[entityId] = entityName;
}

int WorldHierarchy::CreatePrimitiveGeometry3D(PrimitiveGeometryType3D type, std::string name)
{
	auto geometry = _world->CreateEntity();
	
	auto transform = TransformComponent(
		_world->GetNextComponentId(),
		DirectX::XMFLOAT3{0.0f, 0.0f, 0.0f},
		DirectX::XMFLOAT3{0.0f, 0.0f, 0.0f},
		DirectX::XMFLOAT3{1.0f, 1.0f, 1.0f}
	);
	_world->AddComponent(geometry.GetId(), transform);

    auto mesh = MeshComponent::GetPrimitiveMeshComponent(_world->GetNextComponentId(), type);
	mesh.SetInstancePoolIndex((int)type);

	_world->AddComponent(geometry.GetId(), mesh);

    auto material = MaterialComponent::GetDefaultMaterialComponent(_world->GetNextComponentId());
	_world->AddComponent(geometry.GetId(), material);

    _world->AddEntity(geometry);
	AddEntity(geometry.GetId(), name);
	std::cout << geometry.GetId() << std::endl;
	return geometry.GetId();
}

std::string WorldHierarchy::GetEntityName(int entityId) const
{
	return std::string();
}
