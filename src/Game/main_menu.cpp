#include <Game/main_menu.h>
#include <Utility/log.h>
#include <CoreEngine/engine.h>
#include <Utility/profiler.h>
#include <Utility/time_custom.h>
#include <Game/ComponentManagers/player_manager.h>
#include <Game/ComponentManagers/weapon_manager.h>
#include <Game/ComponentManagers/destructible_element_manager.h>
#include <Game/Ecs/game_archetype.h>
#include <Game/game.h>

namespace poke::game {
MainMenu::MainMenu(Engine& engine, Game& game)
	: GameSystem(engine, game),
	camera_(*game.GetCameraPtr()),
	time_(Time::Get()),
	playerManager_(EcsManagerLocator::Get().GetComponentsManager<PlayerManager>()),
	weaponManager_(EcsManagerLocator::Get().GetComponentsManager<WeaponManager>()),
	transformsManager_(EcsManagerLocator::Get().GetComponentsManager<ecs::TransformsManager>()),
	rigidbodyManager_(EcsManagerLocator::Get().GetComponentsManager<ecs::RigidbodyManager>()),
	modelsManager_(EcsManagerLocator::Get().GetComponentsManager<ecs::ModelsManager>()),
	collidersManager_(EcsManagerLocator::Get().GetComponentsManager<ecs::CollidersManager>()),
	destructibleElementManager_(EcsManagerLocator::Get().GetComponentsManager<DestructibleElementManager>()),
	gamePrefabsManager_(static_cast<GamePrefabsManager&>(PrefabsManagerLocator::Get())),
	gizmoCommandBuffer_(GraphicsEngineLocator::Get().GetGizmoCommandBuffer()) {

    ObserveLoadScene();
}

void MainMenu::OnLoadScene() {
	//gamePrefabsManager_.AddPrefab("Player");
	//gamePrefabsManager_.AddPrefab("ProjectilePlayer");
	//for (size_t i = 0; i < asteroidPrefabs_.size(); i++)
	//	gamePrefabsManager_.AddPrefab(asteroidPrefabs_[i]);
	
	// Check if the scene is the main menu
	if (SceneManagerLocator::Get().GetActiveScene().GetSceneName() == "MainMenu") {
		// TODO (@Cédric) : Remove when the prefabs are loaded elsewhere
		gamePrefabsManager_.AddPrefab("Player");
		gamePrefabsManager_.AddPrefab("ProjectilePlayer");
		gamePrefabsManager_.AddPrefab("MissilePlayer");

		for (std::string prefabName : asteroidPrefabs_)
			gamePrefabsManager_.AddPrefab(prefabName);

		SetupCamera();
		InitAsteroidePool();
		game_.RegisterObserverUpdate([this] { this->OnUpdate(); });
		systemIsRunning_ = true;
	}
	else
		systemIsRunning_ = false;
}

void MainMenu::OnUpdate() {
	pok_BeginProfiling(Main_Menu_System, 0);
	if (!systemIsRunning_) {
		pok_EndProfiling(Main_Menu_System);
		return;
	}
	
	if (time_.GetTime() >= lastSpawn_ + spawnDelay_)
		InstantiateAsteroide();

	// Check if the game has to start
	if (!ecsManager_.HasComponent(asteroidPool_[0], ecs::EntityFlag::IS_ACTIVE))
		LoadLevel();

	// Check if the player want to open the options
	if (!ecsManager_.HasComponent(asteroidPool_[1], ecs::EntityFlag::IS_ACTIVE))
		ShowOptions();

	// Check if the game has to start
	if (!ecsManager_.HasComponent(asteroidPool_[2], ecs::EntityFlag::IS_ACTIVE))
		ShowCredits();	
	pok_EndProfiling(Main_Menu_System);
}

void MainMenu::SetupCamera() {
	//camera_.SetView(math::Matrix4::LookAt(kCameraBasePosition_, kCameraBaseDirection_));
}

void MainMenu::InitAsteroidePool() {
	// Instantiate the player
	ecs::EntityIndex playerIndex = gamePrefabsManager_.InstantiatePlayer()[0];

	// Move the player in the front of the camera
	math::Transform transform = transformsManager_.GetComponent(playerIndex);
	transform.SetLocalPosition(kCameraBasePosition_ + playerPosOffset_);
	transformsManager_.SetComponent(playerIndex, transform);

    math::Vec3 asteroidePos = kAsteroidSpawnMinPos_;
	asteroidePos.z = kInteractibleAsteroidOffset_.z;
	asteroidePos.y = kInteractibleAsteroidOffset_.y;

	asteroidPool_.reserve(kAsteroidPoolSize_);
	std::vector<ecs::EntityIndex> prefabIndex;
	for(size_t i = 0; i < kAsteroidPoolSize_; i++)	{
		const int rndPrefabID = rand() / (RAND_MAX / asteroidPrefabs_.size());

		prefabIndex = gamePrefabsManager_.Instantiate(asteroidPrefabs_[rndPrefabID]);

		asteroidPool_.emplace_back(prefabIndex[0]);
		
		asteroidePos.x += kInteractibleAsteroidOffset_.x;
		transform.SetLocalPosition(asteroidePos);
		transformsManager_.SetComponent(asteroidPool_[i], transform);
	}
}

void MainMenu::InstantiateAsteroide() {
	std::vector<ecs::EntityIndex> prefabIndex;

	const int rndPrefabID = rand() / (RAND_MAX / asteroidPrefabs_.size());

	prefabIndex = gamePrefabsManager_.Instantiate(asteroidPrefabs_[rndPrefabID]);
	
	math::Vec3 pos = kAsteroidSpawnMinPos_;
	pos.y = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / kAsteroidSpawnMaxPos_.y));
	
	math::Transform transform = transformsManager_.GetComponent(prefabIndex[0]);
	transform.SetLocalPosition(pos);
	transformsManager_.SetComponent(prefabIndex[0], transform);

	physics::Rigidbody rigid = rigidbodyManager_.GetComponent(prefabIndex[0]);
	rigid.linearVelocity = { static_cast <float> ((rand()) / (static_cast <float> (RAND_MAX / kAsteroidMaxSpeed_))) + kAsteroidMinSpeed_,
		0.0f, 0.0f };
	rigidbodyManager_.SetComponent(prefabIndex[0], rigid);

	ecsManager_.SetActive(prefabIndex[0], ecs::EntityStatus::ACTIVE);
	
	spawnDelay_ = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / kMaxSpawnDelay_));
	lastSpawn_ = time_.GetTime();
}
	
void MainMenu::LoadLevel() {
	SceneManagerLocator::Get().LoadScene(levelName_);
}

void MainMenu::ShowOptions() { }
	
void MainMenu::ShowCredits() { }
} // namespace poke::game