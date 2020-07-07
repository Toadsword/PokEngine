#include <Ecs/core_ecs_manager.h>

#include <CoreEngine/engine.h>
#include <Utility/log.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>
#include <Utility/time_custom.h>
#include <Ecs/ComponentManagers/trail_renderer_manager.h>
#include <Ecs/ComponentManagers/segment_renderer_manager.h>

namespace poke::ecs {
CoreEcsManager::CoreEcsManager(Engine& engine, const size_t defaultPoolSize)
    : subjectsContainer_(
        {},
        {
            observer::EntitiesSubjects::INIT,
            observer::EntitiesSubjects::DESTROY,
            observer::EntitiesSubjects::SET_ACTIVE,
            observer::EntitiesSubjects::SET_INACTIVE
        })
{
    pools_.emplace_back(0, defaultPoolSize);

    AllocatePoolMemory(defaultPoolSize);

    engine.AddObserver(
        observer::MainLoopSubject::APP_BUILD,
        [this]() { OnAppBuild(); });

    /*engine.GetModuleManager().sceneManager.AddOnUnloadObserver(
        [this]() { OnUnloadScene(); });*/
}

EntityIndex CoreEcsManager::AddEntity(const ArchetypeID archetypeID) {
	if (archetypeID == defaultArchetypeID) {
		for (auto entityIndex = pools_[archetypeID].nextFreeEntity; entityIndex < pools_[archetypeID].lastEntity; entityIndex++) {
			//If found an entity that is not active
			if (entities_[entityIndex].IsEmpty()) {
				entities_[entityIndex].SetActive();
				subjectsContainer_.NotifySubject(
					observer::EntitiesSubjects::SET_ACTIVE,
					entityIndex);

				subjectsContainer_.NotifySubject(
					observer::EntitiesSubjects::INIT,
					entityIndex);

				//Set new first free entity to the first one that is inactive
				for (EntityIndex newFreeEmpty = pools_[archetypeID].firstEntity; newFreeEmpty < pools_[archetypeID].lastEntity; newFreeEmpty++) {
					if (entities_[newFreeEmpty].IsEmpty()) {
						pools_[archetypeID].nextFreeEntity = newFreeEmpty;
						break;
					}
				}

				return entityIndex;
			}
		}

		pools_[archetypeID].nextFreeEntity = pools_[archetypeID].firstEntity;

		LogWarning(
			"The maximum number of default entities has been overflow => Make the default entity pool bigger",
			LogType::ECS_LOG);

		return pools_[archetypeID].nextFreeEntity;
	}

	for (auto entityIndex = pools_[archetypeID].nextFreeEntity; entityIndex < pools_[archetypeID].lastEntity; entityIndex++) {
		//If found an entity that is not active
		if (!entities_[entityIndex].IsActive()) {

			entities_[entityIndex].SetActive();

			subjectsContainer_.NotifySubject(
				observer::EntitiesSubjects::SET_ACTIVE,
				entityIndex);

			subjectsContainer_.NotifySubject(
				observer::EntitiesSubjects::INIT,
				entityIndex);

			//Set new first free entity to the first one that is inactive
			for (size_t newFreeEmpty =
				pools_[archetypeID].firstEntity;
				newFreeEmpty < pools_[archetypeID].lastEntity; newFreeEmpty++) {
				if (!entities_[newFreeEmpty].IsActive()) {
					pools_[archetypeID].nextFreeEntity = newFreeEmpty;
					break;
				}
			}

			return entityIndex;
		}
	}

	pools_[archetypeID].nextFreeEntity = pools_[archetypeID].firstEntity;

	LogWarning(
		"The maximum number of default entities has been overflow => Make the default entity pool bigger",
		LogType::ECS_LOG);

	return pools_[archetypeID].nextFreeEntity;
}

void CoreEcsManager::SetEntityWithArchetype(
    const EntityIndex entityIndex,
    const Archetype& archetype)
{
    EntityPool pool;
    pool.firstEntity = entityIndex;
    pool.lastEntity = entityIndex + 1;
    componentsManagersContainer_.SetWithArchetype(pool, archetype);
	AddComponent(entityIndex, archetype.GetComponentMask());
}

void CoreEcsManager::DestroyEntity(const EntityIndex entityIndex, const float timeInSecond)
{
	if (timeInSecond <= 0.0f) {
		auto& transformManager = GetComponentsManager<TransformsManager>();
		transformManager.SetParent(entityIndex, kNoParent);

		subjectsContainer_.NotifySubject(
			observer::EntitiesSubjects::DESTROY,
			entityIndex);

		//Destroy all children
		const auto children = transformManager.GetChildren(entityIndex);
		DestroyEntities(children);

		RemoveComponent(entityIndex, entities_[entityIndex].GetComponentMask());

		componentsManagersContainer_.ClearEntity(entityIndex);

		for (size_t i = 0; i < pools_.size(); i++) {
			if (entityIndex >= pools_[i].firstEntity && entityIndex <= pools_[i].lastEntity) {
				if (i > defaultArchetypeID) {
					const auto& archetype = ArchetypesManagerLocator::Get().GetArchetype(i);

					//Reset default value to component fo the archetype
					componentsManagersContainer_.SetWithArchetype(
						EntityPool{ entityIndex, entityIndex + 1 },
						archetype);

					AddComponent(entityIndex, archetype.GetComponentMask());

				}
				else {
					subjectsColliderEnter_[entityIndex].Clear();
					subjectsColliderExit_[entityIndex].Clear();
					subjectsTriggerEnter_[entityIndex].Clear();
					subjectsTriggerExit_[entityIndex].Clear();
				}

				for (EntityIndex index = pools_[i].firstEntity; index < pools_[i].lastEntity; index++) {
					if (!IsEntityActive(index)) {
						pools_[i].nextFreeEntity = index;
						return;
					}
				}
				pools_[i].nextFreeEntity = pools_[i].firstEntity;;
				return;
			}
		}
	}
	else {
		entitiesToDestroy_.push_back({ entityIndex, timeInSecond });
	}

	LogWarning(
		"You're trying to destroy the entity " + std::to_string(entityIndex) +
		"that doesn't belong to any entity pool",
		LogType::ECS_LOG);
}

void CoreEcsManager::DestroyEntities(const std::vector<EntityIndex>& entitiesIndexes)
{
    for(auto entityIndex: entitiesIndexes) {
		DestroyEntity(entityIndex);
    }
}

void CoreEcsManager::AddArchetype(
    const Archetype& archetype,
    const unsigned int sizeNeeded)
{
    //Create new available pool
    EntityPool newPool;

    newPool.firstEntity = pools_.back().lastEntity + 1;
    newPool.lastEntity = newPool.firstEntity + sizeNeeded;
	newPool.nextFreeEntity = newPool.firstEntity;

    //Make the number of entities bigger.
    AllocatePoolMemory(newPool.lastEntity - newPool.firstEntity + 1);

    //Set the component mask for every entity
    for (auto entityIndex = newPool.firstEntity;
         entityIndex < newPool.lastEntity; entityIndex++) {
        AddComponent(entityIndex, archetype.GetComponentMask());

        SetActive(entityIndex, EntityStatus::INACTIVE);
        SetEntityVisible(entityIndex, EntityStatus::INACTIVE);
    }

    componentsManagersContainer_.SetWithArchetype(
        newPool,
        archetype);

    pools_.push_back(newPool);
}

void CoreEcsManager::UpdateArchetype(
    const ArchetypeID archetypeID,
    const Archetype& archetype)
{
	componentsManagersContainer_.SetWithArchetype(
		pools_[archetypeID],
		archetype);

	for (size_t entityIndex = pools_[archetypeID].firstEntity; entityIndex < pools_[archetypeID].lastEntity; entityIndex++) {
		const bool active = IsEntityActive(entityIndex);
		entities_[entityIndex].RemoveComponent(entities_[entityIndex].GetComponentMask());
        if(active) {
		    SetActive(entityIndex, EntityStatus::ACTIVE);
        }
		AddComponent(entityIndex, archetype.GetComponentMask());
    }
}

void CoreEcsManager::ResizeArchetype(const ArchetypeID archetypeID, const size_t newSize, const Archetype& archetype)
{
	const auto previousSize = pools_[archetypeID].lastEntity - pools_[archetypeID].firstEntity;

    if(newSize > previousSize) {
		const size_t diff = newSize - previousSize;
        //Insert new data
		const auto mask = entities_[pools_[archetypeID].firstEntity];
		const auto componentMask = entities_[pools_[archetypeID].firstEntity].GetComponentMask();
        for(auto entity = pools_[archetypeID].firstEntity; entity < pools_[archetypeID].lastEntity + diff; entity++) {
			entities_.insert(entities_.begin() + entity, mask);
			entities_[entity].AddComponent(componentMask);
			SetActive(entity, EntityStatus::INACTIVE);

			componentsManagersContainer_.InsertArchetype(entity, archetype);
        }

		subjectsTriggerEnter_.insert(subjectsTriggerEnter_.begin() + pools_[archetypeID].firstEntity, diff, observer::Subject<const EntityIndex, const physics::Collision>());
		subjectsTriggerExit_.insert(subjectsTriggerExit_.begin() + pools_[archetypeID].firstEntity, diff, observer::Subject<const EntityIndex, const physics::Collision>());
		subjectsColliderEnter_.insert(subjectsColliderEnter_.begin() + pools_[archetypeID].firstEntity, diff, observer::Subject<const EntityIndex, const physics::Collision>());
		subjectsColliderExit_.insert(subjectsColliderExit_.begin() + pools_[archetypeID].firstEntity, diff, observer::Subject<const EntityIndex, const physics::Collision>());

        //Update pools
		pools_[archetypeID].lastEntity += diff;

        for(auto i = archetypeID + 1; i < pools_.size(); i++) {
			pools_[i].firstEntity += diff;
			pools_[i].lastEntity += diff;
        }

    }else if(newSize < previousSize){
		const size_t diff = previousSize - newSize;
        for(int i = pools_[archetypeID].firstEntity; i < pools_[archetypeID].firstEntity + diff; i++) {
			DestroyEntity(i);
        }
		for (int i = 0; i < diff; i++) {
			entities_.erase(entities_.begin() + pools_[archetypeID].firstEntity);


			subjectsTriggerEnter_.erase(subjectsTriggerEnter_.begin() + pools_[archetypeID].firstEntity);
			subjectsTriggerExit_.erase(subjectsTriggerExit_.begin() + pools_[archetypeID].firstEntity);
			subjectsColliderEnter_.erase(subjectsColliderEnter_.begin() + pools_[archetypeID].firstEntity);
			subjectsColliderExit_.erase(subjectsColliderExit_.begin() + pools_[archetypeID].firstEntity);
		}
		componentsManagersContainer_.EraseArchetype(pools_[archetypeID], newSize);

		//Update pools
		pools_[archetypeID].lastEntity -= diff;

		for (auto i = archetypeID + 1; i < pools_.size(); i++) {
			pools_[i].firstEntity -= diff;
			pools_[i].lastEntity -= diff;
		}
    }
}

void CoreEcsManager::AddComponent(
    const EntityIndex entityIndex,
    const ComponentMask attribute)
{
    cassert(
        entityIndex < entities_.size(),
        "Entity " + std::to_string(entityIndex) + "out of range AddComponent!");
    entities_[entityIndex].AddComponent(attribute);

    subjectAddComponent_.Notify(entityIndex, attribute);
}

void CoreEcsManager::UpdateComponent(const EntityIndex entityIndex, const ComponentMask attribute)
{
	subjectUpdateComponent_.Notify(entityIndex, attribute);
}

void CoreEcsManager::AddComponents(
    const std::vector<EntityIndex>& entitiesIndexes,
    const ComponentMask attribute)
{
    for (const size_t entityIndex : entitiesIndexes) {
        cassert(
            entityIndex < entities_.size(),
            "Entity out of range AddComponent!");
        entities_[entityIndex].AddComponent(attribute);
        subjectAddComponent_.Notify(entityIndex, attribute);
    }
}

void CoreEcsManager::RemoveComponent(
    const EntityIndex entityIndex,
    const ComponentMask attribute)
{
    cassert(
        entityIndex < entities_.size(),
        "Entity out of range RemoveComponent!");
    entities_[entityIndex].RemoveComponent(attribute);

    subjectRemoveComponent_.Notify(entityIndex, attribute);
}

void CoreEcsManager::RemoveComponents(
    const std::vector<EntityIndex>& entitiesIndexes,
    const ComponentMask attribute)
{
    for (const size_t entityIndex : entitiesIndexes) {
        cassert(
            entityIndex < entities_.size(),
            "Entity out of range RemoveComponent!");
        entities_[entityIndex].RemoveComponent(attribute);

        subjectRemoveComponent_.Notify(entityIndex, attribute);
    }
}

bool CoreEcsManager::HasComponent(
    const EntityIndex entityIndex,
    const ComponentMask attribute)
{
    if(entityIndex >= entities_.size()) {
 		std::cout << "ici\n";
    }

    cassert(
        entityIndex < entities_.size(),
        "Entity " + std::to_string(entityIndex) + " out of range HasComponent!");
    return entities_[entityIndex].HasComponent(attribute);
}

void CoreEcsManager::SetActive(
    const EntityIndex entityIndex,
    const EntityStatus entityStatus)
{
    switch (entityStatus) {
    case EntityStatus::INACTIVE:
        entities_[entityIndex].RemoveComponent(EntityFlag::IS_ACTIVE);
        subjectsContainer_.NotifySubject(
            observer::EntitiesSubjects::SET_INACTIVE,
            entityIndex);
        break;
    case EntityStatus::ACTIVE:
        entities_[entityIndex].AddComponent(EntityFlag::IS_ACTIVE);
        subjectsContainer_.NotifySubject(
            observer::EntitiesSubjects::SET_ACTIVE,
            entityIndex);
        break;
    }

	const auto& children = GetComponentsManager<TransformsManager>().GetChildren(entityIndex);
	SetActive(children, entityStatus);
}

void CoreEcsManager::SetActive(
    const std::vector<EntityIndex>& entitiesIndex,
    const EntityStatus entityStatus)
{
	for (EntityIndex entityIndex : entitiesIndex) {
		SetActive(entityIndex, entityStatus);
	}
}

void CoreEcsManager::SetEntityVisible(
    const EntityIndex entityIndex,
    const EntityStatus entityStatus)
{
    cassert(
        entityIndex < entities_.size(),
        "Entity out of range SetEntityActive!");
    switch (entityStatus) {
    case EntityStatus::INACTIVE:
        entities_[entityIndex].RemoveComponent(
            EntityFlag::IS_VISIBLE);
        break;
    case EntityStatus::ACTIVE:
        entities_[entityIndex].AddComponent(EntityFlag::IS_VISIBLE);
        break;
    }

	const auto& children = GetComponentsManager<TransformsManager>().GetChildren(entityIndex);
	SetEntityVisible(children, entityStatus);
}

void CoreEcsManager::SetEntityVisible(
    const std::vector<EntityIndex>& entitiesIndex,
    const EntityStatus entityStatus)
{
    switch (entityStatus) {
    case EntityStatus::INACTIVE:
        for (size_t entityIndex : entitiesIndex) {
            cassert(
                entityIndex < entities_.size(),
                "Entity out of range SetEntitiesActive!");
            entities_[entityIndex].RemoveComponent(EntityFlag::IS_VISIBLE);

			const auto& children = GetComponentsManager<TransformsManager>().GetChildren(entityIndex);
			SetEntityVisible(children, entityStatus);
        }
        break;
    case EntityStatus::ACTIVE:
        for (size_t entityIndex : entitiesIndex) {
            cassert(
                entityIndex < entities_.size(),
                "Entity out of range SetEntitiesActive!");
            entities_[entityIndex].AddComponent(
                EntityFlag::IS_VISIBLE);

			const auto& children = GetComponentsManager<TransformsManager>().GetChildren(entityIndex);
			SetEntityVisible(children, entityStatus);
        }
        break;
    }
}

EntityTag CoreEcsManager::GetTag(const EntityIndex entityIndex)
{
    return entities_[entityIndex].GetTag();
}

void CoreEcsManager::SetTag(const EntityIndex entityIndex, const EntityTag tag)
{
    entities_[entityIndex].SetTag(tag);
}

std::vector<EntityIndex> CoreEcsManager::FindSimilarEntities(
    const EntityMask entityMask)
{
    std::vector<EntityIndex> entityIndexes;

    entityIndexes.reserve(entities_.size());

    for (size_t entityIndex = 0; entityIndex < entities_.size(); entityIndex++
    ) {
        if (entities_[entityIndex].HasSameComponents(
            entityMask.GetComponentMask())) {
            entityIndexes.emplace_back(entityIndex);
        }
    }
    entityIndexes.shrink_to_fit();

    return entityIndexes;
}

std::vector<EntityIndex> CoreEcsManager::GetActiveEntities()
{
    EntityMask entityCheck;
    entityCheck.AddComponent(
        static_cast<ComponentType::ComponentType>(EntityFlag::IS_ACTIVE));

    std::vector<EntityIndex> entityIndexes;

    entityIndexes.reserve(entities_.size());

    for (size_t entityIndex = 0; entityIndex < entities_.size(); entityIndex++
    ) {
        if (entities_[entityIndex].HasSameComponents(
            entityCheck.GetComponentMask())) {
            entityIndexes.emplace_back(entityIndex);
        }
    }
    entityIndexes.shrink_to_fit();

    return entityIndexes;
}

std::vector<EntityIndex> CoreEcsManager::GetEntitiesWithComponents(
    const ComponentMask attributes)
{
    EntityMask entityCheck;
    entityCheck.AddComponent(attributes);

    return FindSimilarEntities(entityCheck);
}

std::vector<EntityIndex> CoreEcsManager::GetNotEmptyEntities()
{
    std::vector<EntityIndex> entityIndexes;

    entityIndexes.reserve(entities_.size());

    for (size_t entityIndex = 0; entityIndex < entities_.size(); entityIndex++
    ) {
        if (!entities_[entityIndex].IsEmpty()) {
            entityIndexes.emplace_back(entityIndex);
        }
    }
    return entityIndexes;
}

const std::vector<EntityIndex>& CoreEcsManager::GetDrawnEntities() const
{
    return visibleEntities_;
}

void CoreEcsManager::SetVisibleEntities(
    const std::vector<EntityIndex>& visibleEntities)
{
    visibleEntities_ = visibleEntities;
}

void CoreEcsManager::OnAppBuild()
{
	GraphicsEngineLocator::Get().GetEngine().AddObserver(
		observer::MainLoopSubject::UPDATE,
		[this]() {UpdateDestroyedEntities(); });

    componentsManagersContainer_.Init();
}

void CoreEcsManager::OnUnloadScene()
{
    //Clear all entities.
    for (size_t entityIndex = 0; entityIndex < entities_.size(); entityIndex++) {
        DestroyEntity(entityIndex);
    }

    //Reset all pool to its starting position.
    for (auto& pool : pools_) { pool.nextFreeEntity = pool.firstEntity; }
}

std::vector<EntityIndex> CoreEcsManager::InstantiatePrefab(const Prefab& prefab)
{
	std::vector<EntityIndex> entities;
	const auto& parentsIndexes = prefab.GetParentIndexes();
	const auto& archetypesIDs = prefab.GetArchetypesID();
	entities.reserve(parentsIndexes.size());

	auto& transformManager = 
		componentsManagersContainer_.GetComponentsManager<TransformsManager>();

	for (size_t i = 0; i < parentsIndexes.size(); i++) {
		EntityIndex entityIndex;

		if (archetypesIDs[i] == 0) {
			//If the entity doesn't belongs to an archetype, every component must be set manually
			entityIndex = AddEntity();
			SetEntityWithArchetype(entityIndex, prefab.GetObject(i));
		}
		else {
			//If the entity belongs to an archetype, their is no need to reset all values
			entityIndex = AddEntity(archetypesIDs[i]);
		}

		entities.emplace_back(entityIndex);

		if (parentsIndexes[i] != kNoParent) {
			transformManager.SetParent(entityIndex, entities[parentsIndexes[i]]);
		}
	}

	return entities;
}

void CoreEcsManager::AllocatePoolMemory(const size_t sizeToAdd)
{
	const size_t newSize = entities_.size() + sizeToAdd;

    entities_.resize(newSize, EntityMask());
    componentsManagersContainer_.ResizeEntities(newSize);
    subjectsTriggerEnter_.resize(newSize);
    subjectsTriggerExit_.resize(newSize);
    subjectsColliderEnter_.resize(newSize);
    subjectsColliderExit_.resize(newSize);
}

void CoreEcsManager::AddObserver(
    const observer::EntitiesSubjects subject,
    const std::function<void(int)> observerCallback)
{
    subjectsContainer_.AddObserver(subject, observerCallback);
}

bool CoreEcsManager::IsEntityVisible(const EntityIndex entityIndex)
{
    return entities_[entityIndex].IsGraphicsActive();
}

bool CoreEcsManager::IsEntityActive(const EntityIndex entity)
{
    return entities_[entity].IsActive();
}

bool CoreEcsManager::IsEntityFromArchetype(
    const EntityIndex entity,
    const ArchetypeID archetypeID)
{
    return (entity >= pools_[archetypeID].firstEntity && entity <= pools_[
                archetypeID].lastEntity);
}

std::map<ecs::EntityIndex, ecs::EntityIndex> CoreEcsManager::SetEntitiesFromJson(const json& entitiesJson)
{
	// ------------ Engine components ------------ //
	auto& modelsManager = componentsManagersContainer_.GetComponentsManager<ModelsManager>();
	auto& rigidbodyManager = componentsManagersContainer_.GetComponentsManager<RigidbodyManager>();
	auto& transformManager = componentsManagersContainer_.GetComponentsManager<TransformsManager>();
	auto& collidersManager = componentsManagersContainer_.GetComponentsManager<CollidersManager>();
	auto& splinesManager = componentsManagersContainer_.GetComponentsManager<SplineFollowersManager>();
	auto& lightsManager = componentsManagersContainer_.GetComponentsManager<LightsManager>();
	auto& particleSystemManager = componentsManagersContainer_.GetComponentsManager<ParticleSystemsManager>();
	auto& audioSourcesManager = componentsManagersContainer_.GetComponentsManager<AudioSourcesManager>();
	auto& trailRendererManager = componentsManagersContainer_.GetComponentsManager<TrailRendererManager>();
	auto& segmentRendererManager = componentsManagersContainer_.GetComponentsManager<SegmentRendererManager>();
	// ------------ !Engine components ------------ //

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

			const std::vector<ecs::EntityIndex>& instantiatedObjects = PrefabsManagerLocator::Get().Instantiate(nameString);

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
				AddComponent(entityIndex, ComponentType::ComponentType::COLLIDER);
			}
			if (CheckJsonExists(entityJson, "model") &&
				CheckJsonParameter(entityJson, "model", nlohmann::detail::value_t::object)) {
				modelsManager.SetComponentFromJson(entityIndex, entityJson["model"]);
				AddComponent(entityIndex, ComponentType::ComponentType::MODEL);
			}
			if (CheckJsonExists(entityJson, "rigidbody") &&
				CheckJsonParameter(entityJson, "rigidbody", nlohmann::detail::value_t::object)) {
				rigidbodyManager.SetComponentFromJson(entityIndex, entityJson["rigidbody"]);
				AddComponent(entityIndex, ComponentType::ComponentType::RIGIDBODY);
			}
			if (CheckJsonExists(entityJson, "light") &&
				CheckJsonParameter(entityJson, "light", nlohmann::detail::value_t::object)) {
				lightsManager.SetComponentFromJson(entityIndex, entityJson["light"]);
				AddComponent(entityIndex, ComponentType::ComponentType::LIGHT);
			}
			if (CheckJsonExists(entityJson, "particleSystem") &&
				CheckJsonParameter(entityJson, "particleSystem", nlohmann::detail::value_t::object)) {
				particleSystemManager.SetComponentFromJson(entityIndex, entityJson["particleSystem"]);
				AddComponent(entityIndex, ComponentType::ComponentType::PARTICLE_SYSTEM);
			}
			if (CheckJsonExists(entityJson, "audioSource") &&
				CheckJsonParameter(entityJson, "audioSource", nlohmann::detail::value_t::object)) {

				audioSourcesManager.SetComponentFromJson(entityIndex, entityJson["audioSource"]);
				AddComponent(entityIndex, ComponentType::ComponentType::AUDIO_SOURCE);
			}
			if (CheckJsonExists(entityJson, "trailRenderer") &&
				CheckJsonParameter(entityJson, "trailRenderer", nlohmann::detail::value_t::object)) {

				trailRendererManager.SetComponentFromJson(entityIndex, entityJson["trailRenderer"]);
				AddComponent(entityIndex, ComponentType::ComponentType::TRAIL_RENDERER);
			}
			if (CheckJsonExists(entityJson, "segmentRenderer") &&
				CheckJsonParameter(entityJson, "segmentRenderer", nlohmann::detail::value_t::object)) {

				segmentRendererManager.SetComponentFromJson(entityIndex, entityJson["segmentRenderer"]);
				AddComponent(entityIndex, ComponentType::ComponentType::SEGMENT_RENDERER);
			}
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

std::tuple<json, std::map<ecs::EntityIndex, ecs::EntityIndex>> CoreEcsManager::EntitiesToJson()
{
	cassert(false, "Core Ecs Manager shouldn't be saving scenes.");
	return std::make_tuple(json(), std::map<ecs::EntityIndex, ecs::EntityIndex>());
}

void CoreEcsManager::RegisterObserverAddComponent(
    const std::function<void(EntityIndex, ComponentMask)>& callback)
{
    subjectAddComponent_.AddObserver(callback);
}

void CoreEcsManager::RegisterObserverUpdateComponent(
    const std::function<void(EntityIndex, ComponentMask)>& callback)
{
    subjectUpdateComponent_.AddObserver(callback);
}

void CoreEcsManager::RegisterObserverRemoveComponent(
    const std::function<void(EntityIndex, ComponentMask)>& callback)
{
    subjectRemoveComponent_.AddObserver(callback);
}

void CoreEcsManager::RegisterObserverTriggerEnter(
    const EntityIndex entityIndex,
    const std::function<void(EntityIndex, physics::Collision)>& callback)
{
    subjectsTriggerEnter_[entityIndex].AddObserver(callback);
}

void CoreEcsManager::RegisterObserverTriggerExit(
    const EntityIndex entityIndex,
    const std::function<void(EntityIndex, physics::Collision)>& callback)
{
    subjectsTriggerExit_[entityIndex].AddObserver(callback);
}

void CoreEcsManager::RegisterObserverColliderEnter(
    const EntityIndex entityIndex,
    const std::function<void(EntityIndex, physics::Collision)>& callback)
{
    subjectsColliderEnter_[entityIndex].AddObserver(callback);
}

void CoreEcsManager::RegisterObserverColliderExit(
    const EntityIndex entityIndex,
    const std::function<void(EntityIndex, physics::Collision)>& callback)
{
    subjectsColliderExit_[entityIndex].AddObserver(callback);
}

void CoreEcsManager::RemoveObserverAddComponent(
    const std::function<void(EntityIndex, ComponentMask)>& callback)
{
    subjectAddComponent_.RemoveObserver(callback);
}

void CoreEcsManager::RemoveObserverRemoveComponent(
    const std::function<void(EntityIndex, ComponentMask)>& callback)
{
    subjectRemoveComponent_.RemoveObserver(callback);
}

void CoreEcsManager::RemoveObserverTriggerEnter(
    const EntityIndex entityIndex,
    const std::function<void(EntityIndex, physics::Collision)>& callback)
{
    subjectsTriggerEnter_[entityIndex].RemoveObserver(callback);
}

void CoreEcsManager::RemoveObserverTriggerExit(
    const EntityIndex entityIndex,
    const std::function<void(EntityIndex, physics::Collision)>& callback)
{
    subjectsTriggerExit_[entityIndex].RemoveObserver(callback);
}

void CoreEcsManager::RemoveObserverColliderEnter(
    const EntityIndex entityIndex,
    const std::function<void(EntityIndex, physics::Collision)>& callback)
{
    subjectsColliderEnter_[entityIndex].RemoveObserver(callback);
}

void CoreEcsManager::RemoveObserverColliderExit(
    const EntityIndex entityIndex,
    const std::function<void(EntityIndex, physics::Collision)>& callback)
{
    subjectsColliderExit_[entityIndex].RemoveObserver(callback);
}

void CoreEcsManager::NotifyTriggerEnter(
    const EntityIndex entityIndex,
    const physics::Collision collision)
{
    subjectsTriggerEnter_[entityIndex].Notify(entityIndex, collision);
}

void CoreEcsManager::NotifyTriggerExit(
    const EntityIndex entityIndex,
    const physics::Collision collision)
{
    subjectsTriggerExit_[entityIndex].Notify(entityIndex, collision);
}

void CoreEcsManager::NotifyColliderEnter(
    const EntityIndex entityIndex,
    const physics::Collision collision)
{
    subjectsColliderEnter_[entityIndex].Notify(entityIndex, collision);
}

void CoreEcsManager::NotifyColliderExit(
    const EntityIndex entityIndex,
    const physics::Collision collision)
{
    subjectsColliderExit_[entityIndex].Notify(entityIndex, collision);
}

void CoreEcsManager::Destroy()
{
    visibleEntities_.clear();
    visibleEntities_.resize(0);

    pools_.clear();
    pools_.resize(0);

    subjectsContainer_.Clear();

    subjectAddComponent_.Clear();

    subjectRemoveComponent_.Clear();

    subjectsTriggerEnter_.clear();
    subjectsTriggerEnter_.resize(0);

    subjectsTriggerExit_.clear();
    subjectsTriggerExit_.resize(0);

    subjectsColliderEnter_.clear();
    subjectsColliderEnter_.resize(0);

    subjectsColliderExit_.clear();
    subjectsColliderExit_.resize(0);

    entities_.clear();
    entities_.resize(0);

    entitiesToDestroy_.clear();
    entitiesToDestroy_.resize(0);
}

void CoreEcsManager::UpdateDestroyedEntities()
{
	const float dt = Time::Get().deltaTime.count() / 1000.0f;
    for(auto entityIndex = entitiesToDestroy_.begin(); entityIndex != entitiesToDestroy_.end();) {
		entityIndex->second -= dt;

        if(entityIndex->second <= 0.0f) {
			DestroyEntity(entityIndex->first, 0.0f);
			entityIndex = entitiesToDestroy_.erase(entityIndex);
        }else {
			++entityIndex;
        }
    }
}

} //namespace poke::ecs
