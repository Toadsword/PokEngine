#include <Game/ComponentManagers/weapon_manager.h>
#include <Game/Ecs/game_archetype.h>

namespace poke::game {
void WeaponManager::ResizeEntities(const std::size_t size) {
	weapons_.resize(size);
}

void WeaponManager::ClearEntity(const ecs::EntityIndex entityIndex) {
	weapons_[entityIndex] = Weapon();
}

Weapon WeaponManager::GetComponent(const ecs::EntityIndex entityIndex) {
	return weapons_[entityIndex];
}

void WeaponManager::SetWithArchetype(const ecs::EntityPool entityPool, const ecs::Archetype& archetype) {
	const GameArchetype& gameArchetype = *static_cast<const GameArchetype*>(&archetype);

	for (ecs::EntityIndex entityIndex = entityPool.firstEntity; entityIndex < entityPool.lastEntity; entityIndex++) {
		weapons_[entityIndex] = gameArchetype.weapon;
	}
}

void WeaponManager::SetComponent(const ecs::EntityIndex entityIndex, const Weapon& weapon) {
	weapons_[entityIndex] = weapon;
}

void WeaponManager::SetComponentFromJson(
    ecs::EntityIndex entityIndex,
    const json& componentJson)
{
	Weapon weapon;
	weapon.SetFromJson(componentJson);
	weapons_[entityIndex] = weapon;
}

json WeaponManager::GetJsonFromComponent(ecs::EntityIndex entityIndex) {
	return weapons_[entityIndex].ToJson();
}

void WeaponManager::EraseEntities(
    const ecs::EntityPool pool,
    const size_t nbObjectToErase)
{
	weapons_.erase(weapons_.begin() + pool.firstEntity, weapons_.begin() + pool.firstEntity + nbObjectToErase);
}

void WeaponManager::InsertArchetype(
    const ecs::EntityIndex entity,
    const ecs::Archetype& archetype)
{
	const GameArchetype& gameArchetype = static_cast<const GameArchetype&>(archetype);
	weapons_.insert(weapons_.begin() + entity, gameArchetype.weapon);
}
}
