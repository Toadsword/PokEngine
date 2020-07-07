#include <Game/ComponentManagers/jiggle_manager.h>

#include <Game/Ecs/game_archetype.h>

namespace poke {
namespace game {

void JiggleManager::ResizeEntities(const size_t size) {
	jiggles_.resize(size);
}

void JiggleManager::ClearEntity(const ecs::EntityIndex entityIndex) {
	jiggles_[entityIndex] = Jiggle();
}

void JiggleManager::SetWithArchetype(
    const ecs::EntityPool entityPool,
    const ecs::Archetype& archetype) {
    const GameArchetype& gameArchetype = 
		*static_cast<const GameArchetype*>(&archetype);
    for (ecs::EntityIndex entityIndex = entityPool.firstEntity;
         entityIndex < entityPool.lastEntity; entityIndex++) {
        jiggles_[entityIndex] = gameArchetype.jiggle;
    }
}

void JiggleManager::EraseEntities(const ecs::EntityPool pool, const size_t nbObjectToErase) {
	jiggles_.erase(
		jiggles_.begin() + pool.firstEntity,
		jiggles_.begin() + pool.firstEntity + nbObjectToErase);
}

void JiggleManager::InsertArchetype(const ecs::EntityIndex entity, const ecs::Archetype& archetype) {
	const GameArchetype& gameArchetype = static_cast<const GameArchetype&>(archetype);

	jiggles_.insert(jiggles_.begin() + entity, gameArchetype.jiggle);
}

void JiggleManager::SetComponentFromJson(const ecs::EntityIndex entityIndex, const json& componentJson) {
	jiggles_[entityIndex].SetFromJson(componentJson);
}
json JiggleManager::GetJsonFromComponent(const ecs::EntityIndex entityIndex) {
	return jiggles_[entityIndex].ToJson();
}

Jiggle JiggleManager::GetComponent(const ecs::EntityIndex entityIndex) {
	return jiggles_[entityIndex];
}

void JiggleManager::SetComponent(const ecs::EntityIndex entityIndex, const Jiggle& splineStates) {
	jiggles_[entityIndex] = splineStates;
}
}//namespace game
}//namespace poke