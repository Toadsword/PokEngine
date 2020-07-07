#include <Ecs/core_ecs_manager.h>

#include <Game/Ecs/game_ecs_manager.h>
#include <CoreEngine/engine.h>

#include <Game/ResourcesManager/PrefabsManager/game_prefabs_manager.h>

#include <Game/ComponentManagers/player_manager.h>
#include <Game/ComponentManagers/enemies_manager.h>
#include <Game/ComponentManagers/destructible_element_manager.h>
#include <Game/ComponentManagers/weapon_manager.h>
#include <Game/ComponentManagers/projectile_manager.h>
#include <Game/ComponentManagers/missiles_manager.h>
#include <Game/ComponentManagers/spline_state_manager.h>
#include <Game/ComponentManagers/special_attack_manager.h>
#include <Game/ComponentManagers/jiggle_manager.h>
#include <Game/ComponentManagers/game_camera_manager.h>

namespace poke::game {

GameEcsManager::GameEcsManager(Engine& engine, const size_t defaultPoolSize)
    : CoreEcsManager(engine, defaultPoolSize)
{
	
}

std::map<ecs::EntityIndex, ecs::EntityIndex> GameEcsManager::SetEntitiesFromJson(const json& entitiesJson)
{
	// ------------ Engine components ------------ //
	auto& modelsManager = componentsManagersContainer_.GetComponentsManager< ecs::ModelsManager>();
	auto& rigidbodyManager = componentsManagersContainer_.GetComponentsManager<ecs::RigidbodyManager>();
	auto& transformManager = componentsManagersContainer_.GetComponentsManager<ecs::TransformsManager>();
	auto& collidersManager = componentsManagersContainer_.GetComponentsManager<ecs::CollidersManager>();
	auto& splinesManager = componentsManagersContainer_.GetComponentsManager<ecs::SplineFollowersManager>();
	auto& lightsManager = componentsManagersContainer_.GetComponentsManager<ecs::LightsManager>();
	auto& particleSystemManager = componentsManagersContainer_.GetComponentsManager<ecs::ParticleSystemsManager>();
	auto& audioSourcesManager = componentsManagersContainer_.GetComponentsManager<ecs::AudioSourcesManager>();
	auto& trailRendererManager = componentsManagersContainer_.GetComponentsManager<ecs::TrailRendererManager>();
	auto& segmentRendererManager = componentsManagersContainer_.GetComponentsManager<ecs::SegmentRendererManager>();
	// ------------ !Engine components ------------ //

	// ------------ Game components ------------ //
	auto& enemyManager = componentsManagersContainer_.GetComponentsManager<game::EnemiesManager>();
	auto& playerManager = componentsManagersContainer_.GetComponentsManager<game::PlayerManager>();
	auto& destructibleElementManager = componentsManagersContainer_.GetComponentsManager<game::DestructibleElementManager>();
	auto& weaponManager = componentsManagersContainer_.GetComponentsManager<game::WeaponManager>();
	auto& projectileManager = componentsManagersContainer_.GetComponentsManager<game::ProjectileManager>();
	auto& missilesManager = componentsManagersContainer_.GetComponentsManager<game::MissilesManager>();
	auto& splineStateManager = componentsManagersContainer_.GetComponentsManager<game::SplineStateManager>();
	auto& specialAttackManager = componentsManagersContainer_.GetComponentsManager<game::SpecialAttackManager>();
	auto& jiggleManager = componentsManagersContainer_.GetComponentsManager<game::JiggleManager>();
	auto& gameCameraManager = componentsManagersContainer_.GetComponentsManager<game::GameCameraManager>();
	// ------------ !Game components ------------ //

	int parentOffset = 0;
	int sceneEntityId = 0;
	ecs::EntityIndex lastGeneratedEntityIndex = -1;
	std::map<ecs::EntityIndex, ecs::EntityIndex> baseParentIdToOffsetParentId;
	std::map<ecs::EntityIndex, ecs::EntityIndex> sceneIdToEntityId;
	for (json entityJson : entitiesJson) {
		ecs::EntityIndex entityIndex;
		if (CheckJsonExists(entityJson, "prefabName") &&
			CheckJsonParameter(entityJson, "prefabName", nlohmann::detail::value_t::string)) {

			const std::string nameString = entityJson["prefabName"].get<std::string>();

		    // <Temporary>
			std::vector<ecs::EntityIndex> instantiatedObjects;
			/******************************** Game prefab construction ********************************/
			if (nameString == "Player")
				instantiatedObjects = reinterpret_cast<game::GamePrefabsManager&>(poke::PrefabsManagerLocator::Get()).InstantiatePlayer();
			else if (nameString == "LightFighter")
				instantiatedObjects = reinterpret_cast<game::GamePrefabsManager&>(poke::PrefabsManagerLocator::Get()).InstantiateLightFighter();
			else if (nameString == "Destroyer")
				instantiatedObjects = reinterpret_cast<game::GamePrefabsManager&>(poke::PrefabsManagerLocator::Get()).InstantiateDestroyer();
			else
				instantiatedObjects = PrefabsManagerLocator::Get().Instantiate(nameString);
			/******************************************************************************************/
			// </Temporary>
			entityIndex = instantiatedObjects[0];

			parentOffset += entityIndex - lastGeneratedEntityIndex - 1;

			baseParentIdToOffsetParentId[entityIndex] = entityIndex + parentOffset;

			parentOffset += instantiatedObjects.size() - 1;
			lastGeneratedEntityIndex = instantiatedObjects[instantiatedObjects.size() - 1];

			for (int i = entityIndex - parentOffset; i < baseParentIdToOffsetParentId.size(); i++) {
				const int currentParent = baseParentIdToOffsetParentId.at(i);
				if (currentParent >= entityIndex) {
					baseParentIdToOffsetParentId[i] += instantiatedObjects.size() - 1;
				}
			}

			//Save name and prefabName
			for (int i = 1; i < instantiatedObjects.size(); i++) {
				baseParentIdToOffsetParentId[instantiatedObjects[i]] = instantiatedObjects[i] + parentOffset;
			}
		}
		else {
			entityIndex = AddEntity();
			parentOffset += entityIndex - lastGeneratedEntityIndex - 1;
			lastGeneratedEntityIndex = entityIndex;
			baseParentIdToOffsetParentId[entityIndex] = entityIndex + parentOffset;

			if (CheckJsonExists(entityJson, "collider") &&
				CheckJsonParameter(entityJson, "collider", nlohmann::detail::value_t::object)) {
				collidersManager.SetComponentFromJson(entityIndex, entityJson["collider"]);
				AddComponent(entityIndex, ecs::ComponentType::ComponentType::COLLIDER);
			}
			if (CheckJsonExists(entityJson, "model") &&
				CheckJsonParameter(entityJson, "model", nlohmann::detail::value_t::object)) {
				modelsManager.SetComponentFromJson(entityIndex, entityJson["model"]);
				AddComponent(entityIndex, ecs::ComponentType::ComponentType::MODEL);
			}
			if (CheckJsonExists(entityJson, "rigidbody") &&
				CheckJsonParameter(entityJson, "rigidbody", nlohmann::detail::value_t::object)) {
				rigidbodyManager.SetComponentFromJson(entityIndex, entityJson["rigidbody"]);
				AddComponent(entityIndex, ecs::ComponentType::ComponentType::RIGIDBODY);
			}
			if (CheckJsonExists(entityJson, "light") &&
				CheckJsonParameter(entityJson, "light", nlohmann::detail::value_t::object)) {
				lightsManager.SetComponentFromJson(entityIndex, entityJson["light"]);
				AddComponent(entityIndex, ecs::ComponentType::ComponentType::LIGHT);
			}
			if (CheckJsonExists(entityJson, "particleSystem") &&
				CheckJsonParameter(entityJson, "particleSystem", nlohmann::detail::value_t::object)) {
				particleSystemManager.SetComponentFromJson(entityIndex, entityJson["particleSystem"]);
				AddComponent(entityIndex, ecs::ComponentType::ComponentType::PARTICLE_SYSTEM);
			}
			if (CheckJsonExists(entityJson, "audioSource") &&
				CheckJsonParameter(entityJson, "audioSource", nlohmann::detail::value_t::object)) {

				audioSourcesManager.SetComponentFromJson(entityIndex, entityJson["audioSource"]);
				AddComponent(entityIndex, ecs::ComponentType::ComponentType::AUDIO_SOURCE);
			}
			if (CheckJsonExists(entityJson, "trailRenderer") &&
				CheckJsonParameter(entityJson, "trailRenderer", nlohmann::detail::value_t::object)) {

				trailRendererManager.SetComponentFromJson(entityIndex, entityJson["trailRenderer"]);
				AddComponent(entityIndex, ecs::ComponentType::ComponentType::TRAIL_RENDERER);
			}
			if (CheckJsonExists(entityJson, "segmentRenderer") &&
				CheckJsonParameter(entityJson, "segmentRenderer", nlohmann::detail::value_t::object)) {

				segmentRendererManager.SetComponentFromJson(entityIndex, entityJson["segmentRenderer"]);
				AddComponent(entityIndex, ecs::ComponentType::ComponentType::SEGMENT_RENDERER);
			}

			// ------------ !Engine components ------------ //

			if (CheckJsonExists(entityJson, "enemy") &&
				CheckJsonParameter(entityJson, "enemy", nlohmann::detail::value_t::object)) {

				enemyManager.SetComponentFromJson(entityIndex, entityJson["enemy"]);
				AddComponent(entityIndex, ecs::ComponentType::ComponentType::ENEMY);
			}
			if (CheckJsonExists(entityJson, "player") &&
				CheckJsonParameter(entityJson, "player", nlohmann::detail::value_t::object)) {

				playerManager.SetComponentFromJson(entityIndex, entityJson["player"]);
				AddComponent(entityIndex, ecs::ComponentType::ComponentType::PLAYER);
			}
			if (CheckJsonExists(entityJson, "destructibleElement") &&
				CheckJsonParameter(entityJson, "destructibleElement", nlohmann::detail::value_t::object)) {

				destructibleElementManager.SetComponentFromJson(entityIndex, entityJson["destructibleElement"]);
				AddComponent(entityIndex, ecs::ComponentType::ComponentType::DESTRUCTIBLE_ELEMENT);
			}
			if (CheckJsonExists(entityJson, "weapon") &&
				CheckJsonParameter(entityJson, "weapon", nlohmann::detail::value_t::object)) {

				weaponManager.SetComponentFromJson(entityIndex, entityJson["weapon"]);
				AddComponent(entityIndex, ecs::ComponentType::ComponentType::WEAPON);
			}
			if (CheckJsonExists(entityJson, "projectile") &&
				CheckJsonParameter(entityJson, "projectile", nlohmann::detail::value_t::object)) {

				projectileManager.SetComponentFromJson(entityIndex, entityJson["projectile"]);
				AddComponent(entityIndex, ecs::ComponentType::ComponentType::PROJECTILE);
			}
			if (CheckJsonExists(entityJson, "missile") &&
				CheckJsonParameter(entityJson, "missile", nlohmann::detail::value_t::object)) {

				missilesManager.SetComponentFromJson(entityIndex, entityJson["missile"]);
				AddComponent(entityIndex, ecs::ComponentType::ComponentType::MISSILE);
			}
			if (CheckJsonExists(entityJson, "splineStates") &&
				CheckJsonParameter(entityJson, "splineStates", nlohmann::detail::value_t::object)) {

				splineStateManager.SetComponentFromJson(entityIndex, entityJson["splineStates"]);
				AddComponent(entityIndex, ecs::ComponentType::ComponentType::SPLINE_STATES);
			}
			if (CheckJsonExists(entityJson, "specialAttack") &&
				CheckJsonParameter(entityJson, "specialAttack", nlohmann::detail::value_t::object)) {

				specialAttackManager.SetComponentFromJson(entityIndex, entityJson["specialAttack"]);
				AddComponent(entityIndex, ecs::ComponentType::ComponentType::SPECIAL_ATTACK);
			}
			if (CheckJsonExists(entityJson, "jiggle") &&
				CheckJsonParameter(entityJson, "jiggle", nlohmann::detail::value_t::object)) {

				jiggleManager.SetComponentFromJson(entityIndex, entityJson["jiggle"]);
				AddComponent(entityIndex, ecs::ComponentType::ComponentType::JIGGLE);
			}
			if (CheckJsonExists(entityJson, "gameCamera") &&
				CheckJsonParameter(entityJson, "gameCamera", nlohmann::detail::value_t::object)) {

				gameCameraManager.SetComponentFromJson(entityIndex, entityJson["gameCamera"]);
				AddComponent(entityIndex, ecs::ComponentType::ComponentType::GAME_CAMERA);
			}
			// !-------------------- Game components -------------------! //
		}

		//Generic parameters
		if (CheckJsonExists(entityJson, "transform") &&
			CheckJsonParameter(entityJson, "transform", nlohmann::detail::value_t::object))
		{
			//Update parent with offset
			json transformJson = entityJson["transform"];
			if (CheckJsonExists(transformJson, "parent"))
			{
				const ecs::EntityIndex parentId = transformJson["parent"];
				if (baseParentIdToOffsetParentId.find(parentId) != baseParentIdToOffsetParentId.end()) {
					transformJson["parent"] = baseParentIdToOffsetParentId.find(parentId)->second;
				}
			}
			transformManager.SetComponentFromJson(entityIndex, transformJson);

			AddComponent(entityIndex, ecs::ComponentType::ComponentType::TRANSFORM);
		}
		if (CheckJsonExists(entityJson, "tag") &&
			CheckJsonNumber(entityJson, "tag")) {
			SetTag(entityIndex, entityJson["tag"]);
		}
		if (CheckJsonExists(entityJson, "isActive") &&
			CheckJsonParameter(entityJson, "isActive", nlohmann::detail::value_t::boolean)) {

			if (!entityJson["isActive"]) {
				SetActive(
					entityIndex,
					ecs::EntityStatus::INACTIVE);
			}
		}

		if (CheckJsonExists(entityJson, "spline") &&
			CheckJsonParameter(entityJson, "spline", nlohmann::detail::value_t::object)) {
			splinesManager.SetComponentFromJson(entityIndex, entityJson["spline"]);
			AddComponent(entityIndex, ecs::ComponentType::ComponentType::SPLINE_FOLLOWER);
		}

		sceneIdToEntityId[sceneEntityId] = entityIndex;
		sceneEntityId++;
	}

	return sceneIdToEntityId;
}

std::tuple<json, std::map<ecs::EntityIndex, ecs::EntityIndex>> GameEcsManager::EntitiesToJson()
{
	cassert(false, "Game Ecs Manager shouldn't be saving scenes.");
	return std::make_tuple(json(), std::map<ecs::EntityIndex, ecs::EntityIndex>());
}

} //namespace poke::ecs
