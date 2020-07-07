#include <Game/ComponentManagers/missiles_manager.h>
#include <Game/Ecs/game_archetype.h>

namespace poke {
namespace game {
void MissilesManager::ResizeEntities(size_t size) {
	missiles_.resize(size);
}

void MissilesManager::ClearEntity(const ecs::EntityIndex entityIndex) {
	missiles_[entityIndex] = Missile();
}


Missile MissilesManager::GetComponent(ecs::EntityIndex entityIndex) {
	return missiles_[entityIndex];
}

void MissilesManager::SetComponent(ecs::EntityIndex entityIndex, Missile missile) {
	missiles_[entityIndex] = missile;
}


void MissilesManager::SetWithArchetype(const ecs::EntityPool entityPool, const ecs::Archetype& archetype) {
	const GameArchetype& gameArchetype = *static_cast<const GameArchetype*>(&archetype);

	for (ecs::EntityIndex entityIndex = entityPool.firstEntity; entityIndex < entityPool.lastEntity; entityIndex++) {
		missiles_[entityIndex] = gameArchetype.missile;
	}
}

void MissilesManager::EraseEntities(
    ecs::EntityPool pool,
    size_t nbObjectToErase) {
	missiles_.erase(missiles_.begin() + pool.firstEntity, missiles_.begin() + pool.firstEntity + nbObjectToErase);
}

void MissilesManager::InsertArchetype(
    ecs::EntityIndex entity,
    const ecs::Archetype& archetype) {
	const GameArchetype& gameArchetype = static_cast<const GameArchetype&>(archetype);

	missiles_.insert(missiles_.begin() + entity, gameArchetype.missile);
}

void MissilesManager::SetComponentFromJson(
    ecs::EntityIndex entityIndex,
    const json& componentJson) {
	Missile missile;
	missile.SetFromJson(componentJson);
	missiles_[entityIndex] = missile;
}

json MissilesManager::GetJsonFromComponent(ecs::EntityIndex entityIndex){
	return missiles_[entityIndex].ToJson();
}
}//namespace poke
}//namespace poke