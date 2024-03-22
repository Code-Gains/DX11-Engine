#include "ECSDebugger.hpp"

ECSDebugger::ECSDebugger() : _ecs(nullptr)
{
}

ECSDebugger::ECSDebugger(ECS* ecs) : _ecs(ecs)
{
}

ECSDebugger::~ECSDebugger()
{
}

void ECSDebugger::Update(float deltaTime)
{
}

void ECSDebugger::PeriodicUpdate(float deltaTime)
{
}

void ECSDebugger::Render()
{
    ImGui::Begin("ECS Debugger");
    ImGui::Text("Entity Count: %d", _ecs->GetEntityCount());
    ImGui::Text("Archetype Count: %d", _ecs->GetArchetypeCount());

	if (ImGui::BeginTable("ArchetypeTable", 1, ImGuiTableFlags_Borders))
	{
		auto& signatureToArchetype = _ecs->GetSignatureToArchetype();

		for (auto& signatureIt : signatureToArchetype)
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);

			auto signature = signatureIt.second->GetSignature();

			auto fullBitsetStr = signature.to_string();
			int firstOnePos = fullBitsetStr.find('1');
			auto bitsetStr = fullBitsetStr.substr(firstOnePos);
			//ImGui::Text("%s", bitsetStr.c_str());
			if (ImGui::TreeNode(bitsetStr.c_str()))
			{
				auto& typeToComponentVector = signatureIt.second.get()->GetTypeToComponentVector();
				for (auto& typeIt : typeToComponentVector)
				{
					ImGui::Text("Type: %d Count: %d", typeIt.first, typeIt.second.get()->GetComponentCount());
				}
				ImGui::TreePop();
			}
		}
		ImGui::EndTable();
	}

    // Archetype -> Expandable
    //  -> Component Vector -> Expandable
    //      -> Entity-Component

    ImGui::End();
}
