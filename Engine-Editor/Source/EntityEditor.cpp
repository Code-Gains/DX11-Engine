#include "EntityEditor.hpp"

EntityEditor::EntityEditor()
{
}

EntityEditor::EntityEditor(ECS* ecs) : _ecs(ecs)
{
}

void EntityEditor::Render()
{
	if (!_enabled)
		return;

	ImGui::Begin("Entity Editor");
	auto worldHierarchySystem = _ecs->GetSystem<WorldHierarchy>();
	_selected = worldHierarchySystem->GetSelectedEntity();

	auto typesToComponents = _ecs->GetAllComponents(_selected);
	if (typesToComponents.size() == 0)
	{
		ImGui::Text("Select an entity in the hierarchy window to begin editting.");
		ImGui::End();
		return;
	}

	for (auto typeToComponent : typesToComponents)
	{
		auto& type = typeToComponent.first;
		auto& component = typeToComponent.second;
		auto headerNameOpt = ComponentRegistry::GetComponentNameByType(type);
		if (!headerNameOpt) // could not find component in registry
			continue;

		auto uiFactory = ComponentRegistry::GetUIFactoryFunction(type);
		if (!uiFactory)
			continue;

		if (ImGui::CollapsingHeader(headerNameOpt.value().c_str(), type))
		{
			//ImGui::Indent();
			// Get the UI factory function
			auto uiFactory = ComponentRegistry::GetUIFactoryFunction(type);
			if (uiFactory)
			{ 
				std::unique_ptr<ComponentUI> componentUI = uiFactory();
				componentUI->RenderUI(*component);
			}
			//ImGui::Unindent();
		}
	}
	ImGui::End();
}

void EntityEditor::Update(float deltaTime)
{
	//if (!_enabled)
		//return;

	//auto worldHierarchySystem = _ecs->GetSystem<WorldHierarchy>();
	//_selected = worldHierarchySystem->GetSelectedEntity();
	//auto components = _ecs->GetAllComponents(_selected);

}

void EntityEditor::PeriodicUpdate(float deltaTime)
{
}

void EntityEditor::Toggle()
{
	_enabled = !_enabled;
}
