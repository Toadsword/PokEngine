#include <Ecs/ComponentManagers/trail_renderer_manager.h>

namespace poke {
namespace ecs {

void TrailRendererManager::ResizeEntities(const std::size_t size) { trailRenderers_.resize(size); }

void TrailRendererManager::ClearEntity(const EntityIndex entityIndex)
{
    trailRenderers_[entityIndex].Clear();
}

void TrailRendererManager::SetWithArchetype(const EntityPool entityPool, const Archetype& archetype)
{
    for (auto entityIndex = entityPool.firstEntity;
         entityIndex < entityPool.lastEntity;
         entityIndex++) { trailRenderers_[entityIndex] = archetype.trailRenderer; }
}

void TrailRendererManager::InsertArchetype(const EntityIndex entity, const Archetype& archetype)
{
    trailRenderers_.insert(trailRenderers_.begin() + entity, archetype.trailRenderer);
}

void TrailRendererManager::EraseEntities(const EntityPool pool, const size_t nbObjectToErase)
{
    trailRenderers_.erase(
        trailRenderers_.begin() + pool.firstEntity,
        trailRenderers_.begin() + pool.firstEntity + nbObjectToErase);
}

const TrailRenderer& TrailRendererManager::GetComponent(const EntityIndex entityIndex) const
{
    return trailRenderers_[entityIndex];
}

void TrailRendererManager::SetComponent(
    const EntityIndex entityIndex,
    const TrailRenderer& trailRenderer) { trailRenderers_[entityIndex] = trailRenderer; }

void TrailRendererManager::SetComponentFromJson(
    const EntityIndex entityIndex,
    const json& componentJson) { trailRenderers_[entityIndex].SetFromJson(componentJson); }

json TrailRendererManager::GetJsonFromComponent(const EntityIndex entityIndex)
{
    return trailRenderers_[entityIndex].ToJson();
}
} //namespace ecs
} //namespace poke
