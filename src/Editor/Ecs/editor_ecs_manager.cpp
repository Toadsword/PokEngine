#include <Editor/Ecs/editor_ecs_manager.h>

#include <CoreEngine/ServiceLocator/service_locator_definition.h>
#include <Utility/log.h>
#include <Ecs/Prefabs/prefab.h>
#include <Utility/chronometer.h>
#include <CoreEngine/engine.h>
#include <Game/game.h>
#include <Game/ResourcesManager/PrefabsManager/game_prefabs_manager.h>
#include <Editor/ComponentManagers/editor_component_manager.h>
#include <Ecs/ComponentManagers/trail_renderer_manager.h>

namespace poke {
namespace editor {

EditorEcsManager::EditorEcsManager(Engine& engine, game::Game& game, const size_t defaultPoolSize)
    : GameEcsManager(engine, defaultPoolSize),
	  gameArchetypesManager_(game.GetGameArchetypesManager())
{

	componentsManagersContainer_.RegisterManager(ecs::EditorComponentManager());

	// ----------- Engine Component ------------//
    copyComponentsManagersContainer_.RegisterManager(ecs::TransformsManager());
    copyComponentsManagersContainer_.RegisterManager(ecs::RigidbodyManager());
    copyComponentsManagersContainer_.RegisterManager(ecs::CollidersManager());
    copyComponentsManagersContainer_.RegisterManager(ecs::ModelsManager());
    copyComponentsManagersContainer_.RegisterManager(ecs::SplineFollowersManager());
    copyComponentsManagersContainer_.RegisterManager(ecs::LightsManager());
    copyComponentsManagersContainer_.RegisterManager(ecs::AudioSourcesManager());
    copyComponentsManagersContainer_.RegisterManager(ecs::ParticleSystemsManager());
    copyComponentsManagersContainer_.RegisterManager(ecs::EditorComponentManager());
	copyComponentsManagersContainer_.RegisterManager(ecs::TrailRendererManager());
	// ----------- !Engine Component ------------//

	// ----------- Game Component ------------//
	copyComponentsManagersContainer_.RegisterManager(game::EnemiesManager());
	copyComponentsManagersContainer_.RegisterManager(game::PlayerManager());
	copyComponentsManagersContainer_.RegisterManager(game::DestructibleElementManager());
	copyComponentsManagersContainer_.RegisterManager(game::WeaponManager());
	copyComponentsManagersContainer_.RegisterManager(game::ProjectileManager());
	copyComponentsManagersContainer_.RegisterManager(game::MissilesManager());
	// ----------- !Game Component ------------//
}

ecs::EntityIndex EditorEcsManager::AddEntity(const ecs::ArchetypeID archetypeID)
{
    const auto entityIndex = CoreEcsManager::AddEntity(archetypeID);

    //Add basic name to entity
	AddComponent(entityIndex, ecs::ComponentType::ComponentType::EDITOR_COMPONENT);

	auto& editorComponentManager = GetComponentsManager<ecs::EditorComponentManager>();
	ecs::EditorComponent editorComponent = editorComponentManager.GetComponent(entityIndex);
	editorComponent.name = "Entity " + std::to_string(entityIndex);
	editorComponentManager.SetComponent(entityIndex, editorComponent);

    return entityIndex;
}

void EditorEcsManager::AddArchetype(
	const ecs::Archetype& archetype,
	const unsigned int sizeNeeded)
{
	//Create new available pool
    ecs::EntityPool newPool;

	newPool.firstEntity = pools_.back().lastEntity + 1;
	newPool.lastEntity = newPool.firstEntity + sizeNeeded;
	newPool.nextFreeEntity = newPool.firstEntity;
    
	//Make the number of entities bigger.
	AllocatePoolMemory(newPool.lastEntity - newPool.firstEntity + 1);

	const std::vector<std::string>& archetypesNames = gameArchetypesManager_.GetAllArchetypesNames();
	const std::vector<game::GameArchetype>& gameArchetypes = gameArchetypesManager_.GetAllGameArchetype();

	int index = 0;
    for(const game::GameArchetype& gameArchetype : gameArchetypes) {
		if(gameArchetype == archetype) {
			break;
		}
		index++;
    }
	const std::string& name = archetypesNames[index];

	//Set the component mask for every entity
	auto& editorComponentManager = GetComponentsManager<ecs::EditorComponentManager>();
	for (auto entityIndex = newPool.firstEntity;
		entityIndex < newPool.lastEntity; entityIndex++) {
		AddComponent(entityIndex, archetype.GetComponentMask());

		ecs::EditorComponent editorComponent = editorComponentManager.GetComponent(entityIndex);
		editorComponent.name = "Arch. " + name + " : " + std::to_string(entityIndex);
		editorComponentManager.SetComponent(entityIndex, editorComponent);

		AddComponent(entityIndex, ecs::ComponentType::EDITOR_COMPONENT);

		SetActive(entityIndex, ecs::EntityStatus::INACTIVE);
		SetEntityVisible(entityIndex, ecs::EntityStatus::INACTIVE);
	}

	componentsManagersContainer_.SetWithArchetype(
		newPool,
		archetype);

	pools_.push_back(newPool);
}

std::map<ecs::EntityIndex, ecs::EntityIndex> EditorEcsManager::SetEntitiesFromJson(const json& entitiesJson)
{
	Chronometer chronometer = Chronometer();
	chronometer.Restart();

	// ------------ Engine components ------------ //
	auto& modelsManager = componentsManagersContainer_.GetComponentsManager< ecs::ModelsManager>();
	auto& rigidbodyManager = componentsManagersContainer_.GetComponentsManager<ecs::RigidbodyManager>();
	auto& transformManager = componentsManagersContainer_.GetComponentsManager<ecs::TransformsManager>();
	auto& collidersManager = componentsManagersContainer_.GetComponentsManager<ecs::CollidersManager>();
	auto& splinesManager = componentsManagersContainer_.GetComponentsManager<ecs::SplineFollowersManager>();
	auto& lightsManager = componentsManagersContainer_.GetComponentsManager<ecs::LightsManager>();
	auto& particleSystemManager = componentsManagersContainer_.GetComponentsManager<ecs::ParticleSystemsManager>();
    auto& audioSourcesManager = componentsManagersContainer_.GetComponentsManager<ecs::AudioSourcesManager>();
    auto& editorComponentManager = componentsManagersContainer_.GetComponentsManager<ecs::EditorComponentManager>();
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
                if(currentParent >= entityIndex) {
					baseParentIdToOffsetParentId[i] += instantiatedObjects.size() - 1;
                }
            }

			//Save name and prefabName
            for(int i = 1; i < instantiatedObjects.size(); i++) {
				baseParentIdToOffsetParentId[instantiatedObjects[i]] = instantiatedObjects[i] + parentOffset;
            }

            //Save name and prefabName
			editorComponentManager.SetComponentFromJson(entityIndex, entityJson);
			AddComponent(entityIndex, ecs::ComponentType::ComponentType::EDITOR_COMPONENT);
		}
		else {
			entityIndex = AddEntity();
			parentOffset += entityIndex - lastGeneratedEntityIndex - 1;
			lastGeneratedEntityIndex = entityIndex;
			baseParentIdToOffsetParentId[entityIndex] = entityIndex + parentOffset;
			// ------------ Engine components ------------ //
			if (CheckJsonExists(entityJson, "name") &&
				CheckJsonParameter(entityJson, "name", nlohmann::detail::value_t::string)) {

				//Save name and prefabName
				editorComponentManager.SetComponentFromJson(entityIndex, entityJson);
				AddComponent(entityIndex, ecs::ComponentType::ComponentType::EDITOR_COMPONENT);
			}
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
	const double deltaTime = chronometer.GetTimeMilli();
	LogDebug("Load scene time : " + std::to_string(deltaTime), LogType::SCENE_LOG);

	return sceneIdToEntityId;
}

std::tuple<json, std::map<ecs::EntityIndex, ecs::EntityIndex>> EditorEcsManager::EntitiesToJson()
{
    auto& modelsManager = componentsManagersContainer_.GetComponentsManager<ecs::ModelsManager>();
    auto& rigidbodyManager = componentsManagersContainer_.GetComponentsManager<ecs::RigidbodyManager>();
    auto& transformsManager = componentsManagersContainer_.GetComponentsManager<ecs::TransformsManager>();
    auto& collidersManager = componentsManagersContainer_.GetComponentsManager<ecs::CollidersManager>();
    auto& splinesManager = componentsManagersContainer_.GetComponentsManager<ecs::SplineFollowersManager>();
    auto& lightsManager = componentsManagersContainer_.GetComponentsManager<ecs::LightsManager>();
	auto& particleSystemManager = componentsManagersContainer_.GetComponentsManager<ecs::ParticleSystemsManager>();
    auto& audioSourcesManager = componentsManagersContainer_.GetComponentsManager<ecs::AudioSourcesManager>();
	auto& editorComponent = componentsManagersContainer_.GetComponentsManager<ecs::EditorComponentManager>();
	auto& trailRendererManager = componentsManagersContainer_.GetComponentsManager<ecs::TrailRendererManager>();
	auto& segmentRendererManager = componentsManagersContainer_.GetComponentsManager<ecs::SegmentRendererManager>();

	// Game components //
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

    json outputJson;

    //We want to save only the first pool of entities
	ecs::EntityPool entitiesPool = GetPools()[0];
	const std::vector<ecs::EntityIndex>& entities = GetNotEmptyEntities();
	std::vector<ecs::EntityIndex> parentsToIgnore;
	std::map<ecs::EntityIndex, ecs::EntityIndex> baseParentIdTonewParentId;
	std::map<ecs::EntityIndex, ecs::EntityIndex> oldToSavedEntityId;
	int indexSave = 0;
	int numSkippedEntities = 0;
    for (ecs::EntityIndex entity : entities) {
        //We ignore all the entities that are not in the pool
        if(entity < entitiesPool.firstEntity || 
			entity > entitiesPool.lastEntity) {
			continue;
        }
		bool isPrefab = false;
		bool eligibleForSaving = false;
        
		//Transform
		if (HasComponent(entity, ecs::ComponentType::ComponentType::TRANSFORM)) {
			json transformOutputJson = transformsManager.GetJsonFromComponent(entity);
			const int parentId = transformOutputJson["parent"];
			auto it = std::find(parentsToIgnore.begin(), parentsToIgnore.end(), parentId);
			// Ignore all children of potential prefabs
			if (it == parentsToIgnore.end()) {
				//Evaluate parent delta
				if (parentId != -1) {
					if (baseParentIdTonewParentId.find(parentId) == baseParentIdTonewParentId.end()) {
						transformOutputJson["parent"] = parentId - numSkippedEntities;
						baseParentIdTonewParentId[parentId] = parentId - numSkippedEntities;
					}
					else {
						transformOutputJson["parent"] = baseParentIdTonewParentId.at(parentId);
					}
				}

				outputJson[indexSave]["transform"] = transformOutputJson;
				eligibleForSaving = true;
			}
			else {
				parentsToIgnore.push_back(entity);
				numSkippedEntities++;
				continue;
			}
		}

        //Names
        if(HasComponent(entity, ecs::ComponentType::ComponentType::EDITOR_COMPONENT)) {
            json editorComponentJson = editorComponent.GetJsonFromComponent(entity);
			outputJson[indexSave]["name"] = editorComponentJson["name"];
			std::string prefabName = editorComponentJson["prefabName"];
            //If it's a prefab
            if(!prefabName.empty()) {
				isPrefab = true;
			    outputJson[indexSave]["prefabName"] = editorComponentJson["prefabName"];
				outputJson[indexSave]["isActive"] = entities_[entity].IsActive();
				outputJson[indexSave]["tag"] = GetTag(entity);

                // Ignore all children of the prefab
				parentsToIgnore.push_back(entity);
            }
        }

		//Spline
		if (HasComponent(entity, ecs::ComponentType::ComponentType::SPLINE_FOLLOWER)) {
			outputJson[indexSave]["spline"] = splinesManager.GetJsonFromComponent(entity);
			eligibleForSaving = true;
		}

        if(isPrefab) {
			oldToSavedEntityId[entity] = indexSave;
			indexSave++;
			continue;
        }

        //Model
        if (HasComponent(entity, ecs::ComponentType::ComponentType::MODEL)) {
			outputJson[indexSave]["model"] = modelsManager.GetJsonFromComponent(entity);
			eligibleForSaving = true;
        }

 
        //Rigidbody
        if (HasComponent(entity, ecs::ComponentType::ComponentType::RIGIDBODY)) {
			outputJson[indexSave]["rigidbody"] = rigidbodyManager.GetJsonFromComponent(entity);
			eligibleForSaving = true;
        }

        //Collider
        if (HasComponent(entity, ecs::ComponentType::ComponentType::COLLIDER)) {
			outputJson[indexSave]["collider"] = collidersManager.GetJsonFromComponent(entity);
			eligibleForSaving = true;
        }

        //Light
        if (HasComponent(entity, ecs::ComponentType::ComponentType::LIGHT)) {
			outputJson[indexSave]["light"] = lightsManager.GetJsonFromComponent(entity);
			eligibleForSaving = true;
        }

		//Particle system
		if (HasComponent(entity, ecs::ComponentType::ComponentType::PARTICLE_SYSTEM)) {
			outputJson[indexSave]["particleSystem"] = particleSystemManager.GetJsonFromComponent(entity);
			eligibleForSaving = true;
		}

        //Audio source
        if (HasComponent(entity, ecs::ComponentType::ComponentType::AUDIO_SOURCE)) {
			outputJson[indexSave]["audioSource"] = audioSourcesManager.GetJsonFromComponent(entity);
			eligibleForSaving = true;
        }

		//Trail renderer
		if (HasComponent(entity, ecs::ComponentType::ComponentType::TRAIL_RENDERER)) {
			outputJson[indexSave]["trailRenderer"] = trailRendererManager.GetJsonFromComponent(entity);
			eligibleForSaving = true;
		}

		//Segment renderer
		if (HasComponent(entity, ecs::ComponentType::ComponentType::SEGMENT_RENDERER)) {
			outputJson[indexSave]["segmentRenderer"] = segmentRendererManager.GetJsonFromComponent(entity);
			eligibleForSaving = true;
		}

        // ------------- Game components ------------ //
        //Enemy
		if (HasComponent(entity, ecs::ComponentType::ComponentType::ENEMY)) {
			outputJson[indexSave]["enemy"] = enemyManager.GetJsonFromComponent(entity);
			eligibleForSaving = true;
		}

		//Player
		if (HasComponent(entity, ecs::ComponentType::ComponentType::PLAYER)) {
			outputJson[indexSave]["player"] = playerManager.GetJsonFromComponent(entity);
			eligibleForSaving = true;
		}

		//DestructibleElement
		if (HasComponent(entity, ecs::ComponentType::ComponentType::DESTRUCTIBLE_ELEMENT)) {
			outputJson[indexSave]["destructibleElement"] = destructibleElementManager.GetJsonFromComponent(entity);
			eligibleForSaving = true;
		}

		//Weapon
		if (HasComponent(entity, ecs::ComponentType::ComponentType::WEAPON)) {
			outputJson[indexSave]["weapon"] = weaponManager.GetJsonFromComponent(entity);
			eligibleForSaving = true;
		}

		//Projectile
		if (HasComponent(entity, ecs::ComponentType::ComponentType::PROJECTILE)) {
			outputJson[indexSave]["projectile"] = projectileManager.GetJsonFromComponent(entity);
			eligibleForSaving = true;
		}

		//Missile
		if (HasComponent(entity, ecs::ComponentType::ComponentType::MISSILE)) {
			outputJson[indexSave]["missile"] = missilesManager.GetJsonFromComponent(entity);
			eligibleForSaving = true;
		}

		if (HasComponent(entity, ecs::ComponentType::ComponentType::SPLINE_STATES)) {
			outputJson[indexSave]["splineStates"] = splineStateManager.GetJsonFromComponent(entity);
			eligibleForSaving = true;
		}

		if (HasComponent(entity, ecs::ComponentType::ComponentType::SPECIAL_ATTACK)) {
			outputJson[indexSave]["specialattack"] = specialAttackManager.GetJsonFromComponent(entity);
			eligibleForSaving = true;
		}

		if (HasComponent(entity, ecs::ComponentType::ComponentType::JIGGLE)) {
			outputJson[indexSave]["jiggle"] = jiggleManager.GetJsonFromComponent(entity);
			eligibleForSaving = true;
		}

		if (HasComponent(entity, ecs::ComponentType::ComponentType::GAME_CAMERA)) {
			outputJson[indexSave]["gameCamera"] = gameCameraManager.GetJsonFromComponent(entity);
			eligibleForSaving = true;
		}

        if(eligibleForSaving) {
			outputJson[indexSave]["isActive"] = entities_[entity].IsActive();
			outputJson[indexSave]["tag"] = GetTag(entity);
			oldToSavedEntityId[entity] = indexSave;
			indexSave++;
        }
    }

    return std::make_tuple(outputJson, oldToSavedEntityId);
}

const std::string EditorEcsManager::GetEntityName(ecs::EntityIndex entityIndex)
{
	ecs::EditorComponentManager& editorComponentManager = componentsManagersContainer_.GetComponentsManager<ecs::EditorComponentManager>();
	return editorComponentManager.GetComponent(entityIndex).name;
}

void EditorEcsManager::ReloadState()
{
	// ----------- Engine Component ------------//
    componentsManagersContainer_.GetComponentsManager<ecs::TransformsManager>() = 
		copyComponentsManagersContainer_.GetComponentsManager<ecs::TransformsManager>();

    componentsManagersContainer_.GetComponentsManager<ecs::RigidbodyManager>() = 
		copyComponentsManagersContainer_.GetComponentsManager<ecs::RigidbodyManager>();

    componentsManagersContainer_.GetComponentsManager<ecs::CollidersManager>() = 
		copyComponentsManagersContainer_.GetComponentsManager<ecs::CollidersManager>();

    componentsManagersContainer_.GetComponentsManager<ecs::SplineFollowersManager>() =
        copyComponentsManagersContainer_.GetComponentsManager<ecs::SplineFollowersManager>();

	componentsManagersContainer_.GetComponentsManager<ecs::EditorComponentManager>() = 
		copyComponentsManagersContainer_.GetComponentsManager<ecs::EditorComponentManager>();

    componentsManagersContainer_.GetComponentsManager<ecs::ModelsManager>() =
		copyComponentsManagersContainer_.GetComponentsManager<ecs::ModelsManager>();

    componentsManagersContainer_.GetComponentsManager<ecs::LightsManager>() = 
		copyComponentsManagersContainer_.GetComponentsManager<ecs::LightsManager>();

    componentsManagersContainer_.GetComponentsManager<ecs::AudioSourcesManager>() = 
		copyComponentsManagersContainer_.GetComponentsManager<ecs::AudioSourcesManager>();

	componentsManagersContainer_.GetComponentsManager<ecs::TrailRendererManager>() = 
		copyComponentsManagersContainer_.GetComponentsManager<ecs::TrailRendererManager>();

    componentsManagersContainer_.GetComponentsManager<ecs::ParticleSystemsManager>() = 
		copyComponentsManagersContainer_.GetComponentsManager<ecs::ParticleSystemsManager>();
	// ----------- !Engine Component ------------//
    
    // ----------- Game Component ------------//
	componentsManagersContainer_.GetComponentsManager<game::EnemiesManager>() =
		copyComponentsManagersContainer_.GetComponentsManager<game::EnemiesManager>();

	componentsManagersContainer_.GetComponentsManager<game::PlayerManager>() =
		copyComponentsManagersContainer_.GetComponentsManager<game::PlayerManager>();

	componentsManagersContainer_.GetComponentsManager<game::DestructibleElementManager>() =
		copyComponentsManagersContainer_.GetComponentsManager<game::DestructibleElementManager>();

	componentsManagersContainer_.GetComponentsManager<game::WeaponManager>() =
		copyComponentsManagersContainer_.GetComponentsManager<game::WeaponManager>();

	componentsManagersContainer_.GetComponentsManager<game::ProjectileManager>() =
		copyComponentsManagersContainer_.GetComponentsManager<game::ProjectileManager>();

	componentsManagersContainer_.GetComponentsManager<game::MissilesManager>() =
		copyComponentsManagersContainer_.GetComponentsManager<game::MissilesManager>();
	// ----------- !Game Component ------------//


	copyComponentsManagersContainer_
		.GetComponentsManager<ecs::TrailRendererManager>(
			).ResizeEntities(0);

	// ----------- Engine Component ------------//
    copyComponentsManagersContainer_.GetComponentsManager<ecs::TransformsManager>().ResizeEntities(0);
    copyComponentsManagersContainer_.GetComponentsManager<ecs::RigidbodyManager>().ResizeEntities(0);
    copyComponentsManagersContainer_.GetComponentsManager<ecs::ParticleSystemsManager>().ResizeEntities(0);
    copyComponentsManagersContainer_.GetComponentsManager<ecs::CollidersManager>().ResizeEntities(0);
    copyComponentsManagersContainer_.GetComponentsManager<ecs::SplineFollowersManager>().ResizeEntities(0);
    copyComponentsManagersContainer_.GetComponentsManager<ecs::ModelsManager>().ResizeEntities(0);
    copyComponentsManagersContainer_.GetComponentsManager<ecs::LightsManager>().ResizeEntities(0);
    copyComponentsManagersContainer_.GetComponentsManager<ecs::AudioSourcesManager>().ResizeEntities(0);
	copyComponentsManagersContainer_.GetComponentsManager<ecs::EditorComponentManager>().ResizeEntities(0);
    copyComponentsManagersContainer_.GetComponentsManager<ecs::ParticleSystemsManager>().ResizeEntities(0);
	// ----------- !Engine Component ------------//

	// ----------- Game Component ------------//
	copyComponentsManagersContainer_.GetComponentsManager<game::EnemiesManager>().ResizeEntities(0);
	copyComponentsManagersContainer_.GetComponentsManager<game::PlayerManager>().ResizeEntities(0);
	copyComponentsManagersContainer_.GetComponentsManager<game::DestructibleElementManager>().ResizeEntities(0);
	copyComponentsManagersContainer_.GetComponentsManager<game::WeaponManager>().ResizeEntities(0);
	copyComponentsManagersContainer_.GetComponentsManager<game::ProjectileManager>().ResizeEntities(0);
	copyComponentsManagersContainer_.GetComponentsManager<game::MissilesManager>().ResizeEntities(0);
	// ----------- !Game Component ------------//

    subjectAddComponent_ = copySubjectAddComponent_;
    subjectRemoveComponent_ = copySubjectRemoveComponent_;
    subjectsTriggerEnter_ = copySubjectsTriggerEnter_;
    subjectsTriggerExit_ = copySubjectsTriggerExit_;
    subjectsColliderEnter_ = copySubjectsColliderEnter_;
    subjectsColliderExit_ = copySubjectsColliderExit_;

    //Remove new component, add previous component
    for (ecs::EntityIndex i = 0; i < entities_.size(); i++) {
        RemoveComponent(i, entities_[i].GetComponentMask());
        entities_[i].Clear();
    }
    entities_ = copyEntities_;
    for (ecs::EntityIndex i = 0; i < entities_.size(); i++) {
        AddComponent(i, entities_[i].GetComponentMask());
    }
}

void EditorEcsManager::SetEntityName(ecs::EntityIndex entityIndex, const std::string & name)
{
	auto& editorComponentManager = componentsManagersContainer_.GetComponentsManager<ecs::EditorComponentManager>();
	if (!HasComponent(entityIndex, ecs::ComponentType::ComponentType::EDITOR_COMPONENT)) {
		AddComponent(entityIndex, ecs::ComponentType::ComponentType::EDITOR_COMPONENT);
	}
	ecs::EditorComponent editorComponent = editorComponentManager.GetComponent(entityIndex);
    if(name.empty()) {
		editorComponent.name = std::string("Entity ").append(std::to_string(entityIndex));
    } else {
	    editorComponent.name = name;
	}
	editorComponentManager.SetComponent(entityIndex, editorComponent);
}

void EditorEcsManager::SaveState()
{

	// ------------ Engine components ------------ //
    copyComponentsManagersContainer_.GetComponentsManager<ecs::TransformsManager>() = 
		componentsManagersContainer_.GetComponentsManager<ecs::TransformsManager>();

    copyComponentsManagersContainer_.GetComponentsManager<ecs::RigidbodyManager>() = 
		componentsManagersContainer_.GetComponentsManager<ecs::RigidbodyManager>();

    copyComponentsManagersContainer_.GetComponentsManager<ecs::CollidersManager>() = 
		componentsManagersContainer_.GetComponentsManager<ecs::CollidersManager>();

    copyComponentsManagersContainer_.GetComponentsManager<ecs::SplineFollowersManager>() =
		componentsManagersContainer_.GetComponentsManager<ecs::SplineFollowersManager>();

    copyComponentsManagersContainer_.GetComponentsManager<ecs::ModelsManager>() = 
		componentsManagersContainer_.GetComponentsManager<ecs::ModelsManager>();

    copyComponentsManagersContainer_.GetComponentsManager<ecs::LightsManager>() = 
		componentsManagersContainer_.GetComponentsManager<ecs::LightsManager>();

	copyComponentsManagersContainer_.GetComponentsManager<ecs::AudioSourcesManager>() =
		componentsManagersContainer_.GetComponentsManager<ecs::AudioSourcesManager>();

	copyComponentsManagersContainer_.GetComponentsManager<ecs::TrailRendererManager>() =
		componentsManagersContainer_.GetComponentsManager<ecs::TrailRendererManager>();

    copyComponentsManagersContainer_.GetComponentsManager<ecs::ParticleSystemsManager>() = 
		componentsManagersContainer_.GetComponentsManager<ecs::ParticleSystemsManager>();

	copyComponentsManagersContainer_.GetComponentsManager<ecs::EditorComponentManager>() =
		componentsManagersContainer_.GetComponentsManager<ecs::EditorComponentManager>();

    // ------------ !Engine components ------------ //

    // ------------ Game components ------------ //
	copyComponentsManagersContainer_.GetComponentsManager<game::EnemiesManager>() =
		componentsManagersContainer_.GetComponentsManager<game::EnemiesManager>();

	copyComponentsManagersContainer_.GetComponentsManager<game::PlayerManager>() =
		componentsManagersContainer_.GetComponentsManager<game::PlayerManager>();

	copyComponentsManagersContainer_.GetComponentsManager<game::DestructibleElementManager>() =
		componentsManagersContainer_.GetComponentsManager<game::DestructibleElementManager>();

	copyComponentsManagersContainer_.GetComponentsManager<game::WeaponManager>() =
		componentsManagersContainer_.GetComponentsManager<game::WeaponManager>();

	copyComponentsManagersContainer_.GetComponentsManager<game::ProjectileManager>() =
		componentsManagersContainer_.GetComponentsManager<game::ProjectileManager>();

	copyComponentsManagersContainer_.GetComponentsManager<game::MissilesManager>() =
		componentsManagersContainer_.GetComponentsManager<game::MissilesManager>();
	// ------------ !Game components ------------ //

    copySubjectAddComponent_ = subjectAddComponent_;
    copySubjectRemoveComponent_ = subjectRemoveComponent_;
    copySubjectsTriggerEnter_ = subjectsTriggerEnter_;
    copySubjectsTriggerExit_ = subjectsTriggerExit_;
    copySubjectsColliderEnter_ = subjectsColliderEnter_;
    copySubjectsColliderExit_ = subjectsColliderExit_;
    copyEntities_ = entities_;
}

std::vector<ecs::EntityIndex> EditorEcsManager::InstantiatePrefab(const ecs::Prefab& prefab)
{
	auto entities = GameEcsManager::InstantiatePrefab(prefab);

	return entities;
}

} //namespace editor
} //namespace poke