#include <Game/ComponentManagers/enemies_manager.h>
#include <Game/Ecs/game_archetype.h>

namespace poke {
namespace game {
void EnemiesManager::ResizeEntities(size_t size) {
	enemies_.resize(size);
}

void EnemiesManager::ClearEntity(const ecs::EntityIndex entityIndex) {
	enemies_[entityIndex] = Enemy();
}

void EnemiesManager::SetWithArchetype(const ecs::EntityPool entityPool, const ecs::Archetype& archetype) {
	const GameArchetype& gameArchetype = *static_cast<const GameArchetype*>(&archetype);

	for (ecs::EntityIndex entityIndex = entityPool.firstEntity; entityIndex < entityPool.lastEntity; entityIndex++) {
		enemies_[entityIndex] = gameArchetype.enemy;
	}
}

Enemy EnemiesManager::GetComponent(const ecs::EntityIndex entityIndex) {
	return enemies_[entityIndex];
}
void EnemiesManager::SetComponent(const ecs::EntityIndex entityIndex, Enemy enemy) {
	enemies_[entityIndex] = enemy;
}

void EnemiesManager::EraseEntities(ecs::EntityPool pool, size_t nbObjectToErase) {
	enemies_.erase(
		enemies_.begin() + pool.firstEntity,
		enemies_.begin() + pool.firstEntity + nbObjectToErase);
}

void EnemiesManager::InsertArchetype(
	ecs::EntityIndex entity,
	const ecs::Archetype& archetype)
{
	const GameArchetype& gameArchetype = static_cast<const GameArchetype&>(archetype);
	enemies_.insert(enemies_.begin() + entity, gameArchetype.enemy);
}

json EnemiesManager::GetJsonFromComponent(ecs::EntityIndex entityIndex) {
	return enemies_[entityIndex].ToJson();
}

void EnemiesManager::SetComponentFromJson(
	ecs::EntityIndex entityIndex,
	const json& componentJson)
{
	Enemy enemy;
	enemy.SetFromJson(componentJson);
	enemies_[entityIndex] = enemy;
}
}//namespace game
}//namespace poke