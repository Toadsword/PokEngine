#include <Ecs/ComponentManagers/colliders_manager.h>

#include <Ecs/ComponentManagers/rigidbody_manager.h>

namespace poke {
namespace ecs {
CollidersManager::CollidersManager() { }

void CollidersManager::ClearEntity(const EntityIndex entityIndex)
{
    colliders_[entityIndex].isTrigger = true;
}


void CollidersManager::ResizeEntities(const std::size_t size)
{
    colliders_.resize(size);
}

void CollidersManager::SetWithArchetype(
    const EntityPool entityPool,
    const Archetype& archetype)
{
    for (EntityIndex entityIndex = entityPool.firstEntity;
         entityIndex < entityPool.lastEntity; entityIndex++) {
        colliders_[entityIndex] = archetype.collider;
    }
}

void CollidersManager::InsertArchetype(
    const EntityIndex entity,
    const Archetype& archetype)
{
    colliders_.insert(colliders_.begin() + entity, archetype.collider);
}

void CollidersManager::EraseEntities(
    const EntityPool pool,
    const size_t nbObjectToErase)
{
    for (size_t i = 0; i < nbObjectToErase; i++) {
        colliders_.erase(colliders_.begin() + pool.firstEntity);
    }
}

void CollidersManager::SetComponent(
    const EntityIndex entityIndex,
    const physics::Collider& collider)
{
    colliders_[static_cast<int>(entityIndex)] = collider;
}

const physics::Collider& CollidersManager::GetComponent(
    const EntityIndex entityIndex) const { return colliders_[entityIndex]; }

void CollidersManager::SetComponentFromJson(
    const EntityIndex entityIndex,
    const json& componentJson)
{
    colliders_[entityIndex].SetFromJson(componentJson);
}

json CollidersManager::GetJsonFromComponent(const EntityIndex entityIndex)
{
    return colliders_[entityIndex].ToJson();
}
} //namespace ecs
} //namespace poke
