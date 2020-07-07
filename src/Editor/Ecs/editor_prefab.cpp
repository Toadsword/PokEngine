#include <Editor/Ecs/editor_prefab.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>
#include <Editor/ComponentManagers/editor_component_manager.h>

namespace poke::editor {

const std::vector<game::GameArchetype>& EditorPrefab::GetArchetypes() const
{
	return archetypes_;
}

void EditorPrefab::SetArchetypes(const std::vector<game::GameArchetype>& archetypes)
{
	archetypes_ = archetypes;
}

std::vector<ecs::EntityIndex> EditorPrefab::Instantiate() const
{
	const auto generatedEntities = EcsManagerLocator::Get().InstantiatePrefab(*this);
	ecs::EditorComponentManager& editorComponentManager = EcsManagerLocator::Get().GetComponentsManager<ecs::EditorComponentManager>();

	for (size_t i = 0; i < generatedEntities.size(); i++) {

		auto editorComponent = editorComponentManager.GetComponent(generatedEntities[i]);
		editorComponent.name = names_[i];
		editorComponentManager.SetComponent(generatedEntities[i], editorComponent);
		EcsManagerLocator::Get().AddComponent(generatedEntities[i], ecs::ComponentType::EDITOR_COMPONENT);
	}
	return generatedEntities;
}

const std::vector<size_t>& EditorPrefab::GetParentIndexes()const
{
	return parentIndexes_;
}

void EditorPrefab::SetParentIndexes(const std::vector<size_t>& parentIndexes)
{
	parentIndexes_ = parentIndexes;
}
void EditorPrefab::Clear()
{
	archetypes_.clear();
}

const ecs::Archetype& EditorPrefab::GetObject(const size_t index) const
{
	return archetypes_[index];
}

void EditorPrefab::SetNames(const std::vector<std::string>& names)
{
	names_ = names;
}
void EditorPrefab::SetArchetypesId(const std::vector<ecs::ArchetypeID>& archetypesId)
{
	archetypesID_ = archetypesId;
}

void EditorPrefab::SetFromJson(const json& prefabJson)
{
	names_.resize(prefabJson.size());
	archetypes_.resize(prefabJson.size());
	archetypesID_.resize(prefabJson.size());
	parentIndexes_.resize(prefabJson.size());

	for (size_t i = 0; i < prefabJson.size(); i++) {
		archetypes_[i].SetFromJson(prefabJson[i]);

		if (CheckJsonExists(prefabJson[i], "archetypeID")) {
			archetypesID_[i] = prefabJson[i]["archetypeID"];
		}

		parentIndexes_[i] = prefabJson[i]["transform"]["parent"];
		names_[i] = prefabJson[i]["objectName"].get<std::string>();

	}
}

json EditorPrefab::ToJson() const
{
	json prefabJson;

	for (size_t i = 0; i < names_.size(); i++) {
		prefabJson[i] = archetypes_[i].ToJson();

		if (archetypesID_[i] != 0) {
			prefabJson[i]["archetypeID"] = archetypesID_[i];
		}

		prefabJson[i]["transform"]["parent"] = parentIndexes_[i];
		prefabJson[i]["objectName"] = names_[i];
	}

	return prefabJson;
}
}//namespace poke::editor
