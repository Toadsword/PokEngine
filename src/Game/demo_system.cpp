#include <Utility/log.h>
#include <CoreEngine/engine.h>
#include <Utility/profiler.h>
#include <Utility/time_custom.h>
#include <Game/ComponentManagers/player_manager.h>
#include <Game/ComponentManagers/weapon_manager.h>
#include <Game/ComponentManagers/destructible_element_manager.h>
#include <Game/Ecs/game_archetype.h>
#include <Game/game.h>
#include <Game/demo_system.h>

namespace poke::game {
	DemoSystem::DemoSystem(Engine& engine, Game& game)
		: GameSystem(engine, game),
		camera_(*game.GetCameraPtr()),
		time_(Time::Get()),
		playerManager_(EcsManagerLocator::Get().GetComponentsManager<PlayerManager>()),
		weaponManager_(EcsManagerLocator::Get().GetComponentsManager<WeaponManager>()),
		transformsManager_(EcsManagerLocator::Get().GetComponentsManager<ecs::TransformsManager>()),
		splineFollowersManager_(EcsManagerLocator::Get().GetComponentsManager<ecs::SplineFollowersManager>()),
		gamePrefabsManager_(reinterpret_cast<GamePrefabsManager&>(PrefabsManagerLocator::Get())),
		gizmoCommandBuffer_(GraphicsEngineLocator::Get().GetGizmoCommandBuffer()),
		trailRendererManager_(EcsManagerLocator::Get().GetComponentsManager<ecs::TrailRendererManager>()),
		rigidbodyManager_(EcsManagerLocator::Get().GetComponentsManager<ecs::RigidbodyManager>()),
		jiggleManager_(EcsManagerLocator::Get().GetComponentsManager<JiggleManager>()),
		enemiesManager_(EcsManagerLocator::Get().GetComponentsManager<EnemiesManager>()),
		splineStateManager_(ecsManager_.GetComponentsManager<poke::game::SplineStateManager>())
{
    ObserveLoadScene();
}

void DemoSystem::OnLoadScene() {
	//gamePrefabsManager_.AddPrefab("Player");
	//gamePrefabsManager_.AddPrefab("TargetSight");
	//gamePrefabsManager_.AddPrefab("LightFighter");
	//gamePrefabsManager_.AddPrefab("ProjectilePlayer");
	//gamePrefabsManager_.AddPrefab("MissilePlayer");
	//gamePrefabsManager_.AddPrefab("Projectile");
	//gamePrefabsManager_.AddPrefab("Missile");

	for (size_t i = 0; i < asteroidPrefabs_.size(); i++)
		gamePrefabsManager_.AddPrefab(asteroidPrefabs_[i]);
	
	// Check if the scene is the main menu
	if (SceneManagerLocator::Get().GetActiveScene().GetSceneName() == "Demo") {
		InitializeScene();
		systemIsRunning_ = true;
	}
	else
		systemIsRunning_ = false;
}

void DemoSystem::InitializeScene() {
	// Set the pseudo camera
	cameraSplineFollower_ = ecsManager_.AddEntity();

	math::Transform transform;
	transform.SetLocalPosition(kCameraBasePosition_);
	transformsManager_.SetComponent(cameraSplineFollower_, transform);

	std::vector<math::Vec3>	splinePoints;
	math::Vec3 lastPoint = kCameraBasePosition_;
	lastPoint.z = 3000.0f;
	splinePoints.push_back(kCameraBasePosition_ - playerPosOffset_);
	splinePoints.push_back(kCameraBasePosition_);
	splinePoints.push_back(kCameraBasePosition_ + playerPosOffset_);
	splinePoints.push_back(kCameraBasePosition_ + (playerPosOffset_ * 2));
	splinePoints.push_back(kCameraBasePosition_ + (playerPosOffset_ * 3));
	splinePoints.push_back(kCameraBasePosition_ + (playerPosOffset_ * 4));
	splinePoints.push_back(kCameraBasePosition_ + (playerPosOffset_ * 5));
	splinePoints.push_back(kCameraBasePosition_ + (playerPosOffset_ * 6));
	splinePoints.push_back(kCameraBasePosition_ + (playerPosOffset_ * 7));
	splinePoints.push_back(lastPoint);
	lastPoint.z = lastPoint.z + 20.0f;
	splinePoints.push_back(lastPoint);
	
	ecs::SplineFollower spline(splinePoints);
	spline.speed = 40.0f;
	splineFollowersManager_.SetComponent(cameraSplineFollower_, spline);

	SplineStates splineStates;
	for (size_t i = 0; i < kCameraSplineRotation_.size(); i++) {
		PointState point;
		point.pointIndex = i;
		point.rotation = kCameraSplineRotation_[i];
		point.moveSpeed = kCameraSplineMoveSpeed_[i];
		point.lerpSpeed = kCameraSplineLerpSpeed_[i];
		splineStates.pointStates.push_back(point);
	}
	splineStateManager_.SetComponent(cameraSplineFollower_, splineStates);
		
	GameCamera gameCamera;
	gameCamera.playerOffset = playerPosOffset_;
	ecsManager_.GetComponentsManager<GameCameraManager>().SetComponent(cameraSplineFollower_, gameCamera);
	
	ecsManager_.AddComponent(cameraSplineFollower_,
		ecs::ComponentType::TRANSFORM | 
		ecs::ComponentType::SPLINE_FOLLOWER |
		ecs::ComponentType::SPLINE_STATES |
		ecs::ComponentType::GAME_CAMERA);
	
	camera_.SetViewMatrix(math::Matrix4::LookAt(kCameraBasePosition_, kCameraBaseDirection_));
	camera_.SetPosition(kCameraBasePosition_);
	camera_.SetRotation({ 0.0f, 180.0f * math::kDeg2Rad, 0.0f });
	camera_.Update();

	camera_.SetPosition(kCameraBasePosition_);
	camera_.SetRotation(kCameraBaseDirection_);
	camera_.SetViewMatrix(math::Matrix4::LookAt(kCameraBasePosition_, camera_.GetFront() * -1.0f));
	camera_.Update();
		
	// Spawn player
	playerIndex_ = gamePrefabsManager_.InstantiatePlayer();
	transformsManager_.SetParent(playerIndex_[0], cameraSplineFollower_);
	transform = transformsManager_.GetComponent(playerIndex_[0]);
	transform.SetLocalPosition(playerPosOffset_);
	transformsManager_.SetComponent(playerIndex_[0], transform);

	ecsManager_.RemoveComponent(playerIndex_[0], ecs::ComponentType::GAME_CAMERA);
		
	Player player = ecsManager_.GetComponentsManager<PlayerManager>().GetComponent(playerIndex_[1]);
	player.movementArea = { 60.0f, 30.0f };
	ecsManager_.GetComponentsManager<PlayerManager>().SetComponent(playerIndex_[1], player);
	
    //Initialize enemies
    for(size_t index = 0; index < kEnemySplines_.size(); index++) {
		ecs::EntityIndex enemyIndex = gamePrefabsManager_.InstantiateLightFighter()[0];

		ecs::SplineFollower splineFollower = splineFollowersManager_.GetComponent(enemyIndex);

		splineFollower.speed = kEnemyApproachSpeed_;
		
		for (size_t i = 0; i < kEnemySplines_[0].size(); i++) {
			splineFollower.spline.AddPoint(kEnemySplines_[index][i]);
		}

		math::Transform transform = transformsManager_.GetComponent(enemyIndex);
		transform.SetLocalPosition(kEnemySplines_[index][1]);
		transformsManager_.SetComponent(enemyIndex, transform);
    	
		splineFollowersManager_.SetComponent(enemyIndex, splineFollower);
		ecsManager_.AddComponent(enemyIndex, ecs::ComponentType::SPLINE_FOLLOWER);

		Enemy enemy = enemiesManager_.GetComponent(enemyIndex);
		enemy.state = Enemy::State::LAUNCH;
		enemy.splineFireStartPoint = kEnemyStartFireAt_;
		enemy.splineFireEndPoint = kEnemyStartFleeAt_;
		enemy.approachSpeed = kEnemyApproachSpeed_;
		enemy.attackSpeed = kEnemyAttakSpeed_;
		enemy.fleeSpeed = kEnemyFleeSpeed_;
		enemiesManager_.SetComponent(enemyIndex, enemy);
    }

	// Instantiate destroyer
	for (size_t index = 0; index < kDestroyerSplines_.size(); index++) {
		ecs::EntityIndex enemyIndex = gamePrefabsManager_.InstantiateDestroyer()[0];

		ecs::SplineFollower splineFollower = splineFollowersManager_.GetComponent(enemyIndex);

		splineFollower.speed = kDestroyerApproachSpeed_;

		for (size_t i = 0; i < kDestroyerSplines_[0].size(); i++) {
			splineFollower.spline.AddPoint(kDestroyerSplines_[index][i]);
		}

		math::Transform transform = transformsManager_.GetComponent(enemyIndex);
		transform.SetLocalPosition(kDestroyerSplines_[index][1]);
		transformsManager_.SetComponent(enemyIndex, transform);

		splineFollowersManager_.SetComponent(enemyIndex, splineFollower);
		ecsManager_.AddComponent(enemyIndex, ecs::ComponentType::SPLINE_FOLLOWER);

		Enemy enemy = enemiesManager_.GetComponent(enemyIndex);
		enemy.state = Enemy::State::LAUNCH;
		enemy.splineFireStartPoint = kDestroyerStartFireAt_;
		enemy.splineFireEndPoint = kDestroyerStartFleeAt_;
		enemy.approachSpeed = kDestroyerApproachSpeed_;
		enemy.attackSpeed = kDestroyerAttakSpeed_;
		enemy.fleeSpeed = kDestroyerFleeSpeed_;
		enemiesManager_.SetComponent(enemyIndex, enemy);
	}

	CreateBoxes();
	GenerateAsteroids();
		
	if(isStraightLine) {
		CreateBoxes();
		GenerateAsteroids();
	}
}

void DemoSystem::CreateBoxes() {
	math::Vec3 minPosition = kAsteroidSpawnMinPos_;
	math::Vec3 maxPosition = kAsteroidSpawnMaxPos_;

	float xSize = maxPosition.x - minPosition.x;
	float ySize = maxPosition.y - minPosition.y;
	float zSize = maxPosition.z - minPosition.z;

	int xBoxQuantity = static_cast<int>(xSize / kAsteroidBoxSize.x);
	int yBoxQuantity = static_cast<int>(ySize / kAsteroidBoxSize.y);
	int zBoxQuantity = static_cast<int>(zSize / kAsteroidBoxSize.z);

	for (int z = 0; z < zBoxQuantity; z++)
		for (int y = 0; y < yBoxQuantity; y++)
			for (int x = 0; x < xBoxQuantity; x++)
			{
				math::Vec3 newBox{ (x * kAsteroidBoxSize.x) + (kAsteroidBoxSize.x / 2),
								   (y * kAsteroidBoxSize.y) + (kAsteroidBoxSize.y / 2),
								   (z * kAsteroidBoxSize.z) + (kAsteroidBoxSize.z / 2) };
				fields.emplace_back(newBox);
			}
}

void DemoSystem::GenerateAsteroids() {
	math::Vec3 halfBoxSize = math::Vec3::Divide(kAsteroidBoxSize, { 2.0f });
	for (math::Vec3 field : fields) {
		for (int j = 0; j < kAsteroidPerBox; j++)
		{
			const int rndOffsetX = rand() / (RAND_MAX / halfBoxSize.x);
			const int rndOffsetY = rand() / (RAND_MAX / halfBoxSize.y);
			const int rndOffsetZ = rand() / (RAND_MAX / halfBoxSize.z);
			math::Vec3 randomPosOffset = math::Vec3(rndOffsetX, rndOffsetY, rndOffsetZ);

			const int rndRotationX = rand() / (RAND_MAX / 360);
			const int rndRotationY = rand() / (RAND_MAX / 360);
			const int rndRotationZ = rand() / (RAND_MAX / 360);
			math::Vec3 randomRotation = math::Vec3(rndRotationX, rndRotationY, rndRotationZ) * math::kDeg2Rad;
			
			const math::Vec3 rndScaleFactor((static_cast<float>(rand()) / (static_cast<float>(RAND_MAX) / 100.0f)) / 100.0f);
			
			const int invertPos = rand() / (RAND_MAX / 2);
			if (invertPos == 0)
				randomPosOffset = math::Vec3::Multiply(randomPosOffset, { -1.0f });
			
			const int rndPrefabID = rand() / (RAND_MAX / (asteroidPrefabs_.size() - 1));

			std::vector<ecs::EntityIndex> prefabIndex = gamePrefabsManager_.Instantiate(asteroidPrefabs_[rndPrefabID]);
			math::Transform transform = transformsManager_.GetComponent(prefabIndex[0]);
			transform.SetLocalPosition(field + randomPosOffset);
			transform.SetLocalRotation(randomRotation);
			transform.SetLocalScale(transform.GetLocalScale() + math::Vec3::Multiply(transform.GetLocalScale(), rndScaleFactor));
			transformsManager_.SetComponent(prefabIndex[0], transform);

			Jiggle jiggle;
			jiggle.maxTime = 0.1f;
			jiggle.rotationSpeed = 0.01f;
			jiggle.speed = 0.1f;
			jiggleManager_.SetComponent(prefabIndex[0], jiggle);

			ecsManager_.AddComponent(prefabIndex[0], ecs::ComponentType::JIGGLE);
		}
	}
}
} // namespace poke::game