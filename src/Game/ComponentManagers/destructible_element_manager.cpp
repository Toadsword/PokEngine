#include <Game/ComponentManagers/destructible_element_manager.h>
#include <Game/Ecs/game_archetype.h>

namespace poke::game {
void DestructibleElementManager::ResizeEntities(const std::size_t size) {
	destructibleElements_.resize(size);
}

void DestructibleElementManager::ClearEntity(const ecs::EntityIndex entityIndex) {
	destructibleElements_[entityIndex] = DestructibleElement();
}

DestructibleElement DestructibleElementManager::GetComponent(const ecs::EntityIndex entityIndex) {
	return destructibleElements_[entityIndex];
}

void DestructibleElementManager::SetWithArchetype(const ecs::EntityPool entityPool,
												  const ecs::Archetype& archetype) {
	const GameArchetype& gameArchetype = *static_cast<const GameArchetype*>(&archetype);

	for (ecs::EntityIndex entityIndex = entityPool.firstEntity; entityIndex < entityPool.lastEntity; entityIndex++) {
		destructibleElements_[entityIndex] = gameArchetype.destructibleElement;
	}
}

void DestructibleElementManager::SetComponent(const ecs::EntityIndex entityIndex, 
                                              const DestructibleElement& destructibleElement) {
	destructibleElements_[entityIndex] = destructibleElement;
}

void DestructibleElementManager::SetComponentFromJson(
    ecs::EntityIndex entityIndex,
    const json& componentJson)
{
	DestructibleElement destructibleElement;
	destructibleElement.SetFromJson(componentJson);
	SetComponent(entityIndex, destructibleElement);
}

json DestructibleElementManager::GetJsonFromComponent(
    ecs::EntityIndex entityIndex) {
	return destructibleElements_[entityIndex].ToJson();
}

void DestructibleElementManager::EraseEntities(
    const ecs::EntityPool pool,
    const size_t nbObjectToErase)
{
	destructibleElements_.erase(destructibleElements_.begin() + pool.firstEntity, destructibleElements_.begin() + pool.firstEntity + nbObjectToErase);
}

void DestructibleElementManager::InsertArchetype(
    const ecs::EntityIndex entity,
    const ecs::Archetype& archetype)
{
	const GameArchetype& gameArchetype = static_cast<const GameArchetype&>(archetype);
	destructibleElements_.insert(destructibleElements_.begin() + entity, gameArchetype.destructibleElement);
}
}// namespace poke::game
