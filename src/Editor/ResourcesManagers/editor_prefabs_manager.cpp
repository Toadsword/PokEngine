#include <Editor/ResourcesManagers/editor_prefabs_manager.h>

#include <Utility/log.h>
#include <Utility/json_utility.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>

namespace poke {
namespace editor {

EditorPrefabsManager::EditorPrefabsManager() {}


std::vector<ecs::EntityIndex> EditorPrefabsManager::Instantiate(
	const nonstd::string_view& prefabName) const
{
	const auto hash = math::HashString(prefabName);
	for (size_t i = 0; i < prefabsID_.size(); i++) {
		if (hash == prefabsID_[i]) {
			return EcsManagerLocator::Get().InstantiatePrefab(prefabs_[i]);
		}
	}

	LogWarning(
		"You're trying to instantiate a prefab with the name " + static_cast<std
		::string>(prefabName) +
		" but this prefab has not been loaded by the manager",
		LogType::ECS_LOG);
	return {};
}

void EditorPrefabsManager::AddPrefab(const std::string& prefabName)
{
	const auto hash = math::HashString(prefabName);

	const auto it = std::find(prefabsID_.begin(), prefabsID_.end(), hash);
	if (it != prefabsID_.end()) return;

    prefabsID_.push_back(hash);

	json prefabJson = LoadJson(
		prefabName,
		FileType::PREFAB,
		FolderType::ROM);

	prefabJson[0]["prefabName"] = prefabName;

	prefabs_.emplace_back();
	prefabs_.back().SetFromJson(prefabJson);

    prefabsNames_.push_back(prefabName);
}

const ecs::Prefab& EditorPrefabsManager::GetPrefab(
	const nonstd::string_view& prefabName) const
{
	const auto hash = math::HashString(prefabName);
	const auto it = std::find(
		prefabsID_.begin(),
		prefabsID_.end(),
		hash);

	const auto index = std::distance(prefabsID_.begin(), it);

	return prefabs_[index];
}

const std::vector<ResourceID>& EditorPrefabsManager::GetPrefabsIDs() const
{
	return prefabsID_;
}

void EditorPrefabsManager::Clear()
{
    prefabsID_.clear();
    prefabs_.clear();
    prefabsNames_.clear();
}

void EditorPrefabsManager::Resize(const size_t newSize)
{
	prefabsID_.resize(newSize);
	prefabs_.resize(newSize);
	prefabsNames_.resize(newSize);
}

json EditorPrefabsManager::ToJson()
{
	json prefabsJson;

    for(size_t i = 0; i < prefabs_.size(); i++) {
		prefabsJson.push_back(prefabsNames_[i]);
		/*prefabsJson[i]["name"] = prefabsNames_[i];
		prefabsJson[i]["objects"] = prefabs_[i].ToJson();*/
    }

	return prefabsJson;
}

void EditorPrefabsManager::SetFromJson(const json& prefabsJson)
{
	for (const auto& prefabJson : prefabsJson) {
		std::string prefabName = prefabJson;

		AddPrefab(prefabName);
	}
}

const std::vector<std::string>& EditorPrefabsManager::
GetAllPrefabsNames() const { return prefabsNames_; }

void EditorPrefabsManager::SetPrefab(
    const editor::EditorPrefab& prefab,
    const std::string& name)
{
    const auto hash = math::HashString(name);

    SetPrefab(prefab, hash);

    LogWarning(
        "You tried to update the prefab : " + name +
        "But this prefabs doesn't seems to exist");
}

void EditorPrefabsManager::SetPrefab(
    const editor::EditorPrefab& prefab,
    const ResourceID prefabID)
{
    for (size_t i = 0; i < prefabsID_.size(); i++) {
        if (prefabID == prefabsID_[i]) {
            prefabs_[i] = prefab;

            PokFileSystem::WriteFile(
                prefabsNames_[i],
                prefab.ToJson(),
                FileType::PREFAB);
            return;
        }
    }
}

void EditorPrefabsManager::AddPrefab(
    const editor::EditorPrefab& prefab,
    const std::string& name)
{

	const auto it = std::find(prefabsNames_.begin(), prefabsNames_.end(), name);
    if(it != prefabsNames_.end()) {
		LogWarning(
			"You tried to create a prefab with the name : " + name +
			" but this prefab already exist.");
		return;
    }

    prefabs_.push_back(prefab);
    prefabsNames_.push_back(name);
    prefabsID_.push_back(math::HashString(name));

	PokFileSystem::WriteFile(
		name,
		prefab.ToJson(),
		FileType::PREFAB);
}
} //namespace editor
} //namespace poke