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
#include <Ecs/interface_ecs_manager.h>
#include <CoreEngine/cassert.h>

namespace poke {
namespace ecs {
class NullEcsManager : public IEcsManager {
public:
    EntityIndex AddEntity(ArchetypeID archetypeID = defaultArchetypeID) override
    {
		archetypeID;
		return kNoEntity;
    }

    void DestroyEntity(const EntityIndex entityIndex, float timeInSecond = 0.0f) override {
		(EntityIndex)entityIndex;
		(float)timeInSecond;
		cassert(false, "Impossible to clear entity in a null EcsManager");
    }

    void DestroyEntities(const std::vector<EntityIndex>& entitiesIndexes) override {
		entitiesIndexes;
		cassert(false, "Impossible to ClearEntities() in a null EcsManager");
    }

    void AddArchetype(
        const Archetype& archetype,
        unsigned int sizeOfArchetype) override {}

    void AddComponent(
        const EntityIndex entityIndex,
		const ComponentMask attribute) override {
		entityIndex;
		attribute;
		cassert(false, "Impossible to add a component to an entity in a null EcsManager");
    }

	void AddComponents(
		const std::vector<EntityIndex>& entitiesIndexes,
		const ComponentMask attribute) override {
		entitiesIndexes;
		attribute;
		cassert(false, "Impossible to add a component to entities in a null EcsManager");
    }

    void RemoveComponent(
        const EntityIndex entityIndex,
		const ComponentMask attribute) override {
		entityIndex;
		attribute;
		cassert(false, "Impossible to remove a component to an entity in a null EcsManager");
    }

	void RemoveComponents(
		const std::vector<EntityIndex>& entitiesIndexes,
		const ComponentMask attribute) override {
		entitiesIndexes;
		attribute;
		cassert(false, "Impossible to remove a component to entities in a null EcsManager");
    }

    bool HasComponent(
        const EntityIndex entityIndex,
		const ComponentMask attribute) override {
		entityIndex;
		attribute;
		return false;
    }

    void SetActive(
        const EntityIndex entityIndex,
        const EntityStatus entityStatus) override {
		entityIndex;
		entityStatus;
    }

	void SetEntityWithArchetype(EntityIndex entityIndex, const Archetype& archetype) override    
    {
		entityIndex;
		archetype;
    }

    void SetActive(
        const std::vector<EntityIndex>& entitiesIndex,
        const EntityStatus entityStatus) override {
		entitiesIndex;
		entityStatus;
    }

    std::vector<EntityIndex> FindSimilarEntities(
		const EntityMask entityMask) override {
		entityMask;
		cassert(false, "Impossible to find similar entities in a null EcsManager");
		return {};
    }

    std::vector<EntityIndex> GetActiveEntities() override {
		cassert(false, "Impossible to get active entities from a null EcsManager");
		return {};
    }

    std::vector<EntityIndex> GetEntitiesWithComponents(
		const ComponentMask attribute) override {
		attribute;
		cassert(false, "Impossible to get entities with components in a null EcsManager");
		return {};
    }

    std::vector<EntityIndex> GetNotEmptyEntities() override {
		cassert(false, "Impossible to get not empty entity in a null EcsManager");
		return {};
    }

    void AddObserver(
        observer::EntitiesSubjects subject,
        std::function<void(int)> observerCallback) override {
		subject;
		observerCallback;
		cassert(false, "Impossible to add an obsever in a null EcsManager");
    }

	void SetEntityVisible(
		EntityIndex entityIndex,
		EntityStatus entityStatus) override {
		entityIndex;
		entityStatus;
		cassert(false, "Impossible to SetEntityGraphicsActive from a null EcsManager");
    }

    void SetEntityVisible(
        const std::vector<EntityIndex>& entitiesIndex,
        EntityStatus entityStatus) override {
		entitiesIndex;
		entityStatus;
		cassert(false, "Impossible to SetEntityGraphicsActive from a null EcsManager");
    }

	EntityTag GetTag(EntityIndex entityIndex) override {
		entityIndex;
		return 0;
	}

	void SetTag(EntityIndex entityIndex, const EntityTag tag) override
    {
		entityIndex;
		tag;
    }

	bool IsEntityVisible(EntityIndex entityIndex) override
    {
		entityIndex;
        return false;
    }

	//------------------------- LOADING / UNLOADING ---------------------------
	std::map<ecs::EntityIndex, ecs::EntityIndex> SetEntitiesFromJson(const json& entitiesJson) override
    {
		entitiesJson;
		return std::map<ecs::EntityIndex, ecs::EntityIndex>();
    }
	std::tuple<json, std::map<ecs::EntityIndex, ecs::EntityIndex>> EntitiesToJson() override {
        return std::make_tuple(json(), std::map<ecs::EntityIndex, ecs::EntityIndex>());
    }
	//-------------------------------------------------------------------------

	//------------------------------- OBSERVER --------------------------------
    void RegisterObserverAddComponent(const std::function<void(EntityIndex, ComponentMask)>& callback) override
    {
		callback;
    }

	void RegisterObserverUpdateComponent(const std::function<void(EntityIndex, ComponentMask)>& callback) override
    {
		callback;
    }

    void RegisterObserverRemoveComponent(
        const std::function<void(EntityIndex, ComponentMask)>& callback)
    override
    {
		callback;
    }

    void RegisterObserverTriggerEnter(
        EntityIndex entityIndex,
        const std::function<void(EntityIndex, physics::Collision)>& callback) override
    {
		entityIndex;
		callback;
    }

    void RegisterObserverTriggerExit(
        EntityIndex entityIndex,
        const std::function<void(EntityIndex, physics::Collision)>& callback) override
    {
		entityIndex;
		callback;
    }

    void RegisterObserverColliderEnter(
        EntityIndex entityIndex,
        const std::function<void(EntityIndex, physics::Collision)>& callback) override
    {
		entityIndex;
		callback;
    }

    void RegisterObserverColliderExit(
        EntityIndex entityIndex,
        const std::function<void(EntityIndex, physics::Collision)>& callback) override
    {
		entityIndex;
		callback;
    }

    void RemoveObserverAddComponent(
        const std::function<void(EntityIndex, ComponentMask)>& callback) override
    {
		callback;
    }

    void RemoveObserverRemoveComponent(
        const std::function<void(EntityIndex, ComponentMask)>& callback) override
    {
		callback;
    }

    void RemoveObserverTriggerEnter(
        EntityIndex entityIndex,
        const std::function<void(EntityIndex, physics::Collision)>& callback) override
    {
		entityIndex;
		callback;
    }

    void RemoveObserverTriggerExit(
        EntityIndex entityIndex,
        const std::function<void(EntityIndex, physics::Collision)>& callback) override
    {
		entityIndex;
		callback;
    }

    void RemoveObserverColliderEnter(
        EntityIndex entityIndex,
        const std::function<void(EntityIndex, physics::Collision)>& callback) override
    {
		entityIndex;
		callback;
    }

    void RemoveObserverColliderExit(
        EntityIndex entityIndex,
        const std::function<void(EntityIndex, physics::Collision)>& callback) override
    {
		entityIndex;
		callback;
    }

    void NotifyTriggerEnter(
        EntityIndex entityIndex,
        physics::Collision collision) override
    {
		entityIndex;
		collision;
    }

    void NotifyTriggerExit(
        EntityIndex entityIndex,
        physics::Collision collision) override
    {
		entityIndex;
		collision;
    }

    void NotifyColliderEnter(
        EntityIndex entityIndex,
        physics::Collision collision) override
    {
		entityIndex;
		collision;
    }

    void NotifyColliderExit(
        EntityIndex entityIndex,
        physics::Collision collision) override
    {
		entityIndex;
		collision;
    }

    const std::vector<EntityIndex>& GetDrawnEntities() const override
    {
		return {};
    }

    void SetVisibleEntities(
        const std::vector<EntityIndex>& visibleEntities) override
    {
		visibleEntities;
    }

	bool IsEntityActive(EntityIndex entity) override
    {
		entity;
        return false;
    }
	const std::vector<EntityPool>& GetPools() override { return {}; }
    bool IsEntityFromArchetype(
        EntityIndex entity,
		ArchetypeID archetypeID) override {
		entity;
		archetypeID;
		return false;
	}

	void UpdateComponent(EntityIndex entityIndex, ComponentMask attribute) override
    {
		entityIndex;
		attribute;
    }
private:
    std::vector<EntityIndex> InstantiatePrefab(const Prefab& prefab) override
    {
		prefab;
		return { };
    }
	void UpdateArchetype(
		ArchetypeID archetypeID,
		const Archetype& archetype) override
    {
		archetypeID;
		archetype;
    }

	void ResizeArchetype(ArchetypeID archetypeID, size_t newSize, const Archetype& archetype) override
    {
		archetypeID;
		newSize;
		archetype;
    }

public:
    void OnUnloadScene() override{}
};
}//namespace ecs
}//namespace poke

