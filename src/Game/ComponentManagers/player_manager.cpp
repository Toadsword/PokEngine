#include <Game/ComponentManagers/player_manager.h>
#include <Utility/log.h>
#include <Game/Ecs/game_archetype.h>

namespace poke::game {
void PlayerManager::ResizeEntities(const std::size_t size) {
	players_.resize(size);
}

void PlayerManager::ClearEntity(const ecs::EntityIndex entityIndex) {
	players_[entityIndex] = Player();
}

Player PlayerManager::GetComponent(const ecs::EntityIndex entityIndex) {
	return players_[entityIndex];
}

void PlayerManager::SetWithArchetype(const ecs::EntityPool entityPool, const ecs::Archetype& archetype) {
	const GameArchetype& gameArchetype = *static_cast<const GameArchetype*>(&archetype);

	for (ecs::EntityIndex entityIndex = entityPool.firstEntity; entityIndex < entityPool.lastEntity; entityIndex++) {
		players_[entityIndex] = gameArchetype.player;
	}
}

void PlayerManager::SetComponent(const ecs::EntityIndex entityIndex, const Player& player) {
	players_[entityIndex] = player;
}

void PlayerManager::SetComponentFromJson(
    ecs::EntityIndex entityIndex,
    const json& componentJson)
{
	Player player;
	player.SetFromJson(componentJson);
	players_[entityIndex] = player;
}

json PlayerManager::GetJsonFromComponent(ecs::EntityIndex entityIndex) {
    return players_[entityIndex].ToJson();
}

void PlayerManager::EraseEntities(
    const ecs::EntityPool pool,
    const size_t nbObjectToErase)
{
	players_.erase(players_.begin() + pool.firstEntity, players_.begin() + pool.firstEntity + nbObjectToErase);
}

void PlayerManager::InsertArchetype(
    const ecs::EntityIndex entity,
    const ecs::Archetype& archetype)
{
	const GameArchetype& gameArchetype = static_cast<const GameArchetype&>(archetype);

	players_.insert(players_.begin() + entity, gameArchetype.player);
}
}// namespace poke::game