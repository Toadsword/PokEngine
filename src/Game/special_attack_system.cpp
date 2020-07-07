#include <Game/special_attack_system.h>
#include <Utility/log.h>
#include <Game/game.h>
#include <Utility/profiler.h>

namespace poke::game
{
SpecialAttackSystem::SpecialAttackSystem(Engine& engine, Game& game)
	: GameSystem(engine, game),
	  time_(Time::Get()),
	  specialAttackManager_(ecsManager_.GetComponentsManager<SpecialAttackManager>()),
	  transformsManager_(ecsManager_.GetComponentsManager<ecs::TransformsManager>()),
	  colliderManager_(ecsManager_.GetComponentsManager<ecs::CollidersManager>()),
	  gizmoCommandBuffer_(GraphicsEngineLocator::Get().GetGizmoCommandBuffer()),
	  destructibleElementManager_(ecsManager_.GetComponentsManager<DestructibleElementManager>())
{
	game.RegisterObserverUpdate([this] { this->OnUpdate(); });

	ecsManager_.RegisterObserverAddComponent(
		[this](ecs::EntityIndex entityIndex, ecs::ComponentMask component)
		{
			OnAddComponent(entityIndex, component);
		});
	ecsManager_.RegisterObserverRemoveComponent(
		[this](ecs::EntityIndex entityIndex, ecs::ComponentMask component)
		{
			OnRemoveComponent(entityIndex, component);
		});
}

void SpecialAttackSystem::OnUpdate() {
	pok_BeginProfiling(Special_Attack_System, 0);
	for (auto it = entityIndexes_.begin(); it != entityIndexes_.end(); ++it) {
		SpecialAttack specialAttack = specialAttackManager_.GetComponent(*it);
		math::Transform transform = transformsManager_.GetComponent(*it);

        const float newRadius = transform.GetLocalScale().x + specialAttack.maxRadius / specialAttack.expansionSpeed;

		if (newRadius > specialAttack.maxRadius) {
			specialAttack.expansionSpeed = 0.0f;
			specialAttack.maxRadius = 0.1f;
			transform.SetLocalScale({ 0.1f });
		} else {
			transform.SetLocalScale({ newRadius });
		}

		transform.SetLocalPosition({ 0.0f });
		transformsManager_.SetComponent(*it, transform);
		specialAttackManager_.SetComponent(*it, specialAttack);
	}
	pok_EndProfiling(Special_Attack_System);
}

void SpecialAttackSystem::OnTriggerEnter(const ecs::EntityIndex entityIndex, const physics::Collision collision) const {
	if (ecsManager_.HasComponent(collision.otherEntity, ecs::ComponentType::DESTRUCTIBLE_ELEMENT)) {
		DestructibleElement destructibleElement = destructibleElementManager_.GetComponent(collision.otherEntity);
		if (destructibleElement.isIndestructible) { return; }
		destructibleElement.life = 0;
		destructibleElementManager_.SetComponent(collision.otherEntity, destructibleElement);
	}
}

void SpecialAttackSystem::OnAddComponent(
	const ecs::EntityIndex entityIndex,
	const ecs::ComponentMask component)
{
	//Check if the entity have the component required
	if ((component & ecs::ComponentType::SPECIAL_ATTACK) != ecs::ComponentType::SPECIAL_ATTACK) { return; }

	//check if the entity has already been added the the system by the past
	if (!registeredEntities_.exist(entityIndex)) {
		//Add the entity to the list of registered entities and sort it
		registeredEntities_.insert(entityIndex);

		ecsManager_.RegisterObserverTriggerEnter(
			entityIndex,
			[this](ecs::EntityIndex entityIndex, physics::Collision collision)
		{this->OnTriggerEnter(entityIndex, collision); });
	}

	//Check if the entity is active
	if (!ecsManager_.HasComponent(entityIndex, ecs::EntityFlag::IS_ACTIVE)) { return; }

    if(entityIndexes_.exist(entityIndex)) { return; }

	//Add the entity to the list of entities and sort it
	entityIndexes_.insert(entityIndex);
}

void SpecialAttackSystem::OnRemoveComponent(
	const ecs::EntityIndex entityIndex,
	const ecs::ComponentMask component)
{
	//Check if the entity have the component required
	if ((component & ecs::ComponentType::SPECIAL_ATTACK) != ecs::ComponentType::SPECIAL_ATTACK) {
		return;
	}
	//Check if the entity is in the list of entities
	if (!entityIndexes_.exist(entityIndex)) { return; }

	const auto it = entityIndexes_.find(entityIndex);

	//remove if one of the removed components
	entityIndexes_.erase(it);
}
	
void SpecialAttackSystem::OnEntitySetActive(const ecs::EntityIndex entityIndex)
{
	//Check if the activated entity is a missile
	if (!ecsManager_.HasComponent(entityIndex, ecs::ComponentType::SPECIAL_ATTACK)) { return; }

	if (entityIndexes_.exist(entityIndex)) { return; }

	//Add the entity to the list of entities and sort it
	entityIndexes_.insert(entityIndex);
}
	
void SpecialAttackSystem::OnEntitySetInactive(const ecs::EntityIndex entityIndex)
{
	//Check if the entity is in the list of entities
	if (!entityIndexes_.exist(entityIndex)) { return; }

	const auto it = entityIndexes_.find(entityIndex);

	//remove if one of the removed components
	entityIndexes_.erase(it);
}
} // namespace poke::game
