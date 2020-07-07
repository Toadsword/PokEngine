#include <Game/ComponentManagers/spline_state_manager.h>

#include "Game/Ecs/game_archetype.h"

namespace poke {
namespace game {

void SplineStateManager::ResizeEntities(size_t size) {
	splineStates_.resize(size);
}

void SplineStateManager::ClearEntity(const ecs::EntityIndex entityIndex) {
	splineStates_[entityIndex].pointStates.clear();
}

void SplineStateManager::SetWithArchetype(ecs::EntityPool entityPool, const ecs::Archetype& archetype) {
	const GameArchetype& gameArchetype = *static_cast<const GameArchetype*>(&archetype);

	for (ecs::EntityIndex entityIndex = entityPool.firstEntity; entityIndex < entityPool.lastEntity; entityIndex++) {
		splineStates_[entityIndex] = gameArchetype.splineStates;
	}
}

void SplineStateManager::EraseEntities(ecs::EntityPool pool, size_t nbObjectToErase) {
	splineStates_.erase(
		splineStates_.begin() + pool.firstEntity,
		splineStates_.begin() + pool.firstEntity + nbObjectToErase);
}

void SplineStateManager::InsertArchetype(const ecs::EntityIndex entity, const ecs::Archetype& archetype) {
	const GameArchetype& gameArchetype = static_cast<const GameArchetype&>(archetype);

	splineStates_.insert(splineStates_.begin() + entity, gameArchetype.splineStates);
}

void SplineStateManager::SetComponentFromJson(const ecs::EntityIndex entityIndex, const json& componentJson) {
	splineStates_[entityIndex].SetFromJson(componentJson);
}
json SplineStateManager::GetJsonFromComponent(const ecs::EntityIndex entityIndex) {
	return splineStates_[entityIndex].ToJson();
}

SplineStates SplineStateManager::GetComponent(const ecs::EntityIndex entityIndex) {
	return splineStates_[entityIndex];
}

void SplineStateManager::SetComponent(const ecs::EntityIndex entityIndex, const SplineStates& splineStates) {
	splineStates_[entityIndex] = splineStates;
}
}//namespace game
}//namespace poke