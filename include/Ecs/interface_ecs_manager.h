//----------------------------------------------------------------------------------
// Copyright (c) 2019-2020, POK Family. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of POK Family nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author : Robin Alves
// Co-Author : Nicolas Schneider
// Date : 28.02.2020
//----------------------------------------------------------------------------------
#pragma once

#include <Ecs/ecs_utility.h>
#include <Ecs/Entities/entity_mask.h>
#include <Ecs/ComponentManagers/components_managers_container.h>
#include <Ecs/Archetypes/archetype.h>
#include <PhysicsEngine/collision.h>
#include <Ecs/Prefabs/prefab.h>

namespace poke {
namespace game {
class GamePrefab;
class GamePrefabsManager;
class GameArchetypesManager;
} //namespace game

namespace editor {
class EditorArchetypesManager;
class EditorPrefab;
class EditorPrefabsManager;
} // namespace editor

class CorePrefabsManager;
class CoreArchetypesManager;

namespace observer {
enum class EntitiesSubjects : uint8_t {
    INIT = 0,
    DESTROY,
    SET_ACTIVE,
    SET_INACTIVE
};
} //namespace observer

namespace ecs {
class EnginePrefab;
class Archetype;

class IEcsManager {
	friend CorePrefabsManager;
	friend EnginePrefab;
	friend CoreArchetypesManager;
	friend game::GamePrefab;
	friend game::GamePrefabsManager;
	friend game::GameArchetypesManager;
	friend editor::EditorPrefab;
	friend editor::EditorPrefabsManager;
	friend editor::EditorArchetypesManager;
public:
    virtual ~IEcsManager() = default;

	/**
	 * \brief Add a new entity. Check if there already a slot ready for the new entity. If not, create expand the size of it
	 * \return The index of the entity currently created
	 */
    virtual EntityIndex AddEntity(ArchetypeID archetypeID = defaultArchetypeID) = 0;

    /**
	 * \brief Set an entity with an archetype. WARNING the entity will not be placed inside the correct pool
	 * \param entityIndex
	 * \param archetype 
	 */
	virtual void SetEntityWithArchetype(EntityIndex entityIndex, const Archetype& archetype) = 0;

	/**
	 * \brief Wait a certain amount of time. When the time is elapse it will destroy the entity. If the entity is in an archetype, it will be SetInactive. If it an entity that is not in a archetype all its components will be removed.
	 * \param entityIndex : the index of the entity.
	 * \param timeInSecond : time before destroying the entity.
	 */
    virtual void DestroyEntity(EntityIndex entityIndex, float timeInSecond = 0.0f) = 0;

	/**
	 * \brief Remove all component from entities and set it inactive.
	 * \param entitiesIndexes : the index of the entity.
	 */
    virtual void DestroyEntities(
        const std::vector<EntityIndex>& entitiesIndexes) = 0;

    /**
     * \brief Add an archetype inside EcsManager. It will lock a pool of entities and return it. Don't forget to update the Archetype with the entityPool received. WARNING : Don't use this function outside from the an ArchetypeManager
     * \param archetype 
     * \param sizeOfArchetype 
     * \return 
     */
    virtual void AddArchetype(
        const Archetype& archetype,
        unsigned int sizeOfArchetype) = 0;

	/**
	 * \brief Add a component to an entity.
	 * \param entityIndex : the index of the entity.
	 * \param attribute: ComponentType(s) or EntityType(s).
	 */
    virtual void AddComponent(
        EntityIndex entityIndex,
        ComponentMask attribute) = 0;

	/**
	 * \brief Add a component to multiple entities.
	 * \param entitiesIndexes : the index of the entity.
	 * \param attribute: ComponentType(s) or EntityType(s).
	 */
    virtual void AddComponents(
        const std::vector<EntityIndex>& entitiesIndexes,
        ComponentMask attribute) = 0;

	/**
	 * \brief Remove a component from an entity.
	 * \param entityIndex : the index of the entity.
	 * \param attribute: ComponentType(s) or EntityType(s).
	*/
    virtual void RemoveComponent(
        EntityIndex entityIndex,
        ComponentMask attribute) = 0;

	/**
     * \brief Update a component from an entity. Avoid using this function for in-game use.
     * \param entityIndex : the index of the entity.
     * \param attribute: ComponentType(s) or EntityType(s).
    */
	virtual void UpdateComponent(
		EntityIndex entityIndex,
		ComponentMask attribute) = 0;

	/**
	 * \brief Remove a component from entities.
	 * \param entitiesIndexes : the index of the entity.
	 * \param attribute: ComponentType(s) or EntityType(s).
	*/
    virtual void RemoveComponents(const std::vector<EntityIndex>& entitiesIndexes, ComponentMask attribute) = 0;

	/**
	 * \brief Check if an entity possess a certain component
	 * \param entityIndex : the entity that you want to test
	 * \param attribute : ComponentType(s) or EntityFlag(s)
	 * \return true if the entity possess the component
	 */
    virtual bool HasComponent(EntityIndex entityIndex, ComponentMask attribute) = 0;

    /**
     * \brief Set an entity active.
     * \param entityIndex 
     * \param entityStatus 
     */
    virtual void SetActive(EntityIndex entityIndex, EntityStatus entityStatus) = 0;

    /**
     * \brief Set multiple entities active.
     * \param entitiesIndex 
     * \param entityStatus 
     */
    virtual void SetActive(const std::vector<EntityIndex>& entitiesIndex, EntityStatus entityStatus) = 0;

	/**
	 * \brief Get all entities Index that have the same components them this entityMask. WARNING This is an heavy function, avoid using this.
	 * \param entityMask
	 * \return
	 */
    virtual std::vector<EntityIndex> FindSimilarEntities(
        EntityMask entityMask) = 0;

	/**
	 * \brief Get all entities that have the "IS_ACTIVE" component. Only use this function in the editor. WARNING This is an heavy function, avoid using this.
	 * \return a vector of EntityIndex
	 */
    virtual std::vector<EntityIndex> GetActiveEntities() = 0;

	/**
	 * \brief Get all entities that have the "IS_ACTIVE" component and a certain set of components. WARNING This is an heavy function, avoid using this.
	 * \param attribute: ComponentType(s) or EntityType(s)
	 * \return a vector with only the entity active and with this component
	 */
    virtual std::vector<EntityIndex> GetEntitiesWithComponents(
        ComponentMask attribute) = 0;

    /**
     * \brief Get all entities that have at least one component. Only use this function in the inspector. WARNING This is an heavy function, avoid using this.
     * \return 
     */
    virtual std::vector<EntityIndex> GetNotEmptyEntities() = 0;

    /**
	 * \brief Check if the entity is active.
	 * \param entity
	 * \return 
	 */
	virtual bool IsEntityActive(EntityIndex entity) = 0;

    /**
	 * \brief Get Pools.
	 * \return 
	 */
	virtual const std::vector<EntityPool>& GetPools() = 0;

    /**
	 * \brief Check if the given entity belong to the Archetype.
	 * \param entity 
	 * \param archetypeID 
	 * \return true if the given entity belongs to the Archetype.
	 */
	virtual bool IsEntityFromArchetype(EntityIndex entity, ArchetypeID archetypeID) = 0;

    template <typename T>
    T& GetComponentsManager()
    {
        return componentsManagersContainer_.GetComponentsManager<T>(
        );
    }

    template <typename T>
    T& RegisterManager(
        T&& componentsManager)
    {
        return componentsManagersContainer_.
            RegisterManager<T>(std::move(componentsManager));
    }

    virtual void AddObserver(
        observer::EntitiesSubjects subject,
        std::function<void(int)> observerCallback) = 0;

    /**
     * \brief Set the entity graphics active. WARNING this function should only be used when loading scene or in the ChunkSystem.
     * \param entityIndex 
     * \param entityStatus 
     */
    virtual void SetEntityVisible(
        EntityIndex entityIndex,
        EntityStatus entityStatus) = 0;

    /**
     * \brief Set entities graphics active. WARNING this function should only be used when loading scene or in the ChunkSystem.
     * \param entitiesIndex 
     * \param entityStatus 
     */
    virtual void SetEntityVisible(
        const std::vector<EntityIndex>& entitiesIndex,
        EntityStatus entityStatus) = 0;

    /**
     * \brief Get the tag of the given entity.
     * \param entityIndex 
     * \return 
     */
    virtual EntityTag GetTag(EntityIndex entityIndex) = 0;

    /**
     * \brief Set the tag for a given entity.
     * \param entityIndex 
     * \param tag 
     */
    virtual void SetTag(EntityIndex entityIndex, EntityTag tag) = 0;

    /**
     * \brief Return true if the entity is graphics active.
     * \param entityIndex 
     * \return 
     */
    virtual bool IsEntityVisible(EntityIndex entityIndex) = 0;

    /**
     * \brief Return all drawn entities from the previous frame.
     * \return 
     */
    virtual const std::vector<EntityIndex>& GetDrawnEntities() const = 0;

    /**
     * \brief Use to set all visible Entities. WARNING this function should only be called from the DrawSystem.
     * \param visibleEntities 
     */
    virtual void SetVisibleEntities(
        const std::vector<EntityIndex>& visibleEntities) = 0;

	//------------------------- LOADING / UNLOADING ---------------------------
	virtual std::map<ecs::EntityIndex, ecs::EntityIndex> SetEntitiesFromJson(const json& entitiesJson) = 0;
	virtual std::tuple<json, std::map<ecs::EntityIndex, ecs::EntityIndex>> EntitiesToJson() = 0;
	//-------------------------------------------------------------------------

    //------------------------------- OBSERVER --------------------------------
    virtual void RegisterObserverAddComponent(
        const std::function<void(EntityIndex, ComponentMask)>& callback) = 0;

    virtual void RegisterObserverRemoveComponent(
        const std::function<void(EntityIndex, ComponentMask)>& callback) = 0;

	virtual void RegisterObserverUpdateComponent(
		const std::function<void(EntityIndex, ComponentMask)>& callback) = 0;

    virtual void RegisterObserverTriggerEnter(
        EntityIndex entityIndex,
        const std::function<void(EntityIndex, physics::Collision)>& callback) = 0;

    virtual void RegisterObserverTriggerExit(
        EntityIndex entityIndex,
        const std::function<void(EntityIndex, physics::Collision)>& callback) = 0;

    virtual void RegisterObserverColliderEnter(
        EntityIndex entityIndex,
        const std::function<void(EntityIndex, physics::Collision)>& callback) = 0;

    virtual void RegisterObserverColliderExit(
        EntityIndex entityIndex,
        const std::function<void(EntityIndex, physics::Collision)>& callback) = 0;

    virtual void RemoveObserverAddComponent(
        const std::function<void(EntityIndex, ComponentMask)>& callback) = 0;

    virtual void RemoveObserverRemoveComponent(
        const std::function<void(EntityIndex, ComponentMask)>& callback) = 0;

    virtual void RemoveObserverTriggerEnter(
        EntityIndex entityIndex,
        const std::function<void(EntityIndex,physics::Collision)>& callback) = 0;

    virtual void RemoveObserverTriggerExit(
        EntityIndex entityIndex,
        const std::function<void(EntityIndex,physics::Collision)>& callback) = 0;

    virtual void RemoveObserverColliderEnter(
        EntityIndex entityIndex,
        const std::function<void(EntityIndex, physics::Collision)>& callback) = 0;

    virtual void RemoveObserverColliderExit(
        EntityIndex entityIndex,
		const std::function<void(EntityIndex,physics::Collision)>& callback) = 0;

    virtual void NotifyTriggerEnter(EntityIndex entityIndex, physics::Collision collision) = 0;

    virtual void NotifyTriggerExit(EntityIndex entityIndex, physics::Collision collision) = 0;

    virtual void NotifyColliderEnter(EntityIndex entityIndex, physics::Collision collision) = 0;

    virtual void NotifyColliderExit(EntityIndex entityIndex, physics::Collision collision) = 0;
    //-------------------------------------------------------------------------

	virtual void OnUnloadScene() = 0;
protected:
    //TODO(@Nico) Remove it from the class.
    ComponentsManagersContainer componentsManagersContainer_;
private:
	virtual std::vector<EntityIndex> InstantiatePrefab(const Prefab& prefab) = 0;

	/**
     * \brief Update an already existing archetype. WARNING Don't use this function for resizing. This function must be called only from the ArchetypeManger.
     * \param archetypeID
     * \param archetype
     */
	virtual void UpdateArchetype(ArchetypeID archetypeID, const Archetype& archetype) = 0;

	/**
	 * \brief Resize an archetype. WARNING This function must be called only from the ArchetypeManager.
	 * \param archetypeID
	 * \param newSize
	 * \param archetype
	 */
	virtual void ResizeArchetype(ArchetypeID archetypeID, size_t newSize, const Archetype& archetype) = 0;
};
} //namespace ecs
} //namespace poke
