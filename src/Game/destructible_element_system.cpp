#include <Game/destructible_element_system.h>
#include <CoreEngine/engine.h>
#include <Game/game.h>
#include <Game/ComponentManagers/destructible_element_manager.h>
#include <Game/ComponentManagers/missiles_manager.h>
#include <Game/Components/destructible_element.h>
#include <Game/Components/player.h>
#include <Utility/profiler.h>
#include <Utility/log.h>

namespace poke::game {
DestructibleElementSystem::DestructibleElementSystem(Engine& engineExport, Game & game)
: GameSystem(engineExport, game),
	transformsManager_(EcsManagerLocator::Get().GetComponentsManager<ecs::TransformsManager>()),
	playerManager_(EcsManagerLocator::Get().GetComponentsManager<PlayerManager>()),
	destructibleElementManager_(EcsManagerLocator::Get().GetComponentsManager<DestructibleElementManager>()),
	projectileManager_(EcsManagerLocator::Get().GetComponentsManager<ProjectileManager>()),
    particleSystemsManager_(EcsManagerLocator::Get().GetComponentsManager<ecs::ParticleSystemsManager>()),
	rigidbodyManager_(ecsManager_.GetComponentsManager<ecs::RigidbodyManager>()),
	missilesManager_(ecsManager_.GetComponentsManager<MissilesManager>()),
	gamePrefabsManager_(static_cast<GamePrefabsManager&>(PrefabsManagerLocator::Get())),
	audioSourcesManager_(ecsManager_.GetComponentsManager<ecs::AudioSourcesManager>())
{
	game.RegisterObserverUpdate([this] { this->OnUpdate(); });
	
    ecsManager_.RegisterObserverAddComponent(
        [this](
        const ecs::EntityIndex entityIndex,
        const ecs::ComponentMask component) {
            OnEntityAddComponent(entityIndex, component);
        });

	ecsManager_.RegisterObserverRemoveComponent(
		[this](ecs::EntityIndex entityIndex, ecs::ComponentMask component)
	    {OnEntityRemoveComponent(entityIndex, component); }
	);


	ObserveLoadScene();

	ObserveEntitySetActive();
	ObserveEntitySetInactive();

	game.RegisterObserverUpdate([this] { this->OnUpdate(); });
}

void DestructibleElementSystem::OnLoadScene() {
	gamePrefabsManager_.AddPrefab("ParticleAsteroid");
	gamePrefabsManager_.AddPrefab("ParticleTurret");

	//Creation of an archetype
	// Prepare the components values
	GameArchetype debrisArchetype;
	debrisArchetype.AddComponent(
		ecs::ComponentType::TRANSFORM |
		ecs::ComponentType::RIGIDBODY |
		ecs::ComponentType::MODEL);

	physics::Rigidbody rigid;
	rigid.type = physics::RigidbodyType::DYNAMIC;
	debrisArchetype.rigidbody = rigid;

	graphics::Model projectileModel;
	std::string materialName = "asteroid_mat";
	auto materialHash = poke::math::HashString(materialName);
	poke::MaterialsManagerLocator::Get().AddMaterial(materialName);
	std::string meshName = "Asteroides/SM_astero6_LOD00.obj";
	auto meshHash = poke::math::HashString("Asteroides/SM_astero6_LOD00.obj");
	poke::MeshManagerLocator::Get().AddMesh(meshName);
	projectileModel = graphics::Model{ materialHash, meshHash };
	debrisArchetype.model = projectileModel;
	debrisArchetype.transform.SetLocalPosition({ 1000000 });
	ArchetypesManagerLocator::Get().AddArchetype(debrisArchetype, "debris", kMaxArchetypeDebrisNb_);

	debrisArchetypeId_ = ArchetypesManagerLocator::Get().GetArchetypeID("debris");
	//End Creation
}

void DestructibleElementSystem::OnUpdate() {
	pok_BeginProfiling(Destructible_Element_System, 0);
	std::vector<ecs::EntityIndex> entitiesToDestroy;
	for (auto it = entityIndexes_.begin(); it != entityIndexes_.end(); ++it) {
		const DestructibleElement destructibleElement = destructibleElementManager_.GetComponent(*it);
		if (destructibleElement.life <= 0) {
			Destruction(*it, destructibleElement);
			entitiesToDestroy.push_back(*it);
		}
	}

	for (auto toDestroy : entitiesToDestroy) {
		ecsManager_.DestroyEntity(toDestroy, destructibleElementManager_.GetComponent(toDestroy).destructionTime);
	}
	pok_EndProfiling(Destructible_Element_System);
}

void DestructibleElementSystem::OnTriggerEnter(
const ecs::EntityIndex entityIndex,
const physics::Collision collision)
{
    //TODO (@Robin) Check perf with OntriggerEnter and the OnUpdate
	// Manage destructible element vs destructible element
	if (ecsManager_.HasComponent(
		collision.otherEntity,
		ecs::ComponentType::DESTRUCTIBLE_ELEMENT)) {
		auto destructibleElement = destructibleElementManager_.GetComponent(entityIndex);
		if (destructibleElement.isIndestructible) { return; }
		destructibleElement.life = 0.0f;
		Destruction(entityIndex, destructibleElement);
		destructibleElementManager_.SetComponent(entityIndex, destructibleElement);
	}
}

void DestructibleElementSystem::Destruction(ecs::EntityIndex entityIndex, DestructibleElement destructibleElement) {
	math::Vec3 position = transformsManager_.GetWorldPosition(entityIndex);
	math::Transform transform;
	transform.SetLocalPosition(position);
	switch (destructibleElement.type) {
    case DestructibleElement::Type::ASTEROID:
	{
		if (destructibleElement.destructionTime <= 0.5f) {
			for (size_t i = 0; i < kDebrisNbPerAsteroids_; i++) {
				const ecs::EntityIndex debrisIndex = ecsManager_.AddEntity(debrisArchetypeId_);
				math::Transform transformDebris = transformsManager_.GetWorldTransform(entityIndex);
				transformDebris.SetLocalScale(math::Vec3::Multiply(transformDebris.GetLocalScale(), math::Vec3(kDebrisSize_)));
				transformsManager_.SetComponent(debrisIndex, transformDebris);
				physics::Rigidbody rigid = rigidbodyManager_.GetComponent(debrisIndex);
				rigid.linearVelocity = {
					math::Random::Get().RandomRange(-1.0f, 1.0f),
					math::Random::Get().RandomRange(-1.0f, 1.0f),
					math::Random::Get().RandomRange(-1.0f, 1.0f)
				};
				rigid.linearVelocity *= kDebrisSpeed_;
				rigidbodyManager_.SetComponent(debrisIndex, rigid);
				ecsManager_.DestroyEntity(debrisIndex, kDebrisLifeTime_);
			}
		}
		const ecs::EntityIndex particleIndex = gamePrefabsManager_.Instantiate("ParticleAsteroid")[0];

		graphics::ParticleSystem particleSystem = particleSystemsManager_.GetComponent(particleIndex);
		particleSystem.minSize = transformsManager_.GetWorldScale(entityIndex).GetMagnitude() * particleSystem.minSize * kParticleAsteroidMultiplier_;
		particleSystem.maxSize = transformsManager_.GetWorldScale(entityIndex).GetMagnitude() * particleSystem.maxSize * kParticleAsteroidMultiplier_;
		particleSystemsManager_.SetComponent(particleIndex, particleSystem);
		ecsManager_.AddComponent(particleIndex, ecs::ComponentType::TRANSFORM);
		transformsManager_.SetComponent(particleIndex, transform);
		ecsManager_.DestroyEntity(particleIndex, kParticleTimeAsteroid_);
		auto audioSource = audioSourcesManager_.GetComponent(entityIndex);
		audioSource.PlayInstance(position);
	}
	    break;
    case DestructibleElement::Type::SHIP:
	{
		const ecs::EntityIndex particleIndex = gamePrefabsManager_.Instantiate("ParticleTurret")[0];
		ecsManager_.AddComponent(particleIndex, ecs::ComponentType::TRANSFORM);
		transformsManager_.SetComponent(particleIndex, transform);
		ecsManager_.DestroyEntity(particleIndex, kParticleTimeShip_);

		auto audioSource = audioSourcesManager_.GetComponent(entityIndex);
		audioSource.PlayInstance(position);
	}
		break;
    case DestructibleElement::Type::TURRET:
	{
		const ecs::EntityIndex particleIndex = gamePrefabsManager_.Instantiate("ParticleTurret")[0];
		ecsManager_.AddComponent(particleIndex, ecs::ComponentType::TRANSFORM);
		transformsManager_.SetComponent(particleIndex, transform);
		ecsManager_.DestroyEntity(particleIndex, kParticleTimeTurret_);

		auto audioSource = audioSourcesManager_.GetComponent(entityIndex);
		audioSource.Play(position);
	}
		break;
	default:
		break;
    }

	// Check if a player has destroyed the element
	if(destructibleElement.destroyedBy != ecs::kNoEntity) {
		// Add score points and destruction points to the player that has destroyed the element
		Player player = playerManager_.GetComponent(destructibleElement.destroyedBy);
		player.score += destructibleElement.scorePointGiven;
		player.destructionPoint += destructibleElement.destructionPointGiven;
		playerManager_.SetComponent(destructibleElement.destroyedBy, player);

		if (player.destructionPoint <= player.specialCapacityAvailableAt) {
			auto specialAttackComponent = particleSystemsManager_.GetComponent(player.specialAttackIndex);
			float pourcent = (static_cast<float>(player.destructionPoint) / static_cast<float>(player.specialCapacityAvailableAt)) * 100;
			specialAttackComponent.rateOverTime = pourcent;
			if (player.destructionPoint > player.specialCapacityAvailableAt) { specialAttackComponent.rateOverTime = 50; }
			particleSystemsManager_.SetComponent(player.specialAttackIndex, specialAttackComponent);
		}

	}
}

void DestructibleElementSystem::OnEntityAddComponent(
const ecs::EntityIndex entityIndex,
const ecs::ComponentMask component)
{
	//Check if the entity have the component required
	if ((component& ecs::ComponentType::DESTRUCTIBLE_ELEMENT) != ecs::ComponentType::DESTRUCTIBLE_ELEMENT) { return; }

	//check if the entity has already been added the the system by the past
	const auto itRegisteredEntities = std::find(registeredEntities_.begin(), registeredEntities_.end(), entityIndex);
	if (itRegisteredEntities == registeredEntities_.end()) {
		//Add the entity to the list of registered entities and sort it
		registeredEntities_.insert(entityIndex);

		ecsManager_.RegisterObserverTriggerEnter(
			entityIndex,
			[this](ecs::EntityIndex entityIndex, physics::Collision collision)
		{this->OnTriggerEnter(entityIndex, collision); });
	}

	//Check if the entity is active
	if (!ecsManager_.HasComponent(entityIndex, ecs::EntityFlag::IS_ACTIVE)) { return; }

	if (entityIndexes_.exist(entityIndex));

	//Add the entity to the list of entities and sort it
	entityIndexes_.insert(entityIndex);
}

void DestructibleElementSystem::OnEntityRemoveComponent(const ecs::EntityIndex entityIndex, const ecs::ComponentMask component) {
	if ((component & ecs::ComponentType::DESTRUCTIBLE_ELEMENT) != ecs::ComponentType::DESTRUCTIBLE_ELEMENT) {
		return;
	}
	//Check if the entity is in the list of entities
	if (!entityIndexes_.exist(entityIndex)) { return; }

	const auto it = entityIndexes_.find(entityIndex);

	//remove if one of the removed components
	entityIndexes_.erase(it);
}

void DestructibleElementSystem::OnEntitySetActive(const ecs::EntityIndex entityIndex) {
	//Check if the activated entity is a missile
	if (!ecsManager_.HasComponent(entityIndex, ecs::ComponentType::DESTRUCTIBLE_ELEMENT)) { return; }

    if(entityIndexes_.exist(entityIndex)) { return; }

	//Add the entity to the list of entities and sort it
	entityIndexes_.insert(entityIndex);
}

void DestructibleElementSystem::OnEntitySetInactive(const ecs::EntityIndex entityIndex) {
	//Check if the entity is in the list of entities
	if (!entityIndexes_.exist(entityIndex)) { return; }

	const auto it = entityIndexes_.find(entityIndex);

	//remove the deactivated entity for the list of entities
	entityIndexes_.erase(it);
}
}//namespace poke::game
