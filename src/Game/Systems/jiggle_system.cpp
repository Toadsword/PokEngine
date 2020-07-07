#include <Game/Systems/jiggle_system.h>
#include <CoreEngine/engine.h>
#include <Utility/time_custom.h>
#include <Game/game.h>


namespace poke {
namespace game {
JiggleSystem::JiggleSystem(Engine& engine, Game& game) :
	GameSystem(engine, game),
	jiggleManager_(ecsManager_.GetComponentsManager<JiggleManager>()),
	transformsManager_(ecsManager_.GetComponentsManager<ecs::TransformsManager>()),
	rigidbodyManager_(ecsManager_.GetComponentsManager<ecs::RigidbodyManager>())
{
	game_.RegisterObserverUpdate([this]() {OnUpdate(); });

	ObserveEntitySetActive();
	ObserveEntitySetInactive();

	ecsManager_.RegisterObserverAddComponent(
		[this](ecs::EntityIndex entityIndex, ecs::ComponentMask componentMask)
	    {this->OnEntityAddComponent(entityIndex, componentMask); }
	);
	ecsManager_.RegisterObserverRemoveComponent(
		[this](ecs::EntityIndex entityIndex, ecs::ComponentMask componentMask)
	    {this->OnEntityRemoveComponent(entityIndex, componentMask); }
	);
	targetDirs_.resize(kJiggleNb_);
	lastDirs_.resize(kJiggleNb_);
	startPositions_.resize(kJiggleNb_);
	timers_.resize(kJiggleNb_);
}

void JiggleSystem::OnUpdate() {

	const float dt = static_cast<float>(Time::Get().deltaTime.count() / 1000.0);
	const size_t size = entityIndexes_.size();
	for (size_t index = 0; index < size; index++) {
		const Jiggle jiggle = jiggleManager_.GetComponent(entityIndexes_[index]);

		//Change velocity direction after maxTime
		if (timers_[index] > jiggle.maxTime)
		{
			const physics::Rigidbody rigidBody = rigidbodyManager_.GetComponent(entityIndexes_[index]);
			math::Vec3 randomVec = math::Vec3(
				math::Random::Get().RandomRange(-1, 1), 
				math::Random::Get().RandomRange(-1, 1),
				math::Random::Get().RandomRange(-1, 1)
			);
			//Direction set to a point around startPosition
			math::Vec3 dir = randomVec + startPositions_[index] - transformsManager_.GetComponent(entityIndexes_[index]).GetLocalPosition();
			targetDirs_[index] = dir/dir.GetMagnitude() * jiggle.speed;
			lastDirs_[index] = rigidBody.linearVelocity;
			timers_[index] = 0;
		} else {
			timers_[index] += dt;
			physics::Rigidbody rigidBody = rigidbodyManager_.GetComponent(entityIndexes_[index]);
			//Lerp velocity from lastDirs to targetDirs
			rigidBody.linearVelocity = lastDirs_[index] + ((targetDirs_[index] - lastDirs_[index]) * (timers_[index] / jiggle.maxTime)); //Lerp velocity
			rigidbodyManager_.SetComponent(entityIndexes_[index], rigidBody);
			
			math::Transform transform = transformsManager_.GetComponent(entityIndexes_[index]);
			//Rotate around an axis
			transform.SetLocalRotation(transform.GetLocalRotation() + kRotation_ * jiggle.rotationSpeed);
			transformsManager_.SetComponent(entityIndexes_[index], transform);
		}
    }
}

void JiggleSystem::OnEntitySetActive(const ecs::EntityIndex entityIndex) {
	//Check if the activated entity is a missile
	if (!ecsManager_.HasComponent(entityIndex, ecs::ComponentType::JIGGLE)) { return; }

    if(entityIndexes_.exist(entityIndex)) { return; }

	//Add the entity to the list of entities and sort it
	entityIndexes_.insert(entityIndex);
}

void JiggleSystem::OnEntitySetInactive(const ecs::EntityIndex entityIndex) {
	//Check if the entity is in the list of entities
	if (!entityIndexes_.exist(entityIndex)) { return; }

	const auto it = entityIndexes_.find(entityIndex);

	//remove the deactivated entity for the list of entities
	entityIndexes_.erase(it);
}

void JiggleSystem::OnEntityAddComponent(const ecs::EntityIndex entityIndex, const ecs::ComponentMask componentMask) {
	//Check if the entity have the component required
	if ((componentMask & ecs::ComponentType::JIGGLE) != ecs::ComponentType::JIGGLE) { return; }

	//check if the entity has already been added the the system by the past
	if (!registeredEntities_.exist(entityIndex)) {
		//Add the entity to the list of registered entities and sort it
		registeredEntities_.insert(entityIndex);
	}
	
	//Check if the entity is active
	if (!ecsManager_.HasComponent(entityIndex, ecs::EntityFlag::IS_ACTIVE)) { return; }

    if(entityIndexes_.exist(entityIndex)) { return; }

	//Add the entity to the list of entities and sort it
	entityIndexes_.insert(entityIndex);

	//Register startPosition
	math::Transform transform = transformsManager_.GetComponent(entityIndex);
	startPositions_[entityIndexes_.size() - 1] = transform.GetLocalPosition();

	//Set random start position
	transform.SetLocalRotation(math::Vec3(
		math::Random::Get().RandomRange(-180.0f, 180.0f) * math::kDeg2Rad,
		math::Random::Get().RandomRange(-180.0f, 180.0f) * math::kDeg2Rad,
		math::Random::Get().RandomRange(-180.0f, 180.0f) * math::kDeg2Rad));
	transformsManager_.SetComponent(entityIndex, transform);
}

void JiggleSystem::OnEntityRemoveComponent(const ecs::EntityIndex entityIndex, const ecs::ComponentMask componentMask) {
	if ((componentMask & ecs::ComponentType::JIGGLE) != ecs::ComponentType::JIGGLE) {
		return;
	}
	//Check if the entity is in the list of entities
	if (!entityIndexes_.exist(entityIndex)) { return; }

	const auto it = entityIndexes_.find(entityIndex);

	//remove if one of the removed components
	entityIndexes_.erase(it);
}
}//namespace game
}//namespace poke