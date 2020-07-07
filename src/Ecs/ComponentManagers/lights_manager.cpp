#include <Ecs/ComponentManagers/lights_manager.h>

namespace poke {
namespace ecs {

LightsManager::LightsManager() {}

void LightsManager::ClearEntity(const EntityIndex entityIndex)
{
    //lights_[entityIndex].lightType = graphics::LightType::DIRECTIONAL;
}

void LightsManager::ResizeEntities(const size_t size)
{
    lights_.resize(size);
}

void LightsManager::SetWithArchetype(
    const EntityPool entityPool,
    const Archetype& archetype)
{
    for (auto entityIndex = entityPool.firstEntity;
		entityIndex < entityPool.lastEntity; entityIndex++) {
		lights_[entityIndex]= archetype.light;
	}
}

void LightsManager::InsertArchetype(
    const EntityIndex entity,
    const Archetype& archetype)
{
	lights_.insert(lights_.begin() + entity, archetype.light);
}

void LightsManager::EraseEntities(const EntityPool pool, const size_t nbObjectToErase)
{
	lights_.erase(lights_.begin() + pool.firstEntity, lights_.begin() + pool.firstEntity + nbObjectToErase);
}

void LightsManager::SetComponentFromJson(
    const EntityIndex entityIndex,
    const json& componentJson)
{
	lights_[entityIndex].SetFromJson(componentJson);
}

json LightsManager::GetJsonFromComponent(const EntityIndex entityIndex)
{
	return lights_[entityIndex].ToJson();
}

const Light& LightsManager::GetComponent(const EntityIndex entityIndex) const
{
    return lights_[entityIndex];
}

void LightsManager::SetComponent(const EntityIndex entityIndex, const Light& light)
{
    lights_[entityIndex] = light;
}
} //namespace ecs
} //namespace poke