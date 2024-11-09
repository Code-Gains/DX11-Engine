#include "World.hpp"
#include "WorldHierarchy.hpp"

WorldHierarchy::WorldHierarchy()
{
}

WorldHierarchy::WorldHierarchy(ECS* ecs, World* world) : _ecs(ecs), _world(world)
{
}

void WorldHierarchy::Update(float deltaTime)
{
}

void WorldHierarchy::PeriodicUpdate(float deltaTime)
{
}

Entity WorldHierarchy::GetSelectedEntity()
{
	return _selectedEntity;
}

void WorldHierarchy::Render()
{
	if (!_enabled)
		return;

	// querry returns a vector of tuples that contain component vectors
	auto componentQueryResult = _ecs->QueryComponentVectors<WorldHierarchyComponent>();
	// iterate
	for (auto& tuple : componentQueryResult)
	{
		auto& hierarchies = std::get<0>(tuple);

		// We want to work with the std::vector
		auto& rawHierarchies = *hierarchies->GetRawVector();

		for (auto& entityToComponent : hierarchies->GetEntityToIndex())
		{
			auto id = entityToComponent.first;
			auto idx = entityToComponent.second;

			auto& hierarchy = rawHierarchies[idx];

			auto& entityToName = hierarchy.GetEntityToNameRef();

			static auto selected = entityToName.end();

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
						CreatePrimitiveGeometry3D(PrimitiveGeometryType3D::Cube, "Cube", entityToName);
					}
					else if (ImGui::MenuItem("Sphere"))
					{
						CreatePrimitiveGeometry3D(PrimitiveGeometryType3D::Sphere, "Sphere", entityToName);
					}
					else if (ImGui::MenuItem("Cylinder"))
					{
						CreatePrimitiveGeometry3D(PrimitiveGeometryType3D::Cylinder, "Cylinder", entityToName);
					}
					else if (ImGui::MenuItem("Pipe"))
					{
						CreatePrimitiveGeometry3D(PrimitiveGeometryType3D::Pipe, "Pipe", entityToName);
					}
					else if (ImGui::MenuItem("Terrain"))
					{
						CreatePrimitiveGeometry3D(PrimitiveGeometryType3D::TerrainChunk, "Terrain", entityToName);
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

			if (selected != entityToName.end())
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
				auto etn = entityToName.begin();
				for (auto& item : entityToName)
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					auto name = item.second + " " + std::to_string(item.first);
					if (ImGui::Selectable(name.c_str(), etn == selected))
					{
						selected = etn;
						_selectedEntity = item.first;
					}
					etn++;
				}
				ImGui::EndTable();
			}
			ImGui::End();

			if (deleteSelected && selected != entityToName.end())
			{
				auto selectedEntityId = selected->first;
				_world->DestroyEntity(selectedEntityId);
				entityToName.erase(selected);

				// deselect
				selected = entityToName.end();
				//_selectedEntity = item.first;
			}
		}
	}
}

int WorldHierarchy::CreatePrimitiveGeometry3D(PrimitiveGeometryType3D type, std::string name, std::unordered_map<Entity, std::string>& entityToName)
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

	entityToName[geometry] = name;

	return geometry;
}

void WorldHierarchy::SetWorld(World* world)
{
	_world = world;
}

void WorldHierarchy::Toggle()
{
	_enabled = !_enabled;
}
