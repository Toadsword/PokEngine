#include <Game/projectile_system.h>

#include <algorithm>

#include <Utility/log.h>
#include <Utility/profiler.h>
#include <CoreEngine/engine.h>
#include <Game/game.h>
#include <Game/Components/projectile.h>

namespace poke::game {
ProjectileSystem::ProjectileSystem(Engine& engine, Game& game)
	: GameSystem(engine, game),
	time_(Time::Get()),
	projectileManager_(ecsManager_.GetComponentsManager<ProjectileManager>()),
	transformsManager_(ecsManager_.GetComponentsManager<ecs::TransformsManager>()),
	rigidbodyManager_(ecsManager_.GetComponentsManager<ecs::RigidbodyManager>()),
	destructibleElementManager_(ecsManager_.GetComponentsManager<DestructibleElementManager>()),
	particleSystemsManager_(ecsManager_.GetComponentsManager<ecs::ParticleSystemsManager>()),
	gizmoCommandBuffer_(GraphicsEngineLocator::Get().GetGizmoCommandBuffer()),
	gamePrefabsManager_(static_cast<GamePrefabsManager&>(PrefabsManagerLocator::Get()))
{
	game.RegisterObserverUpdate([this] { this->OnUpdate(); });

	ObserveEntitySetActive();
	ObserveEntitySetInactive();
	
	ecsManager_.RegisterObserverAddComponent(
		[this](ecs::EntityIndex entityIndex, ecs::ComponentMask component) {
		OnAddComponent(entityIndex, component);
	});
	ecsManager_.RegisterObserverRemoveComponent(
		[this](ecs::EntityIndex entityIndex, ecs::ComponentMask component) {
		OnRemoveComponent(entityIndex, component);
	});


	ObserveLoadScene();

}

void ProjectileSystem::OnLoadScene() {
	gamePrefabsManager_.AddPrefab("ParticleProj");
}

void ProjectileSystem::OnUpdate() {
	pok_BeginProfiling(Projectile_System, 0);
	std::vector<ecs::EntityIndex> entitiesToDestroy;
    for(const ecs::EntityIndex projectileIndex : entityIndexes_) {
		const Projectile projectile = projectileManager_.GetComponent(projectileIndex);

		if (time_.GetTime() >= projectile.shootAtTime + projectile.durationLifeTime) {
			entitiesToDestroy.emplace_back(projectileIndex);
		}
    }

    for (auto toDestroy : entitiesToDestroy) {
		DestroyProjectile(toDestroy);
    }
	entitiesToDestroy.clear();
	pok_EndProfiling(Projectile_System);
}

void ProjectileSystem::OnAddComponent(
	const ecs::EntityIndex entityIndex,
	const ecs::ComponentMask component)
{
	//Check if the entity have the component required
	if ((component & ecs::ComponentType::PROJECTILE) != ecs::ComponentType::PROJECTILE) { return; }

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

void ProjectileSystem::OnRemoveComponent(
	const ecs::EntityIndex entityIndex,
	const ecs::ComponentMask component)
{
	if ((component & ecs::ComponentType::PROJECTILE) != ecs::ComponentType::PROJECTILE) {
		return;
	}
	//Check if the entity is in the list of entities
	if (!entityIndexes_.exist(entityIndex)) { return; }

	const auto it = entityIndexes_.find(entityIndex);

	//remove if one of the removed components
	entityIndexes_.erase(it);
}

void ProjectileSystem::OnTriggerEnter(const ecs::EntityIndex entityIndex, const physics::Collision collision) {
	if (ecsManager_.HasComponent(collision.otherEntity, ecs::ComponentType::DESTRUCTIBLE_ELEMENT)) {
		Projectile projectile = projectileManager_.GetComponent(entityIndex);
		DestructibleElement destructibleElement = destructibleElementManager_.GetComponent(collision.otherEntity);
		if (destructibleElement.isIndestructible) { return; }
        if(projectile.origin != ecs::kNoEntity) {
		    destructibleElement.life -= projectileManager_.GetComponent(entityIndex).damage;
        	if(destructibleElement.life <= 0)
				destructibleElement.destroyedBy = projectile.origin;
		    destructibleElementManager_.SetComponent(collision.otherEntity, destructibleElement);
		    DestroyProjectile(entityIndex, destructibleElement.type);
        }
	}
}

void ProjectileSystem::DestroyProjectile(
	const ecs::EntityIndex entityIndex, DestructibleElement::Type type) {
	LogDebug("Create Projectile Particule");
	switch (type) { //TODO(@Luca) Impact different depend type
	default:
		//particleSystemsManager_.SetComponent(particleIndex, particleSystem_);
		break;
	}
	math::Transform transform;
    transform.SetLocalPosition(transformsManager_.GetWorldPosition(entityIndex));
	const ecs::EntityIndex particleIndex = gamePrefabsManager_.Instantiate("ParticleProj")[0];

	physics::Rigidbody rigidbody = rigidbodyManager_.GetComponent(entityIndex);
	graphics::ParticleSystem particleSystem = particleSystemsManager_.GetComponent(particleIndex);
	particleSystem.emitter.line.axis = rigidbody.linearVelocity.Normalize() * -1.0f;
	particleSystemsManager_.SetComponent(particleIndex, particleSystem);
	transformsManager_.SetComponent(particleIndex, transform);

	rigidbody.linearVelocity = math::Vec3();
	rigidbodyManager_.SetComponent(entityIndex, rigidbody);
	
	ecsManager_.DestroyEntity(particleIndex, kParticleTime_);
	ecsManager_.DestroyEntity(entityIndex);
}


void ProjectileSystem::OnEntitySetActive(const ecs::EntityIndex entityIndex) {
	//Check if the activated entity is a missile
	if (!ecsManager_.HasComponent(entityIndex, ecs::ComponentType::PROJECTILE)) { return; }

    if(entityIndexes_.exist(entityIndex)) { return; }

	//Add the entity to the list of entities and sort it
	entityIndexes_.insert(entityIndex);
}
void ProjectileSystem::OnEntitySetInactive(const ecs::EntityIndex entityIndex) {
	//Check if the entity is in the list of entities
	if (!entityIndexes_.exist(entityIndex)) { return; }

	const auto it = entityIndexes_.find(entityIndex);

	//remove the deactivated entity for the list of entities
	entityIndexes_.erase(it);
}
} // namespace poke::game
