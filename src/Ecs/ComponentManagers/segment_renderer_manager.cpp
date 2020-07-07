#include <Ecs/ComponentManagers/segment_renderer_manager.h>

namespace poke {
namespace ecs {
void SegmentRendererManager::ResizeEntities(const std::size_t size)
{
    segmentRenderers_.resize(size);
}

void SegmentRendererManager::ClearEntity(const EntityIndex entityIndex)
{
    //segmentRenderers_[entityIndex].Clear();
}

void SegmentRendererManager::SetWithArchetype(
    const EntityPool entityPool,
    const Archetype& archetype)
{
    for (auto entityIndex = entityPool.firstEntity;
         entityIndex < entityPool.lastEntity;
         entityIndex++) { segmentRenderers_[entityIndex] = archetype.segmentRenderer; }
}

void SegmentRendererManager::InsertArchetype(const EntityIndex entity, const Archetype& archetype)
{
    segmentRenderers_.insert(segmentRenderers_.begin() + entity, archetype.segmentRenderer);
}

void SegmentRendererManager::EraseEntities(const EntityPool pool, const size_t nbObjectToErase)
{
    segmentRenderers_.erase(
        segmentRenderers_.begin() + pool.firstEntity,
        segmentRenderers_.begin() + pool.firstEntity + nbObjectToErase);
}

const SegmentRenderer& SegmentRendererManager::GetComponent(const EntityIndex entityIndex) const
{
    return segmentRenderers_[entityIndex];
}

void SegmentRendererManager::SetComponent(
    const EntityIndex entityIndex,
    const SegmentRenderer& segmentRenderer) { segmentRenderers_[entityIndex] = segmentRenderer; }

void SegmentRendererManager::SetComponentFromJson(
    const EntityIndex entityIndex,
    const json& componentJson) { segmentRenderers_[entityIndex].SetFromJson(componentJson); }

json SegmentRendererManager::GetJsonFromComponent(const EntityIndex entityIndex)
{
    return segmentRenderers_[entityIndex].ToJson();
}
} //namespace ecs
} //namespace poke
