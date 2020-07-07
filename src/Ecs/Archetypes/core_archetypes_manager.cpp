#include <Ecs/Archetypes/core_archetypes_manager.h>
#include <json.hpp>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>

namespace poke {
namespace ecs {

CoreArchetypesManager::CoreArchetypesManager()
{
    archetypesNames_.push_back("default");
    archetypes_.push_back(Archetype());
}

const Archetype& CoreArchetypesManager::GetArchetype(
    const std::string& archetypeName) const
{
	return GetArchetype(GetArchetypeID(archetypeName));
}

const Archetype& CoreArchetypesManager::GetArchetype(const ArchetypeID archetypeID) const
{
	return archetypes_[archetypeID];
}

ArchetypeID CoreArchetypesManager::GetArchetypeID(
    const std::string& archetypeName) const
{
	for (size_t i = 0; i < archetypesNames_.size(); i++) {
        if(archetypeName == archetypesNames_[i]) {
			return i;
        }
    }

	return defaultArchetypeID;
}

void CoreArchetypesManager::AddArchetype(
    const Archetype& archetype,
    const std::string& name,
    const size_t size)
{
	archetypes_.push_back(archetype);
	archetypesNames_.push_back(name);
    EcsManagerLocator::Get().AddArchetype(archetype, size);
}

void CoreArchetypesManager::SetFromJson(const json& archetypesJson)
{
	for (const auto& archetypeJson : archetypesJson) {

		Archetype archetype;
		archetype.SetFromJson(archetypeJson["archetype"]);
		auto size = archetypeJson["size"];
		const auto archetypeName = archetypeJson["name"].get<std::string>();

		AddArchetype(archetype, archetypeName, size);
		const auto archetypeID = GetArchetypeID(archetypeName);

		auto& ecsManager = EcsManagerLocator::Get();

		auto& modelsManager = ecsManager.GetComponentsManager<ModelsManager>();
		auto& rigidbodyManager = ecsManager.GetComponentsManager<RigidbodyManager>();
		auto& transformManager = ecsManager.GetComponentsManager<TransformsManager>();
		auto& collidersManager = ecsManager.GetComponentsManager<CollidersManager>();
		auto& splinesManager = ecsManager.GetComponentsManager<SplineFollowersManager>();
		auto& lightsManager = ecsManager.GetComponentsManager<LightsManager>();
		auto& audioSourcesManager = ecsManager.GetComponentsManager<AudioSourcesManager>();

		const auto pool = ecsManager.GetPools()[archetypeID];
        
        for(size_t i = 0; i < archetypeJson["entities"].size(); i++) {
			const auto entityIndex = i + pool.firstEntity;

            if(archetype.HasComponent(ComponentType::COLLIDER) && 
			   CheckJsonExists(archetypeJson["entities"][i], "collider")) {
				collidersManager.SetComponentFromJson(entityIndex, archetypeJson["entities"][i]["collider"]);
            }

			if (archetype.HasComponent(ComponentType::MODEL) && 
				CheckJsonExists(archetypeJson["entities"][i], "model")) {
				modelsManager.SetComponentFromJson(entityIndex, archetypeJson["entities"][i]["model"]);
			}

			if (archetype.HasComponent(ComponentType::RIGIDBODY) &&
				CheckJsonExists(archetypeJson["entities"][i], "rigidbody")) {
				rigidbodyManager.SetComponentFromJson(entityIndex, archetypeJson["entities"][i]["rigidbody"]);
			}

			if (archetype.HasComponent(ComponentType::SPLINE_FOLLOWER) &&
				CheckJsonExists(archetypeJson["entities"][i], "spline")) {
				splinesManager.SetComponentFromJson(entityIndex, archetypeJson["entities"][i]["spline"]);
			}

			if (archetype.HasComponent(ComponentType::TRANSFORM) &&
				CheckJsonExists(archetypeJson["entities"][i], "transform")) {
				transformManager.SetComponentFromJson(entityIndex, archetypeJson["entities"][i]["transform"]);
			}

			if (archetype.HasComponent(ComponentType::LIGHT) &&
				CheckJsonExists(archetypeJson["entities"][i], "light")) {
				lightsManager.SetComponentFromJson(entityIndex, archetypeJson["entities"][i]["light"]);
			}

			if (archetype.HasComponent(ComponentType::AUDIO_SOURCE) &&
				CheckJsonExists(archetypeJson["entities"][i], "audioSource")) {
				audioSourcesManager.SetComponentFromJson(entityIndex, archetypeJson["entities"][i]["audioSource"]);
			}
        }
    }
}

json CoreArchetypesManager::ToJson() const
{
	json archetypesJson;
	for (int i = 0; i < archetypes_.size(); i++) {
        if(archetypesNames_[i] == "default") {
			continue;
        }

		archetypesJson[i]["archetype"] = archetypes_[i].ToJson();
		archetypesJson[i]["size"] = archetypes_[i].size;
		archetypesJson[i]["name"] = archetypesNames_[i];

		auto& ecsManager = EcsManagerLocator::Get();

		const auto archetypeID = GetArchetypeID(archetypesNames_[i]);

		auto& modelsManager = ecsManager.GetComponentsManager<ModelsManager>();
		auto& rigidbodyManager = ecsManager.GetComponentsManager<RigidbodyManager>();
		auto& transformManager = ecsManager.GetComponentsManager<TransformsManager>();
		auto& collidersManager = ecsManager.GetComponentsManager<CollidersManager>();
		auto& splinesManager = ecsManager.GetComponentsManager<SplineFollowersManager>();
		auto& lightsManager = ecsManager.GetComponentsManager<LightsManager>();
		auto& audioSourcesManager = ecsManager.GetComponentsManager<AudioSourcesManager>();

		const auto pool = ecsManager.GetPools()[archetypeID];

		int j = 0;
		for (size_t entityIndex = pool.firstEntity; entityIndex < pool.lastEntity; entityIndex++) {
			if (archetypes_[i].HasComponent(ComponentType::COLLIDER) &&
				collidersManager.GetComponent(entityIndex) != archetypes_[i].collider) {

				archetypesJson["entities"][j]["collider"] = collidersManager.GetJsonFromComponent(entityIndex);
			}

			if (archetypes_[i].HasComponent(ComponentType::MODEL) &&
				modelsManager.GetComponent(entityIndex) != archetypes_[i].model ) {

				archetypesJson["entities"][j]["model"] = modelsManager.GetJsonFromComponent(entityIndex);
			}

			if (archetypes_[i].HasComponent(ComponentType::RIGIDBODY) && 
				rigidbodyManager.GetComponent(entityIndex) != archetypes_[i].rigidbody) {

				archetypesJson["entities"][j]["rigidbody"] = rigidbodyManager.GetJsonFromComponent(entityIndex);
			}

			if (archetypes_[i].HasComponent(ComponentType::SPLINE_FOLLOWER) &&
				splinesManager.GetComponent(entityIndex) != archetypes_[i].spline ) {

				archetypesJson["entities"][j]["spline"] = splinesManager.GetJsonFromComponent(entityIndex);
			}

			if (archetypes_[i].HasComponent(ComponentType::TRANSFORM) && 
				transformManager.GetComponent(entityIndex) != archetypes_[i].transform ) {

				archetypesJson["entities"][j]["transform"] = transformManager.GetJsonFromComponent(entityIndex);
			}

			if (archetypes_[i].HasComponent(ComponentType::LIGHT) &&
				lightsManager.GetComponent(entityIndex) != archetypes_[i].light) {
				archetypesJson["entities"][j]["light"] = collidersManager.GetJsonFromComponent(entityIndex);
			}

			if (archetypes_[i].HasComponent(ComponentType::AUDIO_SOURCE) &&
				audioSourcesManager.GetComponent(entityIndex) != archetypes_[i].audioSource) {
				archetypesJson["entities"][j]["audioSource"] = audioSourcesManager.GetJsonFromComponent(entityIndex);
			}

			j++;
		}
	}

	return archetypesJson;
}

void CoreArchetypesManager::UpdateArchetype(
    const ArchetypeID archetypeID,
    const Archetype& archetype)
{
	archetypes_[archetypeID] = archetype;
	EcsManagerLocator::Get().UpdateArchetype(archetypeID, archetype);
}

void CoreArchetypesManager::ResizeArchetype(
    const ArchetypeID archetypeID,
    const size_t newSize)
{
	archetypes_[archetypeID].size = newSize;
	EcsManagerLocator::Get().ResizeArchetype(archetypeID, newSize, archetypes_[archetypeID]);
}

void CoreArchetypesManager::RenameArchetype(
    const ArchetypeID archetypeID,
    const std::string& newName)
{
	archetypesNames_[archetypeID] = newName;
}

EntityPool CoreArchetypesManager::GetEntityPool(const ArchetypeID archetypeID)
{
    return EcsManagerLocator::Get().GetPools()[archetypeID];
}

const std::vector<std::string>& CoreArchetypesManager::
GetAllArchetypesNames() { return archetypesNames_; }
} //namespace poke
} //namespace ecs