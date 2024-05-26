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
	static auto selected = _entityToName.end();

	bool exitEarly = false;
	ImGui::Begin("World Hierarchy");
	//if (ImGui::Button("Save"))
	//{
	//	_world->SaveWorld("./output.json");
	//}
	//ImGui::SameLine();
	//if (ImGui::Button("Load"))
	//{
	//	// Need some sort of prepare to load function TODO
	//	selected = _entityToName.end();
	//	_world->LoadWorld("./output.json");
	//	exitEarly = true;
	//}
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
			else if (ImGui::MenuItem("Terrain"))
			{
				CreatePrimitiveGeometry3D(PrimitiveGeometryType3D::TerrainChunk, "Terrain");
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenu();
	}

	if (exitEarly)
	{
		ImGui::End();
		return;
	}

	bool deleteSelected = false;
	if (ImGui::Button("-"))
	{
		deleteSelected = true;
	}

	static DirectX::XMFLOAT3 selectedPosition = { 0.0f, 0.0f, 0.0f };
	static DirectX::XMFLOAT3 selectedRotation = { 0.0f, 0.0f, 0.0f };
	static DirectX::XMFLOAT3 selectedScale = { 1.0f, 1.0f, 1.0f };

	static DirectX::XMFLOAT4 selectedAmbient = { 1.0f, 1.0f, 1.0f, 1.0f };
	static DirectX::XMFLOAT4 selectedDiffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	static DirectX::XMFLOAT4 selectedSpecular = { 1.0f, 1.0f, 1.0f, 1.0f };
	static float selectedShininess = 1.0f;

	if (selected != _entityToName.end())
	{
		auto transform = _world->GetComponent<TransformComponent>(selected->first);
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
		auto material = _world->GetComponent<MaterialComponent>(selected->first);
		if (material)
		{
			selectedAmbient = material->GetAmbient();
			selectedDiffuse = material->GetDiffuse();
			selectedSpecular = material->GetSpecular();
			selectedShininess = selectedShininess;
		}
		if (ImGui::TreeNode("Material"))
		{
			if (ImGui::ColorPicker4("Ambient", &selectedAmbient.x))
			{
				material->SetAmbient(selectedAmbient);
			}
			if (ImGui::ColorPicker4("Diffuse", &selectedDiffuse.x))
			{
				material->SetDiffuse(selectedDiffuse);
			}
			if (ImGui::ColorPicker4("Specular", &selectedSpecular.x))
			{
				material->SetSpecular(selectedSpecular);
			}
			if (ImGui::DragFloat("Shininess", &selectedShininess, 0.1f, 0.0f, 100.0f))
			{
				material->SetShininess(selectedShininess);
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
			if (ImGui::Selectable(name.c_str(), entityToName == selected))
				selected = entityToName;
			entityToName++;
		}
		ImGui::EndTable();
	}
	ImGui::End();

	if (deleteSelected && selected != _entityToName.end())
	{
		auto selectedEntityId = selected->first;
		_world->DestroyEntity(selectedEntityId);
		_entityToName.erase(selected);

		// deselect
		selected = _entityToName.end();
	}
}

int WorldHierarchy::CreatePrimitiveGeometry3D(PrimitiveGeometryType3D type, std::string name)
{
	auto geometry = _world->CreateEntity();
	auto transform = TransformComponent();
	_world->AddComponent(geometry, transform);

	//temporary to check system TODO SEPARATE

	if (type == PrimitiveGeometryType3D::TerrainChunk)
	{
		std::vector<std::vector<float>> heights;
		heights.resize(256);
		for (unsigned int i = 0; i < 256; ++i) {
			heights[i].resize(256, 0);
		}
		Heightmap heightmap = Heightmap(heights);
		auto mesh = MeshComponent<VertexPositionNormalUvHeight>::GenerateTerrainMeshComponent(type, heightmap);
		auto terrain = TerrainComponent(heightmap, &mesh);
		_world->AddComponent(geometry, mesh);
		_world->AddComponent(geometry, terrain);
	}
	else
	{
		auto mesh = MeshComponent<VertexPositionNormalUv>::GeneratePrimitiveMeshComponent(type);
		_world->AddComponent(geometry, mesh);
	}

	auto material = MaterialComponent::GetDefaultMaterialComponent();
	_world->AddComponent(geometry, material);

	_entityToName[geometry] = name;

	return geometry;
}

void WorldHierarchy::SetWorld(World* world)
{
	_world = world;
}

std::string WorldHierarchy::GetEntityName(int entityId) const
{
	return std::string();
}

void WorldHierarchy::Clear()
{
	_entityToName.clear();
}
