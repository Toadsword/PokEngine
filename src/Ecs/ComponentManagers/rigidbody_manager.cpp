#include <Ecs/ComponentManagers/rigidbody_manager.h>

namespace poke {
namespace ecs {

RigidbodyManager::RigidbodyManager() { }
RigidbodyManager::~RigidbodyManager() {}

void RigidbodyManager::ClearEntity(const EntityIndex entityIndex)
{
    rigidbodies_[entityIndex].type = physics::RigidbodyType::STATIC;
}

void RigidbodyManager::ResizeEntities(const std::size_t size)
{
    rigidbodies_.resize(size, physics::kEmptyRigidbody);
}

void RigidbodyManager::SetWithArchetype(
    const EntityPool entityPool,
    const Archetype& archetype)
{
    for (EntityIndex entityIndex = entityPool.firstEntity;
         entityIndex < entityPool.lastEntity; entityIndex++) {
        rigidbodies_[entityIndex] = archetype.rigidbody;
    }
}

void RigidbodyManager::InsertArchetype(
    const EntityIndex entity,
    const Archetype& archetype)
{
    rigidbodies_.insert(rigidbodies_.begin() + entity, archetype.rigidbody);
}

void RigidbodyManager::EraseEntities(
    const EntityPool pool,
    const size_t nbObjectToErase)
{
    rigidbodies_.erase(
        rigidbodies_.begin() + pool.firstEntity,
        rigidbodies_.begin() + pool.firstEntity + nbObjectToErase);
}

const physics::Rigidbody& RigidbodyManager::GetComponent(
    const EntityIndex entityIndex) const { return rigidbodies_[entityIndex]; }

void RigidbodyManager::SetComponent(
    const EntityIndex entityIndex,
    const physics::Rigidbody& rigidbody)
{
    rigidbodies_[entityIndex] = rigidbody;
}

void RigidbodyManager::SetComponentFromJson(
    const EntityIndex entityIndex,
    const json& componentJson)
{
    rigidbodies_[entityIndex].SetFromJson(componentJson);
}

json RigidbodyManager::GetJsonFromComponent(const EntityIndex entityIndex)
{
    return rigidbodies_[entityIndex].ToJson();
}
} //namespace ecs
} //namespace poke
