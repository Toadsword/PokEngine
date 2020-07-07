#include <Ecs/ComponentManagers/spline_followers_manager.h>

namespace poke::ecs {
void SplineFollowersManager::ResizeEntities(const std::size_t size)
{
    splineFollowers_.resize(size);
}

void SplineFollowersManager::ClearEntity(const EntityIndex entityIndex)
{
    splineFollowers_[entityIndex].spline = math::CubicHermiteSpline();
}

void SplineFollowersManager::SetWithArchetype(
    const EntityPool entityPool,
    const Archetype& archetype)
{
    for (EntityIndex entityIndex = entityPool.firstEntity;
         entityIndex < entityPool.lastEntity;
         entityIndex++) { splineFollowers_[entityIndex] = archetype.spline; }
}

void SplineFollowersManager::InsertArchetype(
    const EntityIndex entity,
    const Archetype& archetype)
{
    splineFollowers_.insert(splineFollowers_.begin() + entity, archetype.spline);
}

void SplineFollowersManager::EraseEntities(const EntityPool pool, const size_t nbObjectToErase)
{
    splineFollowers_.erase(
        splineFollowers_.begin() + pool.firstEntity,
        splineFollowers_.begin() + pool.firstEntity + nbObjectToErase);
}

const SplineFollower& SplineFollowersManager::GetComponent(const EntityIndex entityIndex) const
{
    return splineFollowers_[entityIndex];
}

void SplineFollowersManager::SetComponent(
    const EntityIndex entityIndex,
    const SplineFollower& spline) { splineFollowers_[entityIndex] = spline; }

void SplineFollowersManager::SetComponentFromJson(
    const EntityIndex entityIndex,
    const json& componentJson)
{
	splineFollowers_[entityIndex].SetFromJson(componentJson);
}

json SplineFollowersManager::GetJsonFromComponent(const EntityIndex entityIndex)
{
    return splineFollowers_[entityIndex].ToJson();
}
} // namespace poke::ecs
