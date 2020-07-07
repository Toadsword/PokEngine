#include <ResourcesManager/PrefabsManager/core_prefab_manager.h>

#include <Utility/log.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>

namespace poke {

CorePrefabsManager::CorePrefabsManager() {}

std::vector<ecs::EntityIndex> CorePrefabsManager::Instantiate(
    const nonstd::string_view& prefabName) const
{
	const auto hash = math::HashString(prefabName);
    for (size_t i = 0; i < prefabsID_.size(); i++) {
        if (hash == prefabsID_[i]) {
            return EcsManagerLocator::Get().InstantiatePrefab(prefabs_[i]);
        }
    }

	LogWarning("You're trying to instantiate a prefab with the name " + static_cast<std::string>(prefabName) + " but this prefab has not been loaded by the manager", LogType::ECS_LOG);
	return {};
}

void CorePrefabsManager::AddPrefab(const std::string& prefabName)
{
	const auto hash = math::HashString(prefabName);

	const auto it = std::find(prefabsID_.begin(), prefabsID_.end(), hash);
	if (it != prefabsID_.end()) return;

    prefabsID_.push_back(hash);

	auto prefabJson = LoadJson(
		prefabName,
		FileType::PREFAB,
		FolderType::ROM);

	prefabJson[0]["prefabName"] = prefabName;

    prefabs_.emplace_back();
    prefabs_.back().SetFromJson(prefabJson);
}

const ecs::Prefab& CorePrefabsManager::GetPrefab(const nonstd::string_view& prefabName) const
{
	const auto hash = math::HashString(prefabName);
    const auto it = std::find(
		prefabsID_.begin(),
		prefabsID_.end(),
		hash);

    const auto index = std::distance(prefabsID_.begin(), it);

    return prefabs_[index];
}

const std::vector<ResourceID>& CorePrefabsManager::GetPrefabsIDs() const
{
    return prefabsID_;
}

void CorePrefabsManager::Clear()
{
    prefabsID_.clear();
    prefabs_.clear();
}

void CorePrefabsManager::Resize(const size_t newSize)
{
    prefabs_.resize(0);
    prefabsID_.resize(0);
}

json CorePrefabsManager::ToJson()
{
	LogWarning("Impossible to save with the CorePrefabManager as its meant to be read-only");
	return json();
}

void CorePrefabsManager::SetFromJson(const json& prefabsJson)
{
	prefabsID_.reserve(prefabsJson.size());
	prefabs_.resize(prefabsJson.size());

    for(size_t i = 0; i < prefabsJson.size(); i++) {
		prefabsID_.emplace_back(math::HashString(prefabsJson[i]["name"].get<std::string>()));
		prefabs_[i].SetFromJson(prefabsJson[i]["objects"]);
    }
}
} //namespace poke