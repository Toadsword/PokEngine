#include <Game/enemies_system.h>
#include <CoreEngine/engine.h>
#include <Game/Ecs/game_archetype.h>
#include <Game/ComponentManagers/enemies_manager.h>
#include <Game/ComponentManagers/destructible_element_manager.h>
#include <Utility/profiler.h>
#include <Game/game.h>
#include <Game/Components/enemy.h>

namespace poke::game {
EnemiesSystem::EnemiesSystem(Engine& engine, Game& game) :
	GameSystem(engine, game),
	splineFollowersManager_(EcsManagerLocator::Get().GetComponentsManager<ecs::SplineFollowersManager>()),
	destructibleElementManager_(EcsManagerLocator::Get().GetComponentsManager<DestructibleElementManager>()),
    enemiesManager_(EcsManagerLocator::Get().GetComponentsManager<EnemiesManager>()),
    weaponManager_(EcsManagerLocator::Get().GetComponentsManager<WeaponManager>()),
    transformsManager_(EcsManagerLocator::Get().GetComponentsManager<ecs::TransformsManager>()) {
	game.RegisterObserverUpdate([this] { this->OnUpdate(); });
	
	ObserveEntitySetActive();
	ObserveEntitySetInactive();

    ecsManager_.RegisterObserverAddComponent(
		[this](ecs::EntityIndex entityIndex, ecs::ComponentMask component)
	    {this->OnEntityAddComponent(entityIndex, component); }
	);
	ecsManager_.RegisterObserverRemoveComponent(
		[this](ecs::EntityIndex entityIndex, ecs::ComponentMask component)
	    {this->OnEntityRemoveComponent(entityIndex, component); }
	);

    for(size_t index = 0; index < maxPlayerNb; index++) {
		oldPlayerPos_[index] = { 0.0f, 0.0f, 0.0f };
		newPlayerPos_[index] = { 0.0f, 0.0f, 0.0f };
    }

    for(size_t index = 0; index < futurePosNb; index++) {
		playerFuturePositions_[index] = { 0.0f };
    }
}

void EnemiesSystem::OnUpdate() {
	pok_BeginProfiling(Enemies_System, 0);
    if(enemyIndexes_.size() == 0) {
		pok_EndProfiling(Enemies_System);
        return;
    }
	const auto deltaTime = static_cast<float>(Time::Get().deltaTime.count() / 1000.0);

    //player Direction
	const size_t playerSize = players_.size();
    for(size_t index = 0; index < playerSize; index++) {
		newPlayerPos_[index] = transformsManager_.GetWorldPosition(players_[index]);
    }
    for(size_t index = 0; index < players_.size(); index++) {
	    playerDirections_[index] = (newPlayerPos_[index] - oldPlayerPos_[index]);
		playerSpeeds_[index] = playerDirections_[index].GetMagnitude() / deltaTime;
		playerDirections_[index] = playerDirections_[index].Normalize();
    }
    oldPlayerPos_ = newPlayerPos_;

    for(float i = 0; i < futurePosNb; i++) {
		playerFuturePositions_[i] = 
			newPlayerPos_[0] +
			playerDirections_[0] * 
			playerSpeeds_[0] * 
			kTimeBetweenCheck_ * 
			i;
    }

    // Main behavior of enemies
    for (const ecs::EntityIndex entityIndex : enemyIndexes_) {
		Enemy enemy = enemiesManager_.GetComponent(entityIndex);

		math::Transform enemyTransform = transformsManager_.GetComponent(entityIndex);

		math::Matrix4 worldMatrix = enemyTransform.GetWorldMatrix();
		math::Vec3 direction = math::Vec3(worldMatrix[0][2], worldMatrix[1][2], worldMatrix[2][2]).Normalize();

        switch (enemy.state) {
		    case Enemy::State::LAUNCH: {
				enemy.state = Enemy::State::APPROACH;
				enemiesManager_.SetComponent(entityIndex, enemy);
			    break;
		    }
		    case Enemy::State::APPROACH: {
				auto splineFollower = splineFollowersManager_.GetComponent(entityIndex);
				splineFollower.speed = splineFollower.speed + (enemy.approachSpeed - splineFollower.speed) * kLerpSpeedFactor_ * deltaTime;
			    if (splineFollowersManager_.GetComponent(entityIndex).lastPoint >= enemy.splineFireStartPoint) {
					enemy.state = Enemy::State::ATTACK;
					enemiesManager_.SetComponent(entityIndex, enemy);
					splineFollower.speed = enemy.attackSpeed;
			    }
				splineFollowersManager_.SetComponent(entityIndex, splineFollower);
			    break;
		    }
			case Enemy::State::ATTACK: {
                //If there is player, start fire
                if(players_.size() > 0) {
					pok_BeginProfiling(Enemy_Fire, 0);
					for (ecs::EntityIndex enemyWeaponIndex : enemy.weapons) {
						Weapon weapon = weaponManager_.GetComponent(enemyWeaponIndex);
						const math::Vec3 weaponPos = transformsManager_.GetWorldPosition(weapon.gunPositions[weapon.activeGunID]);
						math::Vec3 playerFuturePos(0);
						for (size_t i = 0; i < futurePosNb; i++) {
							if ((playerFuturePositions_[i] - weaponPos).GetMagnitude() <
								projectileFutureDistances_[i]) {
								playerFuturePos = playerFuturePositions_[i];
								break;
							}
						}
                        

						math::Vec3 aimDirection;
						if (playerFuturePos == math::Vec3(0)) {
							aimDirection = direction;
					        weapon.isShooting = false;
						}
						else {
							aimDirection = (playerFuturePos - weaponPos).Normalize();
					        weapon.isShooting = true;
						}
						weapon.origin = ecs::kNoEntity;
						weapon.shootDirection = aimDirection;
						weapon.targets[0] = players_[0];
						weaponManager_.SetComponent(enemyWeaponIndex, weapon);
					}
					pok_EndProfiling(Enemies_Fire);
                }

				auto splineFollower = splineFollowersManager_.GetComponent(entityIndex);
				splineFollower.speed = splineFollower.speed + (enemy.attackSpeed - splineFollower.speed) * kLerpSpeedFactor_ * deltaTime;
				if (splineFollower.lastPoint >= enemy.splineFireEndPoint) {
					enemy.state = Enemy::State::FLEE;
					enemiesManager_.SetComponent(entityIndex, enemy);
				}
				splineFollowersManager_.SetComponent(entityIndex, splineFollower);
				break;
			}
            case Enemy::State::FLEE: {
				auto splineFollower = splineFollowersManager_.GetComponent(entityIndex);
				splineFollower.speed = splineFollower.speed + (enemy.fleeSpeed - splineFollower.speed) * kLerpSpeedFactor_ * deltaTime;
				if (splineFollower.totalPercentage >= kPercentageFullCheck) {
					enemy.state = Enemy::State::RESET;
					enemiesManager_.SetComponent(entityIndex, enemy);
					splineFollower.speed = 0.0f;
				}
				splineFollowersManager_.SetComponent(entityIndex, splineFollower);

				//Stop firing 
				for (ecs::EntityIndex enemyWeaponIndex : enemy.weapons) {
    				Weapon weapon = weaponManager_.GetComponent(enemyWeaponIndex);
    				weapon.isShooting = false;
    				weaponManager_.SetComponent(enemyWeaponIndex, weapon);
				}
				break;
			}
			case Enemy::State::DYING: {
				enemy.state = Enemy::State::RESET;
				enemiesManager_.SetComponent(entityIndex, enemy);

				//Stop firing 
				for (ecs::EntityIndex enemyWeaponIndex : enemy.weapons) {
					Weapon weapon = weaponManager_.GetComponent(enemyWeaponIndex);
					weapon.isShooting = false;
					weaponManager_.SetComponent(enemyWeaponIndex, weapon);
				}
				break;
		    }
            case Enemy::State::RESET: {
				enemy.state = Enemy::State::INACTIVE;
				enemiesManager_.SetComponent(entityIndex, enemy);

				//Stop firing in death case
				Weapon weapon = weaponManager_.GetComponent(entityIndex);
				weapon.isShooting = false;
				weaponManager_.SetComponent(entityIndex, weapon);
		        break;
            }
			case Enemy::State::INACTIVE: {
				enemyToSetInactiveIndexes_.emplace_back(entityIndex);
                break;
		    }
			default:
				break;
        }
    }

	for (ecs::EntityIndex entityIndex : enemySplineIndexes_) {
		ecs::SplineFollower splineFollower = splineFollowersManager_.GetComponent(entityIndex);
		Enemy enemy = enemiesManager_.GetComponent(entityIndex);
		math::Transform transform = transformsManager_.GetComponent(entityIndex);
		if (enemy.state != Enemy::State::DYING ||
			enemy.state != Enemy::State::RESET ||
			enemy.state != Enemy::State::STAND_BY)
		{
			transform.SetLocalPosition(splineFollower.spline.Lerp(splineFollower.lastPoint, splineFollower.segmentPercentage));
			size_t lastpoint = splineFollower.lastPoint;
			const size_t length = splineFollower.spline.GetPoints().size();
			float percentage = splineFollower.segmentPercentage;
            if(lastpoint == length - 2 && percentage > kMaxPercentage_) {
				lastpoint++;
				percentage = 0.0f;
            }
            else {
				percentage += kAugmentationPercentage_;
                if(percentage > 1.0f) {
					percentage--;
					lastpoint++;
                }
            }
			transform.SetLocalRotation(
                math::Quaternion::FromToRotation(
					{ 0.0f, 0.0f, 1.0f },
					(splineFollower.spline.Lerp(lastpoint, percentage) - transform.GetLocalPosition()).Normalize() 
				).GetEulerAngles()
			);
			
			transformsManager_.SetComponentFromWorldTransform(entityIndex, transform);
		}
	}

    for (size_t i = 0; i < enemyToSetInactiveIndexes_.size(); i++) {
		ecsManager_.SetActive(enemyToSetInactiveIndexes_[i], ecs::EntityStatus::INACTIVE);
    }
	enemyToSetInactiveIndexes_.clear();
	pok_EndProfiling(Enemies_System); 
}


void EnemiesSystem::OnEntitySetActive(const ecs::EntityIndex entityIndex) {
	//Check if the activated entity is a missile
	if (ecsManager_.HasComponent(entityIndex, ecs::ComponentType::ENEMY)) {
        if(!enemyIndexes_.exist(entityIndex)) {
	        //Add the entity to the list of entities and sort it
	        enemyIndexes_.insert(entityIndex);

		    Enemy enemy = enemiesManager_.GetComponent(entityIndex);
		    enemy.state = Enemy::State::LAUNCH;
		    enemiesManager_.SetComponent(entityIndex, enemy);

			ecs::AudioSourcesManager audioSourcesManager = ecsManager_.GetComponentsManager<ecs::AudioSourcesManager>();
			audio::AudioSource reactorAudioSource = audioSourcesManager.GetComponent(enemy.enemyReactorIndex);
			reactorAudioSource.Play(transformsManager_.GetWorldPosition(entityIndex));
			audioSourcesManager.SetComponent(enemy.enemyReactorIndex, reactorAudioSource);
        }

        if(ecsManager_.HasComponent(entityIndex, ecs::ComponentType::SPLINE_FOLLOWER)) {
            if(enemySplineIndexes_.exist(entityIndex)) { return; }
			enemySplineIndexes_.insert(entityIndex);
        }
	    return;
	}

	//Check if the activated entity is a missile
	if (ecsManager_.HasComponent(entityIndex, ecs::ComponentType::PLAYER)) {
        if(players_.exist(entityIndex)) { return; }
	    //Add the entity to the list of entities and sort it
	    players_.insert(entityIndex);
	}
}
	
void EnemiesSystem::OnEntitySetInactive(const ecs::EntityIndex entityIndex) { 
	//Check if the entity is in the list of entities
	if (enemyIndexes_.exist(entityIndex)) {
		const auto it = enemyIndexes_.find(entityIndex);

	    //remove the deactivated entity for the list of entities
	    enemyIndexes_.erase(it);

        if(enemySplineIndexes_.exist(entityIndex)) {
			const auto itSpline = enemySplineIndexes_.find(entityIndex);
			enemySplineIndexes_.erase(itSpline);
        }

	    return;
	}

	if (players_.exist(entityIndex)) {
		const auto it = players_.find(entityIndex);

		//remove the deactivated entity for the list of entities
		players_.erase(it);
	}
}

void EnemiesSystem::OnEntityAddComponent(const ecs::EntityIndex entityIndex, const ecs::ComponentMask component) {
	//Check if the entity have the component required
	if ((component & ecs::ComponentType::ENEMY) == ecs::ComponentType::ENEMY) { 
	    //Check if the entity is active
	    if (ecsManager_.HasComponent(entityIndex, ecs::EntityFlag::IS_ACTIVE)) {
			if (!enemyIndexes_.exist(entityIndex)) {
				//Add the entity to the list of entities and sort it
				enemyIndexes_.insert(entityIndex);
			}

			if (ecsManager_.HasComponent(entityIndex, ecs::ComponentType::SPLINE_FOLLOWER)) {
				if (enemySplineIndexes_.exist(entityIndex)) { return; }
				enemySplineIndexes_.insert(entityIndex);
			}
	    }

        if(!projectileSpeedIsSet_) {
        	// TODO (@Game) : Find a way to get the speed of the projectile that do not crash
            projectileSpeed_ = 1000.0f;
			projectileSpeedIsSet_ = true;

            for(float i = 0; i < futurePosNb; i++) {
				projectileFutureDistances_[i] = projectileSpeed_ * kTimeBetweenCheck_ * i;
            }
        }
        return;
	}

    if((component & ecs::ComponentType::SPLINE_FOLLOWER) == ecs::ComponentType::SPLINE_FOLLOWER &&
		ecsManager_.HasComponent(entityIndex, ecs::ComponentType::ENEMY)) {
		if (enemySplineIndexes_.exist(entityIndex)) { return; }
		enemySplineIndexes_.insert(entityIndex);
    }

	//Check if the entity have the component required
	if ((component & ecs::ComponentType::PLAYER) == ecs::ComponentType::PLAYER) {
	    //Check if the entity is active
	    if (ecsManager_.HasComponent(entityIndex, ecs::EntityFlag::IS_ACTIVE)) {
            if(players_.exist(entityIndex)){ return; }
	        //Add the entity to the list of entities and sort it
	        players_.insert(entityIndex);
	    }
	}
}

void EnemiesSystem::OnEntityRemoveComponent(ecs::EntityIndex entityIndex, ecs::ComponentMask component) {
	if ((component & ecs::ComponentType::ENEMY) == ecs::ComponentType::ENEMY) {
	    //Check if the entity is in the list of entities
		if (enemyIndexes_.exist(entityIndex)) {
			const auto it = enemyIndexes_.find(entityIndex);
			//remove if one of the removed components
			enemyIndexes_.erase(it);

			const Enemy enemy = enemiesManager_.GetComponent(entityIndex);

			ecs::AudioSourcesManager audioSourcesManager = ecsManager_.GetComponentsManager<ecs::AudioSourcesManager>();
			audio::AudioSource reactorAudioSource = audioSourcesManager.GetComponent(enemy.enemyReactorIndex);
			reactorAudioSource.Play(transformsManager_.GetWorldPosition(entityIndex));
			audioSourcesManager.SetComponent(enemy.enemyReactorIndex, reactorAudioSource);
		}

        if(ecsManager_.HasComponent(entityIndex, ecs::ComponentType::SPLINE_FOLLOWER)) {
			if (!enemySplineIndexes_.exist(entityIndex)) { return; }
			const auto itSpline = enemySplineIndexes_.find(entityIndex);
			enemySplineIndexes_.erase(itSpline);
        }
		return;
	}

    if((component & ecs::ComponentType::SPLINE_FOLLOWER) == ecs::ComponentType::SPLINE_FOLLOWER) {
        if((component & ecs::ComponentType::ENEMY) == ecs::ComponentType::ENEMY ||
			ecsManager_.HasComponent(entityIndex, ecs::ComponentType::ENEMY))
		{
            if(!enemySplineIndexes_.exist(entityIndex)) { return; }
			const auto itSpline = enemySplineIndexes_.find(entityIndex);
			enemySplineIndexes_.erase(itSpline);
            return;
        }
    }

	if ((component & ecs::ComponentType::PLAYER) == ecs::ComponentType::PLAYER) {
	    //Check if the entity is in the list of entities
	    if (players_.exist(entityIndex)) {
	        const auto it = players_.find(entityIndex);

	        //remove if one of the removed components
	        players_.erase(it);
	    }
	}
}
}//namespace poke::game