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
// Date :  28.02.2020
//----------------------------------------------------------------------------------
#pragma once

#include <Ecs/interface_ecs_manager.h>
#include <CoreEngine/Observer/subject.h>
#include <CoreEngine/Observer/subjects_container.h>

namespace poke {
namespace ecs {
class CoreEcsManager : public IEcsManager {
public:
    CoreEcsManager(Engine& engine, size_t defaultPoolSize);
    virtual ~CoreEcsManager() = default;

    //------------------------- ENTITY INTERACTION ----------------------------
    void DestroyEntity(EntityIndex entityIndex, float timeInSecond = 0.0f) override;

    void DestroyEntities(const std::vector<EntityIndex>& entitiesIndexes) override;

    void AddComponent(
        EntityIndex entityIndex,
        ComponentMask attribute) override;

    void UpdateComponent(EntityIndex entityIndex, ComponentMask attribute) override;

    void AddComponents(
        const std::vector<EntityIndex>& entitiesIndexes,
        ComponentMask attribute) override;

    void RemoveComponent(
        EntityIndex entityIndex,
        ComponentMask attribute) override;

    void RemoveComponents(
        const std::vector<EntityIndex>& entitiesIndexes,
        ComponentMask attribute) override;

    bool HasComponent(
        EntityIndex entityIndex,
        ComponentMask attribute) override;

    void SetActive(
        EntityIndex entityIndex,
        EntityStatus entityStatus) override;

    void SetActive(
        const std::vector<EntityIndex>& entitiesIndex,
        EntityStatus entityStatus) override;

    void SetEntityVisible(
        EntityIndex entityIndex,
        EntityStatus entityStatus) override;

    void SetEntityVisible(
        const std::vector<EntityIndex>& entitiesIndex,
        EntityStatus entityStatus) override;

    EntityTag GetTag(EntityIndex entityIndex) override;

    void SetTag(EntityIndex entityIndex, EntityTag tag) override;

    std::vector<EntityIndex> FindSimilarEntities(EntityMask entityMask)
    override;

    std::vector<EntityIndex> GetActiveEntities() override;

    std::vector<EntityIndex> GetEntitiesWithComponents(
        ComponentMask attribute) override;

    std::vector<EntityIndex> GetNotEmptyEntities() override;

    const std::vector<EntityIndex>& GetDrawnEntities() const override;

    void SetVisibleEntities(
        const std::vector<EntityIndex>& visibleEntities) override;

    bool IsEntityVisible(EntityIndex entityIndex) override;

    bool IsEntityActive(EntityIndex entity) override;

    bool IsEntityFromArchetype(
        EntityIndex entity,
        ArchetypeID archetypeID) override;

    const std::vector<EntityPool>& GetPools() override { return pools_; }
    //-------------------------------------------------------------------------

    //----------------------------- ARCHETYPES --------------------------------
    EntityIndex AddEntity(ArchetypeID archetypeID = defaultArchetypeID) override;

    void SetEntityWithArchetype(
        EntityIndex entityIndex,
        const Archetype& archetype) override;

	void AddArchetype(
		const Archetype& archetype,
		unsigned int sizeNeeded) override;
    //-------------------------------------------------------------------------

    //------------------------- LOADING / UNLOADING ---------------------------
    std::map<ecs::EntityIndex, ecs::EntityIndex> SetEntitiesFromJson(const json& entitiesJson) override;

	std::tuple<json, std::map<ecs::EntityIndex, ecs::EntityIndex>> EntitiesToJson() override;
    //-------------------------------------------------------------------------

    //------------------------------- OBSERVERS -------------------------------
    void AddObserver(
        observer::EntitiesSubjects subject,
        std::function<void(int)> observerCallback) override;

    void RegisterObserverAddComponent(
        const std::function<void(EntityIndex, ComponentMask)>& callback)
    override;

    void RegisterObserverUpdateComponent(
        const std::function<void(EntityIndex, ComponentMask)>& callback) override;

    void RegisterObserverRemoveComponent(
        const std::function<void(EntityIndex, ComponentMask)>& callback)
    override;

    void RegisterObserverTriggerEnter(
        EntityIndex entityIndex,
        const std::function<void(EntityIndex, physics::Collision)>& callback)
    override;

    void RegisterObserverTriggerExit(
        EntityIndex entityIndex,
        const std::function<void(EntityIndex, physics::Collision)>& callback)
    override;

    void RegisterObserverColliderEnter(
        EntityIndex entityIndex,
        const std::function<void(EntityIndex, physics::Collision)>& callback)
    override;

    void RegisterObserverColliderExit(
        EntityIndex entityIndex,
        const std::function<void(EntityIndex, physics::Collision)>& callback)
    override;

    void RemoveObserverAddComponent(
        const std::function<void(EntityIndex, ComponentMask)>& callback)
    override;

    void RemoveObserverRemoveComponent(
        const std::function<void(EntityIndex, ComponentMask)>& callback)
    override;

    void RemoveObserverTriggerEnter(
        EntityIndex entityIndex,
        const std::function<void(EntityIndex, physics::Collision)>& callback)
    override;

    void RemoveObserverTriggerExit(
        EntityIndex entityIndex,
        const std::function<void(EntityIndex, physics::Collision)>& callback)
    override;

    void RemoveObserverColliderEnter(
        EntityIndex entityIndex,
        const std::function<void(EntityIndex, physics::Collision)>& callback)
    override;

    void RemoveObserverColliderExit(
        EntityIndex entityIndex,
        const std::function<void(EntityIndex, physics::Collision)>& callback)
    override;

    void NotifyTriggerEnter(
        EntityIndex entityIndex,
        physics::Collision collision) override;

    void NotifyTriggerExit(
        EntityIndex entityIndex,
        physics::Collision collision) override;

    void NotifyColliderEnter(
        EntityIndex entityIndex,
        physics::Collision collision) override;

    void NotifyColliderExit(
        EntityIndex entityIndex,
        physics::Collision collision) override;
    //-------------------------------------------------------------------------

    void Destroy();
    void OnUnloadScene() override;
protected:
    void UpdateDestroyedEntities();

    void OnAppBuild();

    void AllocatePoolMemory(size_t sizeToAdd);

    void UpdateArchetype(ArchetypeID archetypeID, const Archetype& archetype) override;

    void ResizeArchetype(
        ArchetypeID archetypeID,
        size_t newSize,
        const Archetype& archetype) override;

    std::vector<EntityIndex> InstantiatePrefab(const Prefab& prefab) override;

    std::vector<EntityIndex> visibleEntities_;

    std::vector<EntityPool> pools_;

    observer::SubjectsContainer<observer::EntitiesSubjects> subjectsContainer_;

    observer::Subject<const EntityIndex, const ComponentMask> subjectAddComponent_;

    observer::Subject<const EntityIndex, const ComponentMask> subjectRemoveComponent_;

    observer::Subject<const EntityIndex, const ComponentMask> subjectUpdateComponent_;

    std::vector<observer::Subject<const EntityIndex, const physics::Collision>>
    subjectsTriggerEnter_;

    std::vector<observer::Subject<const EntityIndex, const physics::Collision>>
    subjectsTriggerExit_;

    std::vector<observer::Subject<const EntityIndex, const physics::Collision>>
    subjectsColliderEnter_;

    std::vector<observer::Subject<const EntityIndex, const physics::Collision>>
    subjectsColliderExit_;

    std::vector<EntityMask> entities_;

    std::vector<std::pair<EntityIndex, float>> entitiesToDestroy_;
};
} // namespace ecs
} //namespace poke
