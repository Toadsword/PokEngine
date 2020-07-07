#include <Game/Ecs/game_archetypes_manager.h>

#include <CoreEngine/ServiceLocator/service_locator_definition.h>

#include <Game/ComponentManagers/destructible_element_manager.h>
#include <Game/ComponentManagers/enemies_manager.h>
#include <Game/ComponentManagers/missiles_manager.h>
#include <Game/ComponentManagers/player_manager.h>
#include <Game/ComponentManagers/weapon_manager.h>
#include <Game/ComponentManagers/spline_state_manager.h>
#include <Game/ComponentManagers/jiggle_manager.h>

namespace poke {
namespace game {

GameArchetypesManager::GameArchetypesManager() {
	archetypesNames_.push_back("default");
	gameArchetypes_.push_back(GameArchetype());
}

const ecs::Archetype& GameArchetypesManager::GetArchetype(const std::string& archetypeName) const {
	return GetArchetype(GetArchetypeID(archetypeName));
}

const ecs::Archetype& GameArchetypesManager::GetArchetype(ecs::ArchetypeID archetypeID) const {
	return gameArchetypes_[archetypeID];
}

ecs::ArchetypeID GameArchetypesManager::GetArchetypeID(const std::string& archetypeName) const {
	for (size_t i = 0; i < archetypesNames_.size(); i++) {
		if (archetypeName == archetypesNames_[i]) {
			return i;
		}
	}
}

void GameArchetypesManager::AddArchetype(const ecs::Archetype& archetype, const std::string& name, size_t size) {
	gameArchetypes_.push_back(static_cast<const GameArchetype&>(archetype));
	archetypesNames_.push_back(name);
	EcsManagerLocator::Get().AddArchetype(archetype, size);
}

void GameArchetypesManager::SetFromJson(const json& archetypesJson) {
	for (const auto& archetypeJson : archetypesJson) {

		GameArchetype archetype;
		archetype.SetFromJson(archetypeJson["archetype"]);
		auto size = archetypeJson["size"];
		const auto archetypeName = archetypeJson["name"].get<std::string>();

		AddArchetype(archetype, archetypeName, size);
		const auto archetypeID = GetArchetypeID(archetypeName);

		auto& ecsManager = EcsManagerLocator::Get();

		auto& modelsManager = ecsManager.GetComponentsManager<ecs::ModelsManager>();
		auto& rigidbodyManager = ecsManager.GetComponentsManager<ecs::RigidbodyManager>();
		auto& transformManager = ecsManager.GetComponentsManager<ecs::TransformsManager>();
		auto& collidersManager = ecsManager.GetComponentsManager<ecs::CollidersManager>();
		auto& splinesManager = ecsManager.GetComponentsManager<ecs::SplineFollowersManager>();
		auto& lightsManager = ecsManager.GetComponentsManager<ecs::LightsManager>();
		auto& audioSourcesManager = ecsManager.GetComponentsManager<ecs::AudioSourcesManager>();

	    //--------------------------------- Game Components Part ---------------------------------------
		auto& playerManager = ecsManager.GetComponentsManager<PlayerManager>();
		auto& destructibleElementManager = ecsManager.GetComponentsManager<DestructibleElementManager>();
		auto& enemyManager = ecsManager.GetComponentsManager<EnemiesManager>();
		auto& missileManager = ecsManager.GetComponentsManager<MissilesManager>();
		auto& projectileManager = ecsManager.GetComponentsManager<ProjectileManager>();
		auto& weaponManager = ecsManager.GetComponentsManager<WeaponManager>();
		auto& splineStateManager = ecsManager.GetComponentsManager<SplineStateManager>();
		auto& jiggleManager = ecsManager.GetComponentsManager<JiggleManager>();
        //-------------------------------------- End Game Part -------------------------------------------


		const auto pool = ecsManager.GetPools()[archetypeID];

		for (size_t i = 0; i < archetypeJson["entities"].size(); i++) {
			const auto entityIndex = i + pool.firstEntity;

			if (archetype.HasComponent(ecs::ComponentType::COLLIDER) &&
				CheckJsonExists(archetypeJson["entities"][i], "collider")) {
				collidersManager.SetComponentFromJson(entityIndex, archetypeJson["entities"][i]["collider"]);
			}

			if (archetype.HasComponent(ecs::ComponentType::MODEL) &&
				CheckJsonExists(archetypeJson["entities"][i], "model")) {
				modelsManager.SetComponentFromJson(entityIndex, archetypeJson["entities"][i]["model"]);
			}

			if (archetype.HasComponent(ecs::ComponentType::RIGIDBODY) &&
				CheckJsonExists(archetypeJson["entities"][i], "rigidbody")) {
				rigidbodyManager.SetComponentFromJson(entityIndex, archetypeJson["entities"][i]["rigidbody"]);
			}

			if (archetype.HasComponent(ecs::ComponentType::SPLINE_FOLLOWER) &&
				CheckJsonExists(archetypeJson["entities"][i], "spline")) {
				splinesManager.SetComponentFromJson(entityIndex, archetypeJson["entities"][i]["spline"]);
			}

			if (archetype.HasComponent(ecs::ComponentType::TRANSFORM) &&
				CheckJsonExists(archetypeJson["entities"][i], "transform")) {
				transformManager.SetComponentFromJson(entityIndex, archetypeJson["entities"][i]["transform"]);
			}

			if (archetype.HasComponent(ecs::ComponentType::LIGHT) &&
				CheckJsonExists(archetypeJson["entities"][i], "light")) {
				lightsManager.SetComponentFromJson(entityIndex, archetypeJson["entities"][i]["light"]);
			}

			if (archetype.HasComponent(ecs::ComponentType::AUDIO_SOURCE) &&
				CheckJsonExists(archetypeJson["entities"][i], "audioSource")) {
				audioSourcesManager.SetComponentFromJson(entityIndex, archetypeJson["entities"][i]["audioSource"]);
			}

            //----------------------------------- Game Components Part ------------------------------
			if (archetype.HasComponent(ecs::ComponentType::PLAYER) &&
				CheckJsonExists(archetypeJson["entities"][i], "player")) {
				playerManager.SetComponentFromJson(entityIndex, archetypeJson["entities"][i]["player"]);
			}

			if (archetype.HasComponent(ecs::ComponentType::DESTRUCTIBLE_ELEMENT) &&
				CheckJsonExists(archetypeJson["entities"][i], "destructibleElement")) {
				destructibleElementManager.SetComponentFromJson(entityIndex, archetypeJson["entities"][i]["destructibleElement"]);
			}

			if (archetype.HasComponent(ecs::ComponentType::ENEMY ) &&
				CheckJsonExists(archetypeJson["entities"][i], "enemy")) {
				enemyManager.SetComponentFromJson(entityIndex, archetypeJson["entities"][i]["enemy"]);
			}

			if (archetype.HasComponent(ecs::ComponentType::MISSILE) &&
				CheckJsonExists(archetypeJson["entities"][i], "missile")) {
				missileManager.SetComponentFromJson(entityIndex, archetypeJson["entities"][i]["missile"]);
			}

			if (archetype.HasComponent(ecs::ComponentType::PROJECTILE) &&
				CheckJsonExists(archetypeJson["entities"][i], "projectile")) {
				projectileManager.SetComponentFromJson(entityIndex, archetypeJson["entities"][i]["projectile"]);
			}

			if (archetype.HasComponent(ecs::ComponentType::WEAPON) &&
				CheckJsonExists(archetypeJson["entities"][i], "weapon")) {
				weaponManager.SetComponentFromJson(entityIndex, archetypeJson["entities"][i]["weapon"]);
			}

			if (archetype.HasComponent(ecs::ComponentType::SPLINE_STATES) &&
				CheckJsonExists(archetypeJson["entities"][i], "splineStates")) {
				splineStateManager.SetComponentFromJson(entityIndex, archetypeJson["entities"][i]["splineStates"]);
			}

			if (archetype.HasComponent(ecs::ComponentType::JIGGLE) &&
				CheckJsonExists(archetypeJson["entities"][i], "jiggle")) {
				jiggleManager.SetComponentFromJson(entityIndex, archetypeJson["entities"][i]["jiggle"]);
			}
            // End Game Part
		}
	}
}

json GameArchetypesManager::ToJson() const {
	json archetypesJson;
	for (int i = 0; i < gameArchetypes_.size(); i++) {
		if (archetypesNames_[i] == "default") {
			continue;
		}

		archetypesJson[i]["archetype"] = gameArchetypes_[i].ToJson();
		archetypesJson[i]["size"] = gameArchetypes_[i].size;
		archetypesJson[i]["name"] = archetypesNames_[i];

		auto& ecsManager = EcsManagerLocator::Get();

		const auto archetypeID = GetArchetypeID(archetypesNames_[i]);

		auto& modelsManager = ecsManager.GetComponentsManager<ecs::ModelsManager>();
		auto& rigidbodyManager = ecsManager.GetComponentsManager<ecs::RigidbodyManager>();
		auto& transformManager = ecsManager.GetComponentsManager<ecs::TransformsManager>();
		auto& collidersManager = ecsManager.GetComponentsManager<ecs::CollidersManager>();
		auto& splinesManager = ecsManager.GetComponentsManager<ecs::SplineFollowersManager>();
		auto& lightsManager = ecsManager.GetComponentsManager<ecs::LightsManager>();
		auto& audioSourcesManager = ecsManager.GetComponentsManager<ecs::AudioSourcesManager>();

		//--------------------------------- Game Components Part ---------------------------------------
		auto& playerManager = ecsManager.GetComponentsManager<PlayerManager>();
		auto& destructibleElementManager = ecsManager.GetComponentsManager<DestructibleElementManager>();
		auto& enemyManager = ecsManager.GetComponentsManager<EnemiesManager>();
		auto& missileManager = ecsManager.GetComponentsManager<MissilesManager>();
		auto& projectileManager = ecsManager.GetComponentsManager<ProjectileManager>();
		auto& weaponManager = ecsManager.GetComponentsManager<WeaponManager>();
		auto& splineStateManager = ecsManager.GetComponentsManager<SplineStateManager>();
		auto& jiggleManager = ecsManager.GetComponentsManager<JiggleManager>();
		//-------------------------------------- End Game Part -------------------------------------------

		const auto pool = ecsManager.GetPools()[archetypeID];

		int j = 0;
		for (size_t entityIndex = pool.firstEntity; entityIndex < pool.lastEntity; entityIndex++) {
			if (gameArchetypes_[i].HasComponent(ecs::ComponentType::COLLIDER) &&
				collidersManager.GetComponent(entityIndex) != gameArchetypes_[i].collider) {

				archetypesJson["entities"][j]["collider"][0] = collidersManager.GetJsonFromComponent(entityIndex);
			}

			if (gameArchetypes_[i].HasComponent(ecs::ComponentType::MODEL) &&
				modelsManager.GetComponent(entityIndex).materialID != gameArchetypes_[i].model.materialID &&
				modelsManager.GetComponent(entityIndex).meshID != gameArchetypes_[i].model.meshID) {

				archetypesJson["entities"][j]["model"] = modelsManager.GetJsonFromComponent(entityIndex);
			}

			if (gameArchetypes_[i].HasComponent(ecs::ComponentType::RIGIDBODY) &&
				rigidbodyManager.GetComponent(entityIndex).type != gameArchetypes_[i].rigidbody.type &&
				rigidbodyManager.GetComponent(entityIndex).angularDrag != gameArchetypes_[i].rigidbody.angularDrag &&
				rigidbodyManager.GetComponent(entityIndex).linearDrag != gameArchetypes_[i].rigidbody.linearDrag) {

				archetypesJson["entities"][j]["rigidbody"] = rigidbodyManager.GetJsonFromComponent(entityIndex);
			}

			if (gameArchetypes_[i].HasComponent(ecs::ComponentType::SPLINE_FOLLOWER) &&
				splinesManager.GetComponent(entityIndex).spline == gameArchetypes_[i].spline.spline &&
				splinesManager.GetComponent(entityIndex).speed == gameArchetypes_[i].spline.speed) {

				archetypesJson["entities"][j]["spline"] = splinesManager.GetJsonFromComponent(entityIndex);
			}

			if (gameArchetypes_[i].HasComponent(ecs::ComponentType::TRANSFORM) &&
				transformManager.GetComponent(entityIndex).GetLocalPosition() != gameArchetypes_[i].transform.GetLocalPosition() &&
				transformManager.GetComponent(entityIndex).GetLocalRotation() != gameArchetypes_[i].transform.GetLocalRotation() &&
				transformManager.GetComponent(entityIndex).GetLocalScale() != gameArchetypes_[i].transform.GetLocalScale()) {

				archetypesJson["entities"][j]["transform"] = transformManager.GetJsonFromComponent(entityIndex);
			}

			if (gameArchetypes_[i].HasComponent(ecs::ComponentType::LIGHT) &&
				lightsManager.GetComponent(entityIndex) != gameArchetypes_[i].light) {
				archetypesJson["entities"][j]["light"] = collidersManager.GetJsonFromComponent(entityIndex);
			}

			if (gameArchetypes_[i].HasComponent(ecs::ComponentType::AUDIO_SOURCE) &&
				audioSourcesManager.GetComponent(entityIndex) != gameArchetypes_[i].audioSource) {
				archetypesJson["entities"][j]["audioSource"] = audioSourcesManager.GetJsonFromComponent(entityIndex);
			}

			//--------------------------------- Game Components Part ---------------------------------------

            if(gameArchetypes_[i].HasComponent(ecs::ComponentType::PLAYER) &&
				playerManager.GetComponent(entityIndex) != gameArchetypes_[i].player) {
				archetypesJson["entities"][j]["player"] = playerManager.GetJsonFromComponent(entityIndex);
            }

            if(gameArchetypes_[i].HasComponent(ecs::ComponentType::DESTRUCTIBLE_ELEMENT) && 
				destructibleElementManager.GetComponent(entityIndex) != gameArchetypes_[i].destructibleElement) {
				archetypesJson["entities"][j]["destructibleElement"] =
					destructibleElementManager.GetJsonFromComponent(entityIndex);
            }

			if (gameArchetypes_[i].HasComponent(ecs::ComponentType::ENEMY) &&
				enemyManager.GetComponent(entityIndex) != gameArchetypes_[i].enemy) {
				archetypesJson["entities"][j]["enemy"] = enemyManager.GetJsonFromComponent(entityIndex);
			}

			if (gameArchetypes_[i].HasComponent(ecs::ComponentType::MISSILE) &&
				missileManager.GetComponent(entityIndex) != gameArchetypes_[i].missile) {
				archetypesJson["entities"][j]["missile"] = missileManager.GetJsonFromComponent(entityIndex);
			}

			if (gameArchetypes_[i].HasComponent(ecs::ComponentType::PROJECTILE) &&
				projectileManager.GetComponent(entityIndex) != gameArchetypes_[i].projectile) {
				archetypesJson["entities"][j]["projectile"] = projectileManager.GetJsonFromComponent(entityIndex);
			}

			if (gameArchetypes_[i].HasComponent(ecs::ComponentType::WEAPON) &&
				weaponManager.GetComponent(entityIndex) != gameArchetypes_[i].weapon) {
				archetypesJson["entities"][j]["weapon"] = weaponManager.GetJsonFromComponent(entityIndex);
			}

			if (gameArchetypes_[i].HasComponent(ecs::ComponentType::SPLINE_STATES) &&
				splineStateManager.GetComponent(entityIndex) != gameArchetypes_[i].splineStates) {
				archetypesJson["entities"][j]["splineStates"] = splineStateManager.GetJsonFromComponent(entityIndex);
			}

			if (gameArchetypes_[i].HasComponent(ecs::ComponentType::JIGGLE) &&
				jiggleManager.GetComponent(entityIndex) != gameArchetypes_[i].jiggle) {
				archetypesJson["entities"][j]["jiggle"] = jiggleManager.GetJsonFromComponent(entityIndex);
			}
			//-------------------------------------- End Game Part -------------------------------------------

			j++;
		}
	}

	return archetypesJson;
}

void GameArchetypesManager::UpdateArchetype(ecs::ArchetypeID archetypeID, const ecs::Archetype& archetype) {
	gameArchetypes_[archetypeID] = static_cast<const GameArchetype&>(archetype);
	EcsManagerLocator::Get().UpdateArchetype(archetypeID, archetype);
}

void GameArchetypesManager::ResizeArchetype(ecs::ArchetypeID archetypeID, size_t newSize) {
	gameArchetypes_[archetypeID].size = newSize;
	EcsManagerLocator::Get().ResizeArchetype(archetypeID, newSize, gameArchetypes_[archetypeID]);
}

void GameArchetypesManager::RenameArchetype(ecs::ArchetypeID archetypeID, const std::string& newName) {
	archetypesNames_[archetypeID] = newName;
}

ecs::EntityPool GameArchetypesManager::GetEntityPool(ecs::ArchetypeID archetypeID) {
	return EcsManagerLocator::Get().GetPools()[archetypeID];
}

const std::vector<std::string>& GameArchetypesManager::GetAllArchetypesNames() {
	return archetypesNames_;
}
}//namespace game
}//namespace poke