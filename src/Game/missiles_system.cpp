#include <Game/missiles_system.h>

#include <Ecs/core_ecs_manager.h>
#include <Utility/time_custom.h>
#include <CoreEngine/engine.h>
#include <Utility/profiler.h>
#include <Game/game.h>

namespace poke {
namespace game {
MissilesSystem::MissilesSystem(Engine& engine, Game& game) :
	GameSystem(engine, game),
	transformsManager_(ecsManager_.GetComponentsManager<ecs::TransformsManager>()),
	rigidbodyManager_(ecsManager_.GetComponentsManager<ecs::RigidbodyManager>()),
	missilesManager_(ecsManager_.GetComponentsManager<MissilesManager>()),
	destructibleElementManager_(ecsManager_.GetComponentsManager<DestructibleElementManager>()),
	particleSystemsManager_(ecsManager_.GetComponentsManager<ecs::ParticleSystemsManager>()),
	gamePrefabsManager_(static_cast<GamePrefabsManager&>(PrefabsManagerLocator::Get()))
{
	game.RegisterObserverUpdate([this] { this->OnUpdate(); });
	
	ObserveEntitySetActive();
	ObserveEntitySetInactive();
	ObserveLoadScene();

	ecsManager_.RegisterObserverAddComponent(
		[this](ecs::EntityIndex entityIndex, ecs::ComponentMask componentMask)
	    {this->OnEntityAddComponent(entityIndex, componentMask); }
	);
	ecsManager_.RegisterObserverRemoveComponent(
		[this](ecs::EntityIndex entityIndex, ecs::ComponentMask componentMask)
	    {this->OnEntityRemoveComponent(entityIndex, componentMask); }
	);

	missiles_.resize(kMaxMissileNb_);
	rigidbodies_.resize(kMaxMissileNb_);
	transforms_.resize(kMaxMissileNb_);
	missileTargetDirections_.resize(kMaxMissileNb_);
}

void MissilesSystem::OnLoadScene() {
	gamePrefabsManager_.AddPrefab("ParticleMissile");
}

void MissilesSystem::OnUpdate() {
	pok_BeginProfiling(Missile_System, 0);
	flagEndData_ = entityIndexes_.size();

    //Get missiles
	for (size_t index = 0; index < flagEndData_; index++) {
		missiles_[index] = missilesManager_.GetComponent(entityIndexes_[index]);
	}

	for (size_t index = 0; index < flagEndData_; index++) {
		rigidbodies_[index] = rigidbodyManager_.GetComponent(entityIndexes_[index]);
	}

	for (size_t index = 0; index < flagEndData_; index++) {
		transforms_[index] = transformsManager_.GetComponent(entityIndexes_[index]);
	}

	for (size_t index = 0; index < flagEndData_; index++) {
		missileTargetDirections_[index] =
			(transformsManager_.GetWorldPosition(missiles_[index].target)
				- transforms_[index].GetLocalPosition()).Normalize();
	}

	std::vector<ecs::EntityIndex> deactivatedEntities;
	const float deltaTime = static_cast<float>(Time::Get().deltaTime.count() / 1000.0);
    for (size_t index = 0; index < flagEndData_; index++) {
		const ecs::EntityIndex entityIndex = entityIndexes_[index];

		if(!missiles_[index].hasBeenShot) {
			missiles_[index].shootDelay -= deltaTime;

			if (missiles_[index].shootDelay <= 0.0f) {
				const math::Vec3 worldPos = transformsManager_.GetWorldPosition(entityIndex);
				transformsManager_.SetParent(entityIndex, -1);
				transforms_[index].SetLocalPosition(worldPos);
				transforms_[index].SetLocalScale({0.02f, 0.02f, 0.02f});
				missiles_[index].hasBeenShot = true;
			}
			else
				continue;
		}
    	
		if (IsInFieldOfAim(missiles_[index].direction, missileTargetDirections_[index], kMaxAngle_)) {
			missiles_[index].direction = 
				(missiles_[index].direction +
				(missileTargetDirections_[index] - missiles_[index].direction) * kLerpValue_
					).Normalize();

			transforms_[index].SetLocalRotation(
				//Look at
				math::Quaternion::FromToRotation(
					math::Vec3(0, 0, 1),
					missiles_[index].direction
				).GetEulerAngles()
			);
		}

		rigidbodies_[index].linearVelocity = missiles_[index].direction * missiles_[index].speed;
    	
		missiles_[index].lifeTime -= deltaTime;

        if(missiles_[index].lifeTime <= 0) {
			missiles_[index].lifeTime = 0;
			deactivatedEntities.push_back(entityIndexes_[index]);
        }
    }

	for (size_t index = 0; index < flagEndData_; index++) {
		rigidbodyManager_.SetComponent(entityIndexes_[index], rigidbodies_[index]);
	}

	for (size_t index = 0; index < flagEndData_; index++) {
		missilesManager_.SetComponent(entityIndexes_[index], missiles_[index]);
	}

	for (size_t index = 0; index < flagEndData_; index++) {
		transformsManager_.SetComponent(entityIndexes_[index], transforms_[index]);
	}

	for (ecs::EntityIndex entityIndex : deactivatedEntities) {
		ecsManager_.DestroyEntity(entityIndex);
	}
	pok_EndProfiling(Missile_System);
}

void MissilesSystem::OnEntitySetActive(const ecs::EntityIndex entityIndex) {
	//Check if the activated entity is a missile
	if (!ecsManager_.HasComponent(entityIndex, ecs::ComponentType::MISSILE)) { return; }
    if(entityIndexes_.exist(entityIndex)) { return; }
    
	//Add the entity to the list of entities and sort it
	entityIndexes_.insert(entityIndex);

	//Set the front direction of the missile it will be like the transform
	Missile missile = missilesManager_.GetComponent(entityIndex);
	math::Matrix4 missileWorldMatrix = transformsManager_.GetComponent(entityIndex).GetWorldMatrix();
	missile.direction = 
		math::Vec3(
			missileWorldMatrix[0][2],
			missileWorldMatrix[1][2],
			missileWorldMatrix[2][2]
		).Normalize();
	missilesManager_.SetComponent(entityIndex, missile);
}

void MissilesSystem::OnEntitySetInactive(const ecs::EntityIndex entityIndex) {
	//Check if the entity is in the list of entities
    if(entityIndexes_.exist(entityIndex)) {
		const auto it = entityIndexes_.find(entityIndex);
		entityIndexes_.erase(it);
    }
}

void MissilesSystem::OnEntityAddComponent(const ecs::EntityIndex entityIndex, const ecs::ComponentMask componentMask) {
	//Check if the entity have the component required
	if ((componentMask & ecs::ComponentType::MISSILE) != ecs::ComponentType::MISSILE) { return; }

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

	//Set the front direction of the missile it will be like the transform
	Missile missile = missilesManager_.GetComponent(entityIndex);
	math::Matrix4 missileWorldMatrix = transformsManager_.GetComponent(entityIndex).GetWorldMatrix();
	missile.direction = math::Vec3(missileWorldMatrix[0][2], missileWorldMatrix[1][2], missileWorldMatrix[2][2]).Normalize();
	missilesManager_.SetComponent(entityIndex, missile);
}

void MissilesSystem::OnEntityRemoveComponent(
	const ecs::EntityIndex entityIndex,
	const ecs::ComponentMask componentMask)
{
	if ((componentMask & ecs::ComponentType::MISSILE) != ecs::ComponentType::MISSILE) {
		return;
	}
	//Check if the entity is in the list of entities
	if(!entityIndexes_.exist(entityIndex)) return;

	const auto it = entityIndexes_.find(entityIndex);

	//remove if one of the removed components
	entityIndexes_.erase(it);
}

void MissilesSystem::OnTriggerEnter(const ecs::EntityIndex entityIndex, const physics::Collision collision) {
	if (ecsManager_.HasComponent(collision.otherEntity, ecs::ComponentType::DESTRUCTIBLE_ELEMENT)) {
		DestructibleElement destructibleElement = destructibleElementManager_.GetComponent(collision.otherEntity);
		Missile missile = missilesManager_.GetComponent(entityIndex);

		destructibleElement.life -= missilesManager_.GetComponent(entityIndex).damage;
		destructibleElement.destroyedBy = missile.origin;
		destructibleElementManager_.SetComponent(collision.otherEntity, destructibleElement);

		MissileExplosion(entityIndex);
	}
}

void MissilesSystem::MissileExplosion(const ecs::EntityIndex entityIndex) {
	physics::Rigidbody rigidbody = rigidbodyManager_.GetComponent(entityIndex);
	rigidbody.linearVelocity = math::Vec3();
	rigidbodyManager_.SetComponent(entityIndex, rigidbody);

	Missile missile = missilesManager_.GetComponent(entityIndex);
	missile.lifeTime = 0.0f;
	missilesManager_.SetComponent(entityIndex, missile);

    //Explosion particle instantiation
	const ecs::EntityIndex particleIndex = gamePrefabsManager_.Instantiate("ParticleMissile")[0];

	math::Transform transform;
	transform.SetLocalPosition(transformsManager_.GetComponent(entityIndex).GetLocalPosition());
	ecsManager_.AddComponent(particleIndex, ecs::ComponentType::TRANSFORM);
	transformsManager_.SetComponent(particleIndex, transform);

	ecsManager_.DestroyEntity(particleIndex, kParticleTime_);

	ecsManager_.DestroyEntity(entityIndex);
}

bool MissilesSystem::IsInFieldOfAim(
	const math::Vec3 missileDirection,
	const math::Vec3 targetDirection,
	const float maxAngle) const
{
	const math::Vec3 mdNormalized = missileDirection.Normalize(); 
	const math::Vec3 tdNormalized = targetDirection.Normalize();

	float angleBetween = atan2(math::Vec3::Cross(mdNormalized, tdNormalized).GetMagnitude(), mdNormalized * tdNormalized);

	angleBetween = angleBetween * math::kRad2Deg;

	//inversion of the angle
	if (angleBetween > 180.0f) {
		angleBetween = (angleBetween - 360.0f) * -1.0f;  
	}

	// return true if the angle is less wider than the max angle
	return angleBetween < maxAngle;
}
}//namespace game  
}//namespace poke
