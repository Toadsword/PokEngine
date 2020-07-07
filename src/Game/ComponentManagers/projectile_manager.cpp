#include <Game/ComponentManagers/projectile_manager.h>
#include <Game/Ecs/game_archetype.h>

namespace poke::game {
void ProjectileManager::ResizeEntities(const std::size_t size) {
	projectiles_.resize(size);
}

void ProjectileManager::ClearEntity(const ecs::EntityIndex entityIndex) {
	projectiles_[entityIndex] = Projectile();
}

Projectile ProjectileManager::GetComponent(const ecs::EntityIndex entityIndex) {
	return projectiles_[entityIndex];
}

void ProjectileManager::SetWithArchetype(const ecs::EntityPool entityPool,
										 const ecs::Archetype& archetype) {
	const GameArchetype& gameArchetype = *static_cast<const GameArchetype*>(&archetype);

	for (ecs::EntityIndex entityIndex = entityPool.firstEntity; entityIndex < entityPool.lastEntity; entityIndex++) {
		projectiles_[entityIndex] = gameArchetype.projectile;
	}
}

void ProjectileManager::SetComponent(const ecs::EntityIndex entityIndex, 
                                     const Projectile& projectile) {
	projectiles_[entityIndex] = projectile;
}

void ProjectileManager::SetComponentFromJson(
    ecs::EntityIndex entityIndex,
    const json& componentJson)
{
	Projectile projectile;
	projectile.SetFromJson(componentJson);
	projectiles_[entityIndex] = projectile;
}

json ProjectileManager::GetJsonFromComponent(ecs::EntityIndex entityIndex) {
	return projectiles_[entityIndex].ToJson();
}

void ProjectileManager::EraseEntities(
    const ecs::EntityPool pool,
    const size_t nbObjectToErase)
{
	projectiles_.erase(projectiles_.begin() + pool.firstEntity, projectiles_.begin() + pool.firstEntity + nbObjectToErase);
}

void ProjectileManager::InsertArchetype(
    const ecs::EntityIndex entity,
    const ecs::Archetype& archetype)
{
	const GameArchetype& gameArchetype = static_cast<const GameArchetype&>(archetype);
	projectiles_.insert(projectiles_.begin() + entity, gameArchetype.projectile);
}
}// namespace poke::game
