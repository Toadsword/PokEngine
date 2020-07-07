#include <ResourcesManager/PrefabsManager/null_prefab_manager.h>

namespace poke {

std::vector<ecs::EntityIndex> NullPrefabsManager::Instantiate(
    const nonstd::string_view& prefabName) const { return {}; }

const ecs::Prefab& NullPrefabsManager::GetPrefab(
    const nonstd::string_view& prefabName) const
{
    cassert(false, "Impossible to access NullPrefabManager");
	abort();
}

const std::vector<ResourceID>& NullPrefabsManager::GetPrefabsIDs() const
{
    cassert(false, "Impossible to use NullPrefabManager");
	abort();
}
} //namespace poke