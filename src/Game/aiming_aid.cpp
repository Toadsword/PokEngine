#include <Game/aiming_aid.h>
#include <CoreEngine/engine.h>
#include <Game/game.h>
#include <Utility/profiler.h>

namespace poke {
namespace game {
AimingAidSystem::AimingAidSystem(Engine& engine, Game& game) :
	GameSystem(engine, game),
	transformsManager_(ecsManager_.GetComponentsManager<ecs::TransformsManager>()),
	playerManager_(ecsManager_.GetComponentsManager<PlayerManager>()),
	modelsManager_(ecsManager_.GetComponentsManager<ecs::ModelsManager>()),
	camera_(*game.GetCameraPtr()),
	gamePrefabsManager_(reinterpret_cast<GamePrefabsManager&>(PrefabsManagerLocator::Get())),
	gizmoCommandBuffer_(GraphicsEngineLocator::Get().GetGizmoCommandBuffer()) {
	aimDirections_.resize(maxPlayerNb);
	playersDatas_.resize(maxPlayerNb); 

	game.RegisterObserverUpdate([this] { this->OnUpdate(); }	);
	
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

	poke::TextureManagerLocator::Get().AddTexture2D("reticule_lock.png");
	poke::TextureManagerLocator::Get().AddTexture2D("reticule_neutral.png");

	std::string reticuleNeutralMaterialName = "reticule_neutral";
	reticuleNeutralMaterial_ = poke::math::HashString(reticuleNeutralMaterialName);
	poke::MaterialsManagerLocator::Get().AddMaterial(reticuleNeutralMaterialName);

	std::string reticuleLockMaterialName = "reticule_lock";
	reticuleLockMaterial_ = poke::math::HashString(reticuleLockMaterialName);
	poke::MaterialsManagerLocator::Get().AddMaterial(reticuleLockMaterialName);


	std::string meshName = "plane.obj";
	auto meshHash = poke::math::HashString(meshName);
	poke::MeshManagerLocator::Get().AddMesh(meshName);
	targetSightNeutralModel_ = poke::graphics::Model{ reticuleNeutralMaterial_, meshHash };
	targetSightLockModel_ = targetSightNeutralModel_;
	targetSightLockModel_.materialID = reticuleLockMaterial_;

	gamePrefabsManager_.AddPrefab("TargetSight");

	for (size_t i = 0; i < maxPlayerNb; i++) {
		currentTargetSightStates_[i] = TargetSightState::NEUTRAL;
		newTargetSightStates_[i] = TargetSightState::NEUTRAL;
	}

	for (size_t i = 0; i < maxPlayerNb; i++) {
		targetSightModels_[i].meshID = poke::MeshManagerLocator::Get().GetQuadID();
	}
}

void AimingAidSystem::OnUpdate() {
	pok_BeginProfiling(AimingAid_System, 0);
    //Check if there is player register
    if(players_.size() == 0) {
		pok_EndProfiling(AimingAid_System);
        return;
    }

	//Prebatch players data
	for (size_t playerIndex = 0; playerIndex < players_.size(); playerIndex++) {
		playersDatas_[playerIndex] =
			playerManager_.GetComponent(players_[playerIndex]);
	}

    //Get players positions
	for (size_t playerIndex = 0; playerIndex < players_.size(); playerIndex++) {
		playerPositions_[playerIndex] = 
			transformsManager_.GetWorldPosition(players_[playerIndex]);
	}

	//Get target sights positions
	for (size_t playerIndex = 0; playerIndex < players_.size(); playerIndex++) {
		targetSightPositions_[playerIndex] =
			transformsManager_.GetWorldPosition(playersDatas_[playerIndex].targetSightPositionThree);
	}
	
    //Get player aim direction
	for (ecs::EntityIndex playerIndex = 0; playerIndex < players_.size(); playerIndex++) {
		aimDirections_[playerIndex] =
			(targetSightPositions_[playerIndex] - playerPositions_[playerIndex]).Normalize();
	}

	SelectVisibleEntities();

	for (size_t playerIndex = 0; playerIndex < players_.size(); playerIndex++) {
		size_t enemyIndex = kNoEntitySelected_;
		if (playersDatas_[playerIndex].currentTarget != ecs::kNoEntity)
			enemyIndex = playersDatas_[playerIndex].currentTarget;

		// Check if the current player already has a target locked
		if (enemyIndex == kNoEntitySelected_) {
			beaconConfirmedTargetNb_ = 0;
			for (size_t index = 0; index < beaconTargetNb_; index++) {
				const math::Vec3 targetDirection =
					(targetPositions_[index] - playerPositions_[playerIndex]).Normalize();

				float angle = atan2(
					math::Vec3::Cross(aimDirections_[playerIndex], targetDirection).GetMagnitude(),
					aimDirections_[playerIndex] * targetDirection);

				angle = angle * math::kRad2Deg;

				//inversion of the angle
				if (angle > kDefaultAngle_) {
					angle = (angle - kDefaultAngle_ * 2) * -1.0f;
				}

				// return true if the angle is less wider than the max angle
				const bool isLockable = angle < kMaxAimAngle_;

				if (isLockable) {
					selectedTargets_[beaconConfirmedTargetNb_] = index;
					selectedTargetAngles_[beaconConfirmedTargetNb_] = angle;
					beaconConfirmedTargetNb_++;
				}
			}

			float previousDistanceToTarget = kMaxTargetDistance_;
			
			for (size_t index = 0; index < beaconConfirmedTargetNb_; index++) {
				const float distanceToTarget = math::Vec3::GetDistanceManhattan(targetPositions_[selectedTargets_[index]], playerPositions_[playerIndex]);
				if (distanceToTarget < previousDistanceToTarget) {
					targetSightPositions_[playerIndex] = targetPositions_[selectedTargets_[index]];
					enemyIndex = index;
					previousDistanceToTarget = distanceToTarget;
				}
			}

			if (enemyIndex != kNoEntitySelected_) {
				playersDatas_[playerIndex].currentTarget = targets_[selectedTargets_[enemyIndex]];
				playersDatas_[playerIndex].fireDirection =
					(targetPositions_[selectedTargets_[enemyIndex]] - playerPositions_[playerIndex]).Normalize();
				newTargetSightStates_[playerIndex] = TargetSightState::LOCK;
			}
			else {
				playersDatas_[playerIndex].currentTarget = ecs::kNoEntity;
				playersDatas_[playerIndex].fireDirection = aimDirections_[playerIndex];
				newTargetSightStates_[playerIndex] = TargetSightState::NEUTRAL;
			}
		} else {
			const math::Vec3 targetPosition = transformsManager_.GetWorldPosition(enemyIndex);
			const math::Vec3 targetDirection = (targetPosition - playerPositions_[playerIndex]).Normalize();

			float angle = atan2(
				math::Vec3::Cross(aimDirections_[playerIndex], targetDirection).GetMagnitude(),
				aimDirections_[playerIndex] * targetDirection);

			angle = angle * math::kRad2Deg;

			//inversion of the angle
			if (angle > kDefaultAngle_) {
				angle = (angle - kDefaultAngle_ * 2) * -1.0f;
			}

			// return true if the angle is less wider than the max angle
			const bool isLockable = angle < kKeepTargetUntilAngle_;

			if (!isLockable) {
				enemyIndex = kNoEntitySelected_;
			} else {
				targetSightPositions_[playerIndex] = targetPosition;
			}

			if (enemyIndex != kNoEntitySelected_) {
				playersDatas_[playerIndex].fireDirection = (targetPosition - playerPositions_[playerIndex]).Normalize();
				newTargetSightStates_[playerIndex] = TargetSightState::LOCK;
			}
			else {
				playersDatas_[playerIndex].currentTarget = ecs::kNoEntity;
				playersDatas_[playerIndex].fireDirection = aimDirections_[playerIndex];
				newTargetSightStates_[playerIndex] = TargetSightState::NEUTRAL;
			}
		}
    }

    if(newTargetSightStates_ != currentTargetSightStates_) {
		targetSightUpdated_ = false;
    }

	if (!targetSightUpdated_) {
		for (size_t index = 0; index < players_.size(); index++) {
			switch (newTargetSightStates_[index]) {
			case TargetSightState::NEUTRAL:
				targetSightModels_[index].materialID = reticuleNeutralMaterial_;
				break;
			case TargetSightState::LOCK:
				targetSightModels_[index].materialID = reticuleLockMaterial_;
				break;
			default:
				break;
			}
		}

		//Update target sight models
		for (size_t index = 0; index < players_.size(); index++) {
			modelsManager_.SetComponent(playersDatas_[index].targetSightModelThree, targetSightModels_[index]);
		}

		//Tell that the model component of each target sight have new data
		for (size_t index = 0; index < players_.size(); index++) {
			ecsManager_.UpdateComponent(playersDatas_[index].targetSightModelThree, ecs::ComponentType::MODEL);
		}

		targetSightUpdated_ = true;
	}

	for (size_t index = 0; index < players_.size(); index++) {
		math::Vec3 sightOnePosition = transformsManager_.GetWorldPosition(playersDatas_[index].targetSightPositionOne);
		math::Vec3 sightTwoPosition = transformsManager_.GetWorldPosition(playersDatas_[index].targetSightPositionTwo);
		math::Vec3 sightThreePosition = transformsManager_.GetWorldPosition(playersDatas_[index].targetSightPositionThree);

		math::Transform sightOneTransform = transformsManager_.GetWorldTransform(playersDatas_[index].targetSightModelOne);
		math::Transform sightTwoTransform = transformsManager_.GetWorldTransform(playersDatas_[index].targetSightModelTwo);
		math::Transform sightThreeTransform = transformsManager_.GetWorldTransform(playersDatas_[index].targetSightModelThree);

		switch (newTargetSightStates_[index]) {
		case TargetSightState::NEUTRAL:
			sightOneTransform.SetLocalPosition(sightOnePosition);
			sightTwoTransform.SetLocalPosition(sightTwoPosition);
			sightThreeTransform.SetLocalPosition(sightThreePosition);
			break;
		case TargetSightState::LOCK:
			sightOneTransform.SetLocalPosition(
				playerPositions_[index] + playersDatas_[index].fireDirection * kDistAimingOne_);
			sightTwoTransform.SetLocalPosition(
				playerPositions_[index] + playersDatas_[index].fireDirection * kDistAimingTwo_);
			sightThreeTransform.SetLocalPosition(
				playerPositions_[index] + playersDatas_[index].fireDirection * kDistAimingThree_);
			break;
		default: 
			break;
		}
		transformsManager_.SetComponentFromWorldTransform(playersDatas_[index].targetSightModelOne, sightOneTransform);
		transformsManager_.SetComponentFromWorldTransform(playersDatas_[index].targetSightModelTwo, sightTwoTransform);
		transformsManager_.SetComponentFromWorldTransform(playersDatas_[index].targetSightModelThree, sightThreeTransform);
	}

    //rotate all aiming sight to look at the camera
    for(size_t playerIndex = 0; playerIndex < players_.size(); playerIndex++) {
		LookAtCamera(playersDatas_[playerIndex].targetSightModelOne);
		LookAtCamera(playersDatas_[playerIndex].targetSightModelTwo);
		LookAtCamera(playersDatas_[playerIndex].targetSightModelThree);
    }
    
	currentTargetSightStates_ = newTargetSightStates_;
	
	//Update Player data
	for (size_t playerIndex = 0; playerIndex < players_.size(); playerIndex++) {
		playerManager_.SetComponent(players_[playerIndex], playersDatas_[playerIndex]);
	}
	pok_EndProfiling(AimingAid_System);
}

void AimingAidSystem::SelectVisibleEntities() {
	beaconTargetNb_ = 0;
	const std::vector<ecs::EntityIndex>& visibleEntities = ecsManager_.GetDrawnEntities();
	
	//Select only visible entities that are destructible elements
	for (const ecs::EntityIndex entityIndex : visibleEntities) {
		if (beaconTargetNb_ >= maxTargetNb) { break; }
		if (ecsManager_.HasComponent(entityIndex, ecs::ComponentType::DESTRUCTIBLE_ELEMENT)) {
			// Avoid locking indestructible element
			if (ecsManager_.GetComponentsManager<DestructibleElementManager>().GetComponent(entityIndex).isIndestructible)
				continue;

			targetPositions_[beaconTargetNb_] = transformsManager_.GetWorldPosition(entityIndex);

			if (math::Vec3::GetDistanceManhattan(
				targetPositions_[beaconTargetNb_], playerPositions_[0]) <
				kMaxTargetDistance_)
			{
				targets_[beaconTargetNb_] = entityIndex;
				beaconTargetNb_++;
			}
		}
	}
}

void AimingAidSystem::LookAtCamera(const ecs::EntityIndex entityIndex) {
	math::Transform transform = transformsManager_.GetWorldTransform(entityIndex);

	transform.SetLocalRotation(
		math::Quaternion::LookRotation(
			camera_.GetFront(), 
			camera_.GetUp()).GetEulerAngles());
	transformsManager_.SetComponentFromWorldTransform(entityIndex, transform);
}

void AimingAidSystem::OnEntitySetActive(const ecs::EntityIndex entityIndex) {
	//Check if the activated entity is a missile
	if (!ecsManager_.HasComponent(entityIndex, ecs::ComponentType::PLAYER)) { return; }

	if (players_.exist(entityIndex)) { return; }

	//Add the entity to the list of entities and sort it
	players_.insert(entityIndex);
}

void AimingAidSystem::OnEntitySetInactive(const ecs::EntityIndex entityIndex) {
	//Check if the entity is in the list of entities
	if (!players_.exist(entityIndex)) { return; }

	const auto it = players_.find(entityIndex);

	//remove the deactivated entity for the list of entities
	players_.erase(it);
}

void AimingAidSystem::OnEntityAddComponent(const ecs::EntityIndex entityIndex, const ecs::ComponentMask componentMask) {
	//Check if the entity have the component required
	if ((componentMask & ecs::ComponentType::PLAYER) != ecs::ComponentType::PLAYER) { return; }

	//Check if the entity is active
	if (!ecsManager_.HasComponent(entityIndex, ecs::EntityFlag::IS_ACTIVE)) { return; }

	if (players_.exist(entityIndex)) { return; }

	//Add the entity to the list of entities and sort it
	players_.insert(entityIndex);
}

void AimingAidSystem::OnEntityRemoveComponent(const ecs::EntityIndex entityIndex, const ecs::ComponentMask componentMask) {
	if ((componentMask & ecs::ComponentType::PLAYER) != ecs::ComponentType::PLAYER) {
		return;
	}

	//Check if the entity is in the list of entities
	if (!players_.exist(entityIndex)) { return; }

	const auto it = players_.find(entityIndex);

	//remove if one of the removed components
	players_.erase(it);

}
}//namespace game
}//namespace poke