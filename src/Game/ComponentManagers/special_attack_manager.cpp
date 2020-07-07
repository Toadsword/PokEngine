#include <Game/ComponentManagers/special_attack_manager.h>
#include <Game/ecs/game_archetype.h>

namespace poke::game {
void SpecialAttackManager::ResizeEntities(const std::size_t size) {
	specialAttacks_.resize(size);
}

void SpecialAttackManager::ClearEntity(const ecs::EntityIndex entityIndex) {
	specialAttacks_[entityIndex] = SpecialAttack();
}

SpecialAttack& SpecialAttackManager::GetComponent(const ecs::EntityIndex entityIndex) {
	return specialAttacks_[entityIndex];
}

void SpecialAttackManager::SetWithArchetype(const ecs::EntityPool entityPool, const ecs::Archetype& archetype) {
	const GameArchetype& gameArchetype = *static_cast<const GameArchetype*>(&archetype);

	for (ecs::EntityIndex entityIndex = entityPool.firstEntity; entityIndex < entityPool.lastEntity; entityIndex++) {
		specialAttacks_[entityIndex].expansionSpeed = gameArchetype.specialAttack.expansionSpeed;
		specialAttacks_[entityIndex].maxRadius = gameArchetype.specialAttack.maxRadius;
	}
}

void SpecialAttackManager::SetComponent(const ecs::EntityIndex entityIndex, const SpecialAttack& specialAttack) {
	specialAttacks_[entityIndex].expansionSpeed = specialAttack.expansionSpeed;
	specialAttacks_[entityIndex].maxRadius = specialAttack.maxRadius;
}

void SpecialAttackManager::SetComponentFromJson(
    ecs::EntityIndex entityIndex,
    const json& componentJson)
{
	SpecialAttack specialAttack;
	specialAttack.SetFromJson(componentJson);
	specialAttacks_[entityIndex] = specialAttack;
}

json SpecialAttackManager::GetJsonFromComponent(ecs::EntityIndex entityIndex) {
	return specialAttacks_[entityIndex].ToJson();
}

void SpecialAttackManager::EraseEntities(
    const ecs::EntityPool pool,
    const size_t nbObjectToErase)
{
	specialAttacks_.erase(specialAttacks_.begin() + pool.firstEntity, specialAttacks_.begin() + pool.firstEntity + nbObjectToErase);
}

void SpecialAttackManager::InsertArchetype(
    const ecs::EntityIndex entity,
    const ecs::Archetype& archetype)
{
	const auto& gameArchetype = static_cast<const GameArchetype&>(archetype);
	specialAttacks_.insert(specialAttacks_.begin() + entity, gameArchetype.specialAttack);
}
}
