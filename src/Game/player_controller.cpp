#include <Game/player_controller.h>
#include <CoreEngine/engine.h>
#include <Utility/time_custom.h>
#include <Utility/profiler.h>
#include <Game/game.h>
#include <Game/Components/special_attack.h>

namespace poke::game {
PlayerController::PlayerController(Engine& engine, Game& game)
	: GameSystem(engine, game),
	inputManager_(engine.GetModuleManager().inputManager),
	playerManager_(ecsManager_.GetComponentsManager<PlayerManager>()),
	weaponManager_(ecsManager_.GetComponentsManager<WeaponManager>()),
	specialAttackManager_(ecsManager_.GetComponentsManager<SpecialAttackManager>()),
	projectileManager_(ecsManager_.GetComponentsManager<ProjectileManager>()),
	missilesManager_(ecsManager_.GetComponentsManager<MissilesManager>()),
	transformsManager_(ecsManager_.GetComponentsManager<ecs::TransformsManager>()),
	rigidbodyManager_(ecsManager_.GetComponentsManager<ecs::RigidbodyManager>()),
	modelsManager_(ecsManager_.GetComponentsManager<ecs::ModelsManager>()),
	destructibleElementManager_(ecsManager_.GetComponentsManager<DestructibleElementManager>()),
	gizmoCommandBuffer_(GraphicsEngineLocator::Get().GetGizmoCommandBuffer()),
	gamePrefabsManager_(static_cast<GamePrefabsManager&>(PrefabsManagerLocator::Get())) {
	game.RegisterObserverUpdate([this] { this->OnUpdate(); });

	ecsManager_.RegisterObserverAddComponent(
		[this](ecs::EntityIndex entityIndex, ecs::ComponentMask component)
	{OnAddComponent(entityIndex, component); }
	);

	ObserveLoadScene();
}

void PlayerController::OnLoadScene()
{
	gamePrefabsManager_.AddPrefab("ParticleFire");
	gamePrefabsManager_.AddPrefab("ParticleExplo");

	if (!archetypeAlreadyLoad_) {
		archetypeTarget_ = reinterpret_cast<const GameArchetype&>(
			PrefabsManagerLocator::Get().GetPrefab(kTargetPrefabName_).GetObject(0));
		archetypeTarget_.transform.SetLocalPosition({ 1000000 });
		ArchetypesManagerLocator::Get().AddArchetype(archetypeTarget_, kTargetPrefabName_, kTargetNb_);
		targetId_ = ArchetypesManagerLocator::Get().GetArchetypeID(kTargetPrefabName_);
		archetypeAlreadyLoad_ = true;
	}
}

void PlayerController::OnUpdate()
{
	pok_BeginProfiling(Player_Controller, 0);
	for (const ecs::EntityIndex entityIndex : entityIndexes_) {
		Player currentPlayer = playerManager_.GetComponent(entityIndex);

		// Check if the current player is dead
		if (currentPlayer.life <= 0) {
			pok_EndProfiling(Player_Controller);
			return;
		}

		if (currentPlayer.state != Player::PlayerState::DASHING) {
			if (inputManager_.IsActionDown(input::Action::DASH) &&
				currentPlayer.lastDashEndedAt > currentPlayer.dashCooldown && 
				(currentPlayer.moveDirection.x != 0.0f || currentPlayer.moveDirection.y != 0.0f)) 
			{
				currentPlayer.state = Player::PlayerState::DASHING;
				currentPlayer.dashStartedAt = Time::Get().GetTime();

			    //Launch dash sound
				audio::AudioSource dashAudioSource = ecsManager_
					.GetComponentsManager<ecs::AudioSourcesManager>()
					.GetComponent(currentPlayer.dashSoundIndex);
				dashAudioSource.PlayInstance(transformsManager_.GetWorldPosition(entityIndex));		    
                
			}
			else {
				currentPlayer.lastDashEndedAt = Time::Get().GetTime() - currentPlayer.dashStartedAt;
			}

			playerManager_.SetComponent(entityIndex, currentPlayer);
		}

		if (currentPlayer.shieldRegenTimeStamp < Time::Get().GetTime()
			&& currentPlayer.shieldLife < currentPlayer.maxShieldLife)
		{
			currentPlayer.shieldLife += currentPlayer.shieldRegenSpeed;
			playerManager_.SetComponent(entityIndex, currentPlayer);

			const auto materialId = modelsManager_.GetComponent(currentPlayer.shieldIndex).materialID;
			auto& mat = MaterialsManagerLocator::Get().GetMaterial(materialId);

			auto& newMat = reinterpret_cast<MaterialDiffuse&>(mat);
			Color color = newMat.GetColor();
			color.a = currentPlayer.shieldLife / currentPlayer.maxShieldLife;

			newMat.SetColor(color);
		}

	    switch (currentPlayer.state) {
	    case Player::PlayerState::DASHING:
		    Dash(entityIndex);
			currentPlayer = playerManager_.GetComponent(entityIndex);
		    break;
	    default:
		    currentPlayer.moveDirection = math::Vec2(0.0f, 0.0f);

			// Check the input
            if (!input::InputManager::usingController) {
			    if (inputManager_.IsActionHeld(input::Action::UP))
			    	currentPlayer.moveDirection.y += 1.0f;
			    if (inputManager_.IsActionHeld(input::Action::DOWN))
			    	currentPlayer.moveDirection.y -= 1.0f;
			    if (inputManager_.IsActionHeld(input::Action::RIGHT))
			        currentPlayer.moveDirection.x += 1.0f;
			    if (inputManager_.IsActionHeld(input::Action::LEFT))
			    	currentPlayer.moveDirection.x -= 1.0f;
            }
			else {
				//std::cout << "using Controller\n";
                currentPlayer.moveDirection = 
				    math::Vec2(
						input::InputManager::joyAxisLX,
					    -input::InputManager::joyAxisLY);
			}

			playerManager_.SetComponent(entityIndex, currentPlayer);
			Move(entityIndex);
			currentPlayer = playerManager_.GetComponent(entityIndex);
			break;
		}
		CheckPosition(entityIndex);

		// Manage sound of special attack
		if (inputManager_.IsActionDown(input::Action::ABILITIES) && currentPlayer.specialCapacityAvailableAt <= currentPlayer.destructionPoint) {
			
			ecs::AudioSourcesManager& audioSourcesManager = ecsManager_.GetComponentsManager<ecs::AudioSourcesManager>();

			//Stop undo sound(in case it was running)
			{
				audio::AudioSource audioSpecialAttackUndo = audioSourcesManager
					.GetComponent(currentPlayer.specialAttackIndex + 1);
				audioSpecialAttackUndo.Stop();
				audioSourcesManager.SetComponent(currentPlayer.specialAttackIndex + 1, audioSpecialAttackUndo);
			}
		    //Play charging sound
			{
				audio::AudioSource audioSpecialAttackCharging =
					audioSourcesManager.GetComponent(currentPlayer.specialAttackIndex);
				audioSpecialAttackCharging.Play(transformsManager_.GetWorldPosition(entityIndex));
				audioSourcesManager.SetComponent(currentPlayer.specialAttackIndex, audioSpecialAttackCharging);
			}

		}
		if (inputManager_.IsActionUp(input::Action::ABILITIES)) {
            if(currentPlayer.isPreparingSpecialAttack) {
				ecs::AudioSourcesManager& audioSourcesManager = ecsManager_.GetComponentsManager<ecs::AudioSourcesManager>();
				audio::AudioSource audioSpecialAttackUndo = audioSourcesManager
                    .GetComponent(currentPlayer.specialAttackIndex + 1);
				audioSpecialAttackUndo.Play(transformsManager_.GetWorldPosition(entityIndex));
				audioSourcesManager.SetComponent(currentPlayer.specialAttackIndex + 1, audioSpecialAttackUndo);
            }
			CleanSpecialCapacity(entityIndex);
		}

		// Check if the player is and can prepare the special attack
		if (inputManager_.IsActionHeld(input::Action::ABILITIES) &&
			currentPlayer.specialCapacityAvailableAt <= currentPlayer.destructionPoint) 
		{
			currentPlayer.isPreparingSpecialAttack = true;
			Weapon weapon = weaponManager_.GetComponent(currentPlayer.projectileWeapon);
			weapon.isShooting = false;
			weaponManager_.SetComponent(currentPlayer.projectileWeapon, weapon);
		} else {
			// Reset the values if special capacity not launch
			currentPlayer.isPreparingSpecialAttack = false;
			currentPlayer.currentSpecialCapacityRadius = 0.0f;
		}
		playerManager_.SetComponent(entityIndex, currentPlayer);
		
		if (currentPlayer.isPreparingSpecialAttack) {
			// Check if the player shoot the special attack
			if (inputManager_.IsActionDown(input::Action::MAIN_SHOOT))
				SpecialCapacity(entityIndex);
			else
				IncreaseSpecialCapacity(entityIndex);
		} else {
			ShootProjectile(entityIndex, inputManager_.IsActionHeld(input::Action::MAIN_SHOOT));

			/*if (inputManager_.IsActionHeld(input::Action::SECONDARY_TARGET))
				LockMissile(entityIndex);

			if (inputManager_.IsActionUp(input::Action::SECONDARY_SHOOT))
				ShootMissile(entityIndex);*/
		}

		ChunksManagerLocator::Get().UpdateActiveChunkWithPosition(transformsManager_.GetWorldPosition(entityIndex));
	}
	pok_EndProfiling(Player_Controller);
}

void PlayerController::Move(const ecs::EntityIndex entityIndex)
{
	// Get the player
	const Player currentPlayer = playerManager_.GetComponent(entityIndex);

	// Set the rotation to apply
	math::Transform transform = transformsManager_.GetComponent(entityIndex);
	math::Vec3 rotationToApply = transform.GetLocalRotation() * math::kRad2Deg;

	// Z Axis
	if (currentPlayer.moveDirection.x == 0.0f) {
		if (abs(rotationToApply.y) < 1.0f || abs(rotationToApply.y) > 360.0f - 1.0f)
			rotationToApply.y = 0.0f;
		else if (rotationToApply.y > 1.0f && rotationToApply.y <= kMaxRotation_.y)
			rotationToApply.y = rotationToApply.y - kRotationSpeed_.y;
		else
			rotationToApply.y = rotationToApply.y + kRotationSpeed_.y;

		if (abs(rotationToApply.z) < 1.0f || abs(rotationToApply.z) > 360.0f - 1.0f)
			rotationToApply.z = 0.0f;
		else if (rotationToApply.z > 1.0f && rotationToApply.z <= kMaxRotation_.z)
			rotationToApply.z = rotationToApply.z - kRotationSpeed_.z;
		else
			rotationToApply.z = rotationToApply.z + kRotationSpeed_.z;
	}
	else
	{
		float newYRotation;
		float newZRotation;

		newYRotation = rotationToApply.y - (kRotationSpeed_.y * currentPlayer.moveDirection.x);
		newZRotation = rotationToApply.z + (kRotationSpeed_.z * currentPlayer.moveDirection.x);

		if (abs(newYRotation) >= kMaxRotation_.y) {
			if (newYRotation < 0.0f)
				newYRotation = -kMaxRotation_.y;
			else
				newYRotation = kMaxRotation_.y;
		}

		if (abs(newZRotation) >= kMaxRotation_.z) {
			if (newZRotation < 0.0f)
				newZRotation = -kMaxRotation_.z;
			else
				newZRotation = kMaxRotation_.z;
		}

		rotationToApply.y = newYRotation;
		rotationToApply.z = newZRotation;
	}

	// X Axis
	if (currentPlayer.moveDirection.y == 0.0f) {
		if (abs(rotationToApply.x) < 1.0f || abs(rotationToApply.x) > 360.0f - 1.0f)
			rotationToApply.x = 0.0f;
		else if (rotationToApply.x > 1.0f && rotationToApply.x <= kMaxRotation_.x)
			rotationToApply.x = rotationToApply.x - kRotationSpeed_.x;
		else
			rotationToApply.x = rotationToApply.x + kRotationSpeed_.x;
	}
	else
	{
		float newXRotation;

		newXRotation = rotationToApply.x - (kRotationSpeed_.x * currentPlayer.moveDirection.y);

		if (abs(newXRotation) >= kMaxRotation_.x) {
			if (newXRotation < 0.0f)
				newXRotation = -kMaxRotation_.x;
			else
				newXRotation = kMaxRotation_.x;
		}

		rotationToApply.x = newXRotation;
	}

	// Apply rotation
	transform.SetLocalRotation(rotationToApply * math::kDeg2Rad);
	transformsManager_.SetComponent(entityIndex, transform);

	// Set the rotation percentage
	math::Vec3 rotationPercentage = {
		rotationToApply.x / kMaxRotation_.x,
		rotationToApply.y / kMaxRotation_.y,
		rotationToApply.z / kMaxRotation_.z,
	};

	// Get the entity rigidbody
	physics::Rigidbody rigid = rigidbodyManager_.GetComponent(entityIndex);

	math::Vec3 cameraRight = (CameraLocator::Get().GetRight()).Normalize();
	math::Vec3 cameraUp = (CameraLocator::Get().GetUp()).Normalize();

	//TODO(@Robin) This version does not work with multiple players
	math::Vec3 currentPosition = transformsManager_.GetComponent(entityIndex).GetLocalPosition();
	currentSpeeds_[0] = math::Vec2(
		-(currentPosition.x - oldPositions_[0].x),
		currentPosition.y - oldPositions_[0].y
	) / (Time::Get().deltaTime.count() / 1000.0);

	oldPositions_[0] = currentPosition;

	//// Check if the player does not move and has already a movement
	if (currentPlayer.moveDirection.x == 0 && rigid.linearVelocity != math::Vec3(0.0f) && rotationPercentage.z == 0.0f) {
		if (currentSpeeds_[0].x > kMinMoveSpeed_) {
			currentSpeeds_[0].x -= currentPlayer.moveSpeed * kDragSpeedDiminution_ * (Time::Get().deltaTime.count() / 1000.0);
		}
		else if (currentSpeeds_[0].x < -kMinMoveSpeed_) {
			currentSpeeds_[0].x += currentPlayer.moveSpeed * kDragSpeedDiminution_ * (Time::Get().deltaTime.count() / 1000.0);
		}
		else {
			currentSpeeds_[0].x = 0;
		}
	}
	else {
		currentSpeeds_[0].x += currentPlayer.moveSpeed * rotationPercentage.z;
	}

	if (currentPlayer.moveDirection.y == 0 && rigid.linearVelocity != math::Vec3(0.0f) && rotationPercentage.x == 0.0f) {
		if (currentSpeeds_[0].y > kMinMoveSpeed_) {
			currentSpeeds_[0].y -= currentPlayer.moveSpeed * kDragSpeedDiminution_ * (Time::Get().deltaTime.count() / 1000.0);
		}
		else if (currentSpeeds_[0].y < -kMinMoveSpeed_) {
			currentSpeeds_[0].y += currentPlayer.moveSpeed * kDragSpeedDiminution_ * (Time::Get().deltaTime.count() / 1000.0);
		}
		else {
			currentSpeeds_[0].y = 0;
		}
	}
	else {
		currentSpeeds_[0].y += currentPlayer.moveSpeed * -rotationPercentage.x;
	}

	// Check if the player has reach his maximum speed on the x axis
	if (abs(currentSpeeds_[0].x) >= currentPlayer.maxMoveSpeed)
		currentSpeeds_[0].x = currentPlayer.maxMoveSpeed * rotationPercentage.z;

	// Check if the player has reach his maximum speed on the y axis
	if (abs(currentSpeeds_[0].y) >= currentPlayer.maxMoveSpeed)
		currentSpeeds_[0].y = currentPlayer.maxMoveSpeed * -rotationPercentage.x;

	rigid.linearVelocity =
		cameraRight * currentSpeeds_[0].x + cameraUp * currentSpeeds_[0].y;

	rigidbodyManager_.SetComponent(entityIndex, rigid);
}

void PlayerController::Dash(const ecs::EntityIndex entityIndex) const
{
	// Get the player
	Player currentPlayer = playerManager_.GetComponent(entityIndex);

	// Set the movement to apply
	math::Vec3 direction;
	direction.x = currentPlayer.moveDirection.x * currentPlayer.dashSpeed;
	direction.y = currentPlayer.moveDirection.y * currentPlayer.dashSpeed;

	if (Time::Get().GetTime() - currentPlayer.dashStartedAt > currentPlayer.dashDuration) 
	{
		currentPlayer.state = Player::PlayerState::NONE;
		currentPlayer.isDashing = false;
	}

	// Get the entity rigidbody
	physics::Rigidbody rigid = rigidbodyManager_.GetComponent(entityIndex);

	math::Vec3 cameraRight = (CameraLocator::Get().GetRight()).Normalize();
	math::Vec3 cameraUp = (CameraLocator::Get().GetUp()).Normalize();

	// Apply the movement
	rigid.linearVelocity = (cameraRight * direction.x) + (cameraUp * direction.y);
	rigidbodyManager_.SetComponent(entityIndex, rigid);

	playerManager_.SetComponent(entityIndex, currentPlayer);
}

void PlayerController::ShootProjectile(const ecs::EntityIndex entityIndex, const bool isShooting) const
{
	Player player = playerManager_.GetComponent(entityIndex);
	Weapon weapon = weaponManager_.GetComponent(player.projectileWeapon);

	if (isShooting)
	{
		player.shieldRegenTimeStamp = Time::Get().GetTime() + player.timeBeforeShieldRegen;
	}

	weapon.isShooting = isShooting;

	weapon.shootDirection = player.fireDirection;
	weapon.origin = entityIndex;
	
	playerManager_.SetComponent(entityIndex, player);
	weaponManager_.SetComponent(player.projectileWeapon, weapon);
}

void PlayerController::ShootMissile(const ecs::EntityIndex entityIndex) const
{
	Player player = playerManager_.GetComponent(entityIndex);
	Weapon weapon = weaponManager_.GetComponent(player.missileWeapon);

	if (!player.lockingTargets.empty() && !weapon.isShooting) 
	{
		player.shieldRegenTimeStamp = Time::Get().GetTime() + player.timeBeforeShieldRegen;
		
		weapon.isShooting = true;
		weapon.targets = player.lockingTargets;
		weapon.origin = entityIndex;
		weaponManager_.SetComponent(player.missileWeapon, weapon);

		player.lockingTargets.clear();
		playerManager_.SetComponent(entityIndex, player);
	}
}

void PlayerController::LockMissile(const ecs::EntityIndex entityIndex) const
{
	Player player = playerManager_.GetComponent(entityIndex);
	Weapon weapon = weaponManager_.GetComponent(player.missileWeapon);

	// Check if the player target exist and that he can lock more targets
	if (player.currentTarget != ecs::kNoEntity && player.lockingTargets.size() < player.maxLockingTargets) 
	{
		bool currentTargetHasToBeAdded = true;

		// Check if the current target has already been locked
		const auto itTarget = std::find(player.lockingTargets.begin(), player.lockingTargets.end(), player.currentTarget);
		if (itTarget != player.lockingTargets.end()) { currentTargetHasToBeAdded = false; }

		// Add the target if needed
		if (currentTargetHasToBeAdded)
			player.lockingTargets.push_back(player.currentTarget);

		playerManager_.SetComponent(entityIndex, player);
	}
}

void PlayerController::CheckPosition(const ecs::EntityIndex entityIndex)
{
	math::Transform transform = transformsManager_.GetComponent(entityIndex);
	const Player player = playerManager_.GetComponent(entityIndex);

	math::Vec3 pos = transform.GetLocalPosition();

	if (pos.x > player.movementArea.x) 
	{
		pos.x = player.movementArea.x;
		oldPositions_[0].x = pos.x;
	}
	else if (pos.x < -player.movementArea.x) 
	{
		pos.x = -player.movementArea.x;
		oldPositions_[0].x = pos.x;
	}

	if (pos.y > player.movementArea.y) 
	{
		pos.y = player.movementArea.y;
		oldPositions_[0].y = pos.y;
	}
	else if (pos.y < -player.movementArea.y) 
	{
		pos.y = -player.movementArea.y;
		oldPositions_[0].y = pos.y;
	}

	pos.z = 40.0f;

	transform.SetLocalPosition(pos);
	transformsManager_.SetComponent(entityIndex, transform);
}

void PlayerController::TakeDamage(const ecs::EntityIndex entityIndex, Player& player, const int damage) const
{
	player.shieldRegenTimeStamp = Time::Get().GetTime() + player.timeBeforeShieldRegen;
	ecs::AudioSourcesManager audioSourcesManager = ecsManager_.GetComponentsManager<ecs::AudioSourcesManager>();
	if (player.shieldLife > 0.0f)
	{
		player.shieldLife -= static_cast<float>(damage);
		if (player.shieldLife < 0.0f) {
			player.shieldLife = 0.0f;

            //Play sound to tell the player the shield is down
			audio::AudioSource shieldDownAudioSource = audioSourcesManager.GetComponent(player.shieldIndex + 1);
			shieldDownAudioSource.PlayInstance(transformsManager_.GetWorldPosition(entityIndex));
		}else {
			//Play sound to tell the player the shield is hit
			audio::AudioSource shieldDownAudioSource = audioSourcesManager.GetComponent(player.shieldIndex);
			shieldDownAudioSource.PlayInstance(transformsManager_.GetWorldPosition(entityIndex));
		}
		
		const auto materialId = modelsManager_.GetComponent(player.shieldIndex).materialID;
		auto& mat = MaterialsManagerLocator::Get().GetMaterial(materialId);

		auto& newMat = reinterpret_cast<MaterialDiffuse&>(mat);
		Color color = newMat.GetColor();
		color.a = player.shieldLife / player.maxShieldLife;

		newMat.SetColor(color);
	}
	else
	{
		player.life -= damage;
		std::cout << player.life << "\n";
		//Play sound to tell the player the player is hit
		audio::AudioSource playerHitAudioSource = audioSourcesManager.GetComponent(player.hitSoundIndex);
		playerHitAudioSource.PlayInstance(transformsManager_.GetWorldPosition(entityIndex));

		switch (player.lifeState)
		{
		case Player::LifeState::NONE:
			if (player.life * 100 / player.maxLife <= 75)
			{
				player.lifeState = Player::LifeState::ONE;
				const ecs::EntityIndex particleIndex = gamePrefabsManager_.Instantiate("ParticleFire")[0];
				transformsManager_.SetParent(particleIndex, player.damageAreas[0]);
			}
			break;
		case Player::LifeState::ONE:
			if (player.life * 100 / player.maxLife <= 50)
			{
				player.lifeState = Player::LifeState::TWO;
				const ecs::EntityIndex particleIndex = gamePrefabsManager_.Instantiate("ParticleFire")[0];
				transformsManager_.SetParent(particleIndex, player.damageAreas[1]);
			}
			break;
		case Player::LifeState::TWO:
			if (player.life * 100 / player.maxLife <= 25)
			{
				player.lifeState = Player::LifeState::THREE;
				const ecs::EntityIndex particleIndex = gamePrefabsManager_.Instantiate("ParticleFire")[0];
				transformsManager_.SetParent(particleIndex, player.damageAreas[2]);
			}
			break;
		case Player::LifeState::THREE:
			player.life = 1;
			if (player.life <= 0)
			{
				const ecs::EntityIndex particleIndex = gamePrefabsManager_.Instantiate("ParticleExplo")[0];
				transformsManager_.SetComponent(particleIndex, math::Transform(transformsManager_.GetWorldPosition(entityIndex)));
				player.state = (Player::PlayerState)4;
				// TODO (@C�dric) Instantiate explosion particle at player world pos and destroy the player
			}
			break;
		default:
			break;
		}
	}

	playerManager_.SetComponent(entityIndex, player);
}

void PlayerController::SpecialCapacity(const ecs::EntityIndex entityIndex)
{
	Player currentPlayer = playerManager_.GetComponent(entityIndex);
	currentPlayer.shieldRegenTimeStamp = Time::Get().GetTime() + currentPlayer.timeBeforeShieldRegen;

	SpecialAttack specialAttack;
	specialAttack.expansionSpeed = 15.0f;
	specialAttack.maxRadius = currentPlayer.currentSpecialCapacityRadius;
	specialAttackManager_.SetComponent(currentPlayer.specialAttackIndex, specialAttack);

	currentPlayer.currentSpecialCapacityRadius = 0.0f;
	currentPlayer.destructionPoint = 0;
	playerManager_.SetComponent(entityIndex, currentPlayer);

	CleanSpecialCapacity(entityIndex);

	//Launch and stop the right sounds
	audio::AudioSource audioSpecialAttackLaunch = ecsManager_
        .GetComponentsManager<ecs::AudioSourcesManager>()
        .GetComponent(currentPlayer.specialAttackIndex + 2);
	audioSpecialAttackLaunch.PlayInstance(transformsManager_.GetWorldPosition(entityIndex));

	auto particleSystemManager = ecsManager_.GetComponentsManager<ecs::ParticleSystemsManager>();
	auto specialAttackComponent = particleSystemManager.GetComponent(currentPlayer.specialAttackIndex);
	specialAttackComponent.rateOverTime = 0.0f;
	particleSystemManager.SetComponent(currentPlayer.specialAttackIndex, specialAttackComponent);
}

void PlayerController::IncreaseSpecialCapacity(const ecs::EntityIndex entityIndex) {
	Player currentPlayer = playerManager_.GetComponent(entityIndex);
	const math::Vec3 playerPosition = transformsManager_.GetWorldPosition(entityIndex);
	
	if (currentPlayer.currentSpecialCapacityRadius < currentPlayer.specialCapacityMaxRadius)
		currentPlayer.currentSpecialCapacityRadius += currentPlayer.specialCapacityIncrementedBy;
	else
		currentPlayer.currentSpecialCapacityRadius = currentPlayer.specialCapacityMaxRadius;

	playerManager_.SetComponent(entityIndex, currentPlayer);
	
	// TODO (@game) : Manage the sound correctly for the special attack (raise the effect, keep the effect and feedback that the maximum has been reach)

	// Show wich visible elements are in range of the special capacity
	const std::vector<ecs::EntityIndex>& visibleEntities = ecsManager_.GetDrawnEntities();

	size_t alreadyRegisteredIndex = -1;
	
	//Select only visible entities that are destructible elements
	for (const ecs::EntityIndex visibleEntityIndex : visibleEntities) {
		if (ecsManager_.HasComponent(visibleEntityIndex, ecs::ComponentType::DESTRUCTIBLE_ELEMENT)) {
			// Avoid locking indestructible element
			if (ecsManager_.GetComponentsManager<DestructibleElementManager>().GetComponent(visibleEntityIndex).isIndestructible)
				continue;
			
			// Check if pair already exist
			for (size_t i = 0; i < entitiesInRange_.size(); i++) {
				if (entitiesInRange_[i] == visibleEntityIndex) {
					alreadyRegisteredIndex = i;
					break;
				}
			}

			const math::Vec3 visibleEntityPosition = transformsManager_.GetWorldPosition(visibleEntityIndex);
			
			if(alreadyRegisteredIndex != -1) {
				// Check if still in range
				if (math::Vec3::GetDistanceManhattan(
					playerPosition, visibleEntityPosition) >
					currentPlayer.currentSpecialCapacityRadius * 0.01) {
					ecsManager_.DestroyEntity(activeTargets_[alreadyRegisteredIndex]);
					activeTargets_.erase(activeTargets_.begin() + alreadyRegisteredIndex);
					entitiesInRange_.erase(entitiesInRange_.begin() + alreadyRegisteredIndex);
				}
			}
			else {
				if (math::Vec3::GetDistanceManhattan(
					playerPosition, visibleEntityPosition) <
					currentPlayer.currentSpecialCapacityRadius / 150.0f) {
					const ecs::EntityIndex targetSight = ecsManager_.AddEntity(targetId_);
					ecsManager_.SetEntityVisible(targetSight, ecs::EntityStatus::ACTIVE);
					math::Transform targetSightTransform = transformsManager_.GetComponent(targetSight);
					targetSightTransform.SetLocalRotation(
						math::Quaternion::LookRotation(
							CameraLocator::Get().GetFront(),
							CameraLocator::Get().GetUp()).GetEulerAngles());
					targetSightTransform.SetLocalPosition(visibleEntityPosition);
					targetSightTransform.SetLocalScale({ 10.0f });
					transformsManager_.SetComponent(targetSight, targetSightTransform);
					entitiesInRange_.emplace_back(visibleEntityIndex);
					activeTargets_.emplace_back(targetSight);
				}
			}

			alreadyRegisteredIndex = -1;
		}
	}
}

void PlayerController::CleanSpecialCapacity(
	const ecs::EntityIndex entityIndex)
{
	Player currentPlayer = playerManager_.GetComponent(entityIndex);

	currentPlayer.isPreparingSpecialAttack = false;
	playerManager_.SetComponent(entityIndex, currentPlayer);

	// Delete all the targets
	for (size_t i = 0; i < activeTargets_.size(); i++) {
		ecsManager_.DestroyEntity(activeTargets_[i]);
	}

	ecs::AudioSourcesManager& audioSourcesManager = ecsManager_.GetComponentsManager<ecs::AudioSourcesManager>();
	audio::AudioSource audioSpecialAttackCharging = audioSourcesManager.GetComponent(currentPlayer.specialAttackIndex);
	audioSpecialAttackCharging.Stop();
	audioSourcesManager.SetComponent(currentPlayer.specialAttackIndex, audioSpecialAttackCharging);

	activeTargets_.clear();
	entitiesInRange_.clear();
}

void PlayerController::OnAddComponent(
	const ecs::EntityIndex entityIndex,
	const ecs::ComponentMask component)
{
	//Check if the entity have the component required
	if ((component & ecs::ComponentType::PLAYER) != ecs::ComponentType::PLAYER) { return; }

	//check if the entity has already been added the the system by the past
	if (!registeredEntities_.exist(entityIndex)) 
	{
		//Add the entity to the list of registered entities and sort it
		registeredEntities_.insert(entityIndex);

		ecsManager_.RegisterObserverTriggerEnter(
			entityIndex,
			[this](ecs::EntityIndex entityIndex, physics::Collision collision)
		{this->OnTriggerEnter(entityIndex, collision); });
	}

	//Check if the entity is active
	if (!ecsManager_.HasComponent(entityIndex, ecs::EntityFlag::IS_ACTIVE)) { return; }

	if (entityIndexes_.exist(entityIndex)) { return; }

	//Add the entity to the list of entities and sort it
	entityIndexes_.insert(entityIndex);

	oldPositions_[entityIndexes_.size() - 1] = transformsManager_.GetComponent(entityIndex).GetLocalPosition();
}

void PlayerController::OnRemoveComponent(
	const ecs::EntityIndex entityIndex,
	const ecs::ComponentMask componentMask)
{
	if ((componentMask & ecs::ComponentType::PLAYER) != ecs::ComponentType::PLAYER)
	{
		return;
	}
	//Check if the entity is in the list of entities
	if (!entityIndexes_.exist(entityIndex)) { return; }

	const auto it = entityIndexes_.find(entityIndex);

	//remove if one of the removed components
	entityIndexes_.erase(it);
}

void PlayerController::OnEntitySetActive(const ecs::EntityIndex entityIndex)
{
	//Check if the activated entity is a missile
	if (!ecsManager_.HasComponent(entityIndex, ecs::ComponentType::PLAYER)) { return; }

	if (entityIndexes_.exist(entityIndex)) { return; }

	//Add the entity to the list of entities and sort it
	entityIndexes_.insert(entityIndex);

	oldPositions_[entityIndexes_.size() - 1] = transformsManager_.GetComponent(entityIndex).GetLocalPosition();
}

void PlayerController::OnEntitySetInactive(const ecs::EntityIndex entityIndex)
{
	//Check if the entity is in the list of entities
	if (!entityIndexes_.exist(entityIndex)) { return; }

	const auto it = entityIndexes_.find(entityIndex);

	//remove the deactivated entity for the list of entities
	entityIndexes_.erase(it);
}

void PlayerController::OnTriggerEnter(const ecs::EntityIndex entityIndex, const physics::Collision collision) {
	Player player = playerManager_.GetComponent(entityIndex);
	if (ecsManager_.HasComponent(collision.otherEntity, ecs::ComponentType::DESTRUCTIBLE_ELEMENT) &&
		!ecsManager_.HasComponent(collision.otherEntity, ecs::ComponentType::MISSILE)) {
		// Destroy the destructible element
		DestructibleElement destructibleElement = destructibleElementManager_.GetComponent(collision.otherEntity);
		if (!destructibleElement.isIndestructible) { destructibleElement.life = 0; }

		if (player.state != Player::PlayerState::DASHING) {
			TakeDamage(entityIndex, player, kDamageFromCollision_);
		}
		else if (!destructibleElement.isIndestructible) {
			// Bump the destructible element
			destructibleElement.destroyedBy = entityIndex;
			destructibleElement.destructionTime = kDashBumpDestructionCooldown;
			ecsManager_.RemoveComponent(collision.otherEntity, ecs::ComponentType::JIGGLE);
			physics::Rigidbody destructibleElementRigidbody = rigidbodyManager_.GetComponent(collision.otherEntity);
			destructibleElementRigidbody.linearVelocity = (transformsManager_.GetWorldPosition(collision.otherEntity) - transformsManager_.GetWorldPosition(entityIndex)).Normalize() * kBumpStrength_;
			destructibleElementRigidbody.angularVelocity = { 50.0f };
			rigidbodyManager_.SetComponent(collision.otherEntity, destructibleElementRigidbody);
		}
		destructibleElementManager_.SetComponent(collision.otherEntity, destructibleElement);
	}
	else if (ecsManager_.HasComponent(collision.otherEntity, ecs::ComponentType::PROJECTILE)) 
	{
		const Projectile projectile = projectileManager_.GetComponent(collision.otherEntity);

		if (projectile.origin == ecs::kNoEntity) 
		{
			TakeDamage(entityIndex, player, projectile.damage);
		}
	}
	else if (ecsManager_.HasComponent(collision.otherEntity, ecs::ComponentType::MISSILE)) 
	{
		const Missile missile = missilesManager_.GetComponent(collision.otherEntity);

		if (missile.origin == ecs::kNoEntity) 
		{
			TakeDamage(entityIndex, player, missile.damage);
		}
	}
}
}// namespace pok::game
