#include <CoreEngine/CoreSystems/chunks_system.h>

#include <CoreEngine/ServiceLocator/service_locator_definition.h>
#include <CoreEngine/engine.h>
#include <Utility/profiler.h>

namespace poke {

ChunksSystem::ChunksSystem(Engine& engine)
    : System(engine),
      newEntities_(8000),
      destroyedEntities_(100),
      updateEntities_(8000),
      chunkIndex_(8000),
      transformsManager_(ecsManager_.GetComponentsManager<ecs::TransformsManager>()),
      collidersManager_(ecsManager_.GetComponentsManager<ecs::CollidersManager>()),
      chunksManager_(ChunksManagerLocator::Get())
{
    engine.AddObserver(observer::MainLoopSubject::UPDATE, [this]() { OnUpdate(); });

    ObserveEntityDestroy();
    ObserveEntityAddComponent();
    ObserveEntityRemoveComponent();

    ChunksManagerLocator::Get().RegisterObserverNewActiveChunk(
        [this](const chunk::ChunkBitSet chunkIndex) { OnChangeActiveChunk(chunkIndex); });

    SceneManagerLocator::Get().AddOnUnloadObserver([this]() { OnUnloadScene(); });
}

void ChunksSystem::OnUpdate()
{
    pok_BeginProfiling(Chunk_System, 0);

    //If there is no chunk, then everything is active and visible
    if (chunksManager_.GetChunkNb() == 0) {
        for (auto entity : updateEntities_) {
            if (ecsManager_.IsEntityActive(entity)) {
                ecsManager_.SetEntityVisible(entity, ecs::EntityStatus::ACTIVE);
            } else { ecsManager_.SetEntityVisible(entity, ecs::EntityStatus::INACTIVE); }
        }

        for (auto newEntity : newEntities_) {
            if (ecsManager_.IsEntityActive(newEntity)) {
                ecsManager_.SetEntityVisible(newEntity, ecs::EntityStatus::ACTIVE);
            } else { ecsManager_.SetEntityVisible(newEntity, ecs::EntityStatus::INACTIVE); }

            updateEntities_.insert(newEntity);
        }

        newEntities_.clear();

        pok_EndProfiling(Chunk_System);
        return;
    }

    //Add entities to chunk
    pok_BeginProfiling(Add_Entities, 0);
    for (auto newEntity : newEntities_) {
        if (ecsManager_.HasComponent(newEntity, ecs::ComponentType::COLLIDER)) {
            chunkIndex_[newEntity] = chunksManager_.AddEntity(
                collidersManager_.GetComponent(newEntity).ComputeAABB(
                    transformsManager_.GetWorldPosition(newEntity),
                    transformsManager_.GetWorldScale(newEntity),
                    transformsManager_.GetWorldRotation(newEntity)));
        } else {
            chunkIndex_[newEntity] = chunksManager_.AddEntity(
                transformsManager_.GetWorldPosition(newEntity));
        }

        if (chunkIndex_[newEntity] == chunk::Chunk::kNullIndex) {
            ecsManager_.SetActive(newEntity, ecs::EntityStatus::INACTIVE);
            ecsManager_.SetEntityVisible(newEntity, ecs::EntityStatus::INACTIVE);
            continue;
        };

        switch (chunksManager_.GetChunkStatus(chunkIndex_[newEntity])) {
        case chunk::ChunkStatus::VISIBLE:
            ecsManager_.SetActive(newEntity, ecs::EntityStatus::INACTIVE);
            ecsManager_.SetEntityVisible(newEntity, ecs::EntityStatus::ACTIVE);
            break;
        case chunk::ChunkStatus::HIDDEN:
            ecsManager_.SetActive(newEntity, ecs::EntityStatus::INACTIVE);
            ecsManager_.SetEntityVisible(newEntity, ecs::EntityStatus::INACTIVE);
            break;
        case chunk::ChunkStatus::ACTIVE:
            ecsManager_.SetActive(newEntity, ecs::EntityStatus::ACTIVE);
            ecsManager_.SetEntityVisible(newEntity, ecs::EntityStatus::ACTIVE);
            break;
        default: ;
        }
    }
    newEntities_.clear();
    pok_EndProfiling(Add_Entities);

    //Update physics entities
    pok_BeginProfiling(Update_Entities, 0);
    for (const auto entity : updateEntities_) {
        if (ecsManager_.HasComponent(entity, ecs::ComponentType::COLLIDER)) {
            //If not in a chunk anymore set inactive
            chunkIndex_[entity] = chunksManager_.AddEntity(
                collidersManager_.GetComponent(entity).ComputeAABB(
                    transformsManager_.GetWorldPosition(entity),
                    transformsManager_.GetWorldScale(entity),
                    transformsManager_.GetWorldRotation(entity)));
        } else {
            //If not in a chunk anymore set inactive
            chunkIndex_[entity] = chunksManager_.AddEntity(
                transformsManager_.GetWorldPosition(entity));
        }
    }
    pok_EndProfiling(Update_Entities);

    //Active chunk
    pok_BeginProfiling(Active_Chunk, 0);

    for (auto entity : updateEntities_) {
        if (chunkIndex_[entity] == chunk::Chunk::kNullIndex) {
            ecsManager_.SetActive(entity, ecs::EntityStatus::INACTIVE);
            ecsManager_.SetEntityVisible(entity, ecs::EntityStatus::INACTIVE);
            continue;
        };

        switch (chunksManager_.GetChunkStatus(chunkIndex_[entity])) {
        case chunk::ChunkStatus::VISIBLE:
            ecsManager_.SetActive(entity, ecs::EntityStatus::INACTIVE);
            ecsManager_.SetEntityVisible(entity, ecs::EntityStatus::ACTIVE);
            break;
        case chunk::ChunkStatus::HIDDEN:
            ecsManager_.SetActive(entity, ecs::EntityStatus::INACTIVE);
            ecsManager_.SetEntityVisible(entity, ecs::EntityStatus::INACTIVE);
            break;
        case chunk::ChunkStatus::ACTIVE:
            ecsManager_.SetActive(entity, ecs::EntityStatus::ACTIVE);
            ecsManager_.SetEntityVisible(entity, ecs::EntityStatus::ACTIVE);
            break;
        default: ;
        }
    }
    pok_EndProfiling(Active_Chunk);
    pok_EndProfiling(Chunk_System);
}

void ChunksSystem::OnEntityDestroy(const ecs::EntityIndex entityIndex)
{
    destroyedEntities_.insert(entityIndex);
}

void ChunksSystem::OnEntityAddComponent(
    const ecs::EntityIndex entityIndex,
    const ecs::ComponentMask component)
{
    if (ecs::ComponentType::TRANSFORM == (component & ecs::ComponentType::TRANSFORM)) {
        newEntities_.insert(entityIndex);

        if (maxEntityIndex_ < entityIndex + 1) { maxEntityIndex_ = entityIndex + 1; }
    }

    if (component == ecs::ComponentType::RIGIDBODY) {
		if (updateEntities_.exist(entityIndex)) { return; }
        updateEntities_.insert(entityIndex);

        if (maxEntityIndex_ < entityIndex + 1) { maxEntityIndex_ = entityIndex + 1; }

        if(updateEntities_.size() == updateEntities_.capacity()) {
			updateEntities_.reserve(updateEntities_.size() * 2);
        }
    }

    if(maxEntityIndex_ > chunkIndex_.size()) {
		chunkIndex_.resize(chunkIndex_.size() * 2);
    }

    if(maxEntityIndex_ > chunkIndex_.size()) {
		chunkIndex_.resize(chunkIndex_.size() * 2);
		updateEntities_.reserve(updateEntities_.size() * 2);
    }
}

void ChunksSystem::OnUnloadScene()
{
    destroyedEntities_.clear();
    updateEntities_.clear();
}

void ChunksSystem::OnChangeActiveChunk(const chunk::ChunkBitSet chunkIndex)
{
    for (ecs::EntityIndex i = 0; i < maxEntityIndex_; i++) {
		if (updateEntities_.exist(i)) { continue; }

        if (ecsManager_.HasComponent(i, ecs::ComponentType::COLLIDER)) {
            chunkIndex_[i] = chunksManager_.AddEntity(
                collidersManager_.GetComponent(i).ComputeAABB(
                    transformsManager_.GetWorldPosition(i),
                    transformsManager_.GetWorldScale(i),
                    transformsManager_.GetWorldRotation(i)));
        } else {
            chunkIndex_[i] = chunksManager_.AddEntity(transformsManager_.GetWorldPosition(i));
        }

        if (chunkIndex_[i] == chunk::Chunk::kNullIndex) {
            ecsManager_.SetActive(i, ecs::EntityStatus::INACTIVE);
            ecsManager_.SetEntityVisible(i, ecs::EntityStatus::INACTIVE);
            continue;
        };

        switch (chunksManager_.GetChunkStatus(chunkIndex_[i])) {
        case chunk::ChunkStatus::VISIBLE:
            ecsManager_.SetActive(i, ecs::EntityStatus::INACTIVE);
            ecsManager_.SetEntityVisible(i, ecs::EntityStatus::ACTIVE);
            break;
        case chunk::ChunkStatus::HIDDEN:
            ecsManager_.SetActive(i, ecs::EntityStatus::INACTIVE);
            ecsManager_.SetEntityVisible(i, ecs::EntityStatus::INACTIVE);
            break;
        case chunk::ChunkStatus::ACTIVE:
            ecsManager_.SetActive(i, ecs::EntityStatus::ACTIVE);
            ecsManager_.SetEntityVisible(i, ecs::EntityStatus::ACTIVE);
            break;
        default: ;
        }
    }
}
} //namespace poke
