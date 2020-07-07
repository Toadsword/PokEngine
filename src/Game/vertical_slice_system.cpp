#include <Utility/log.h>
#include <CoreEngine/engine.h>
#include <Utility/profiler.h>
#include <Utility/time_custom.h>
#include <Game/ComponentManagers/player_manager.h>
#include <Game/Ecs/game_archetype.h>
#include <Game/game.h>
#include <Game/demo_system.h>
#include <Game/vertical_slice_system.h>

namespace poke::game {
	VerticalSliceSystem::VerticalSliceSystem(Engine& engine, Game& game)
		: GameSystem(engine, game),
		time_(Time::Get()),
		camera_(*game.GetCameraPtr()),
		playerManager_(ecsManager_.GetComponentsManager<PlayerManager>()),
		splineFollowersManager_(ecsManager_.GetComponentsManager<ecs::SplineFollowersManager>()),
		gamePrefabsManager_(reinterpret_cast<GamePrefabsManager&>(PrefabsManagerLocator::Get())),
		splineStateManager_(ecsManager_.GetComponentsManager<poke::game::SplineStateManager>()),
		transformManager_(ecsManager_.GetComponentsManager<ecs::TransformsManager>()) {
    ObserveLoadScene();
	game.RegisterObserverUpdate([this] { this->OnUpdate(); });
}


void VerticalSliceSystem::OnLoadScene() {
	for (size_t i = 0; i < asteroidPrefabs_.size(); i++)
		gamePrefabsManager_.AddPrefab(asteroidPrefabs_[i]);
	
	// Check if the scene is the vertical slice
	if (SceneManagerLocator::Get().GetActiveScene().GetSceneName() == "VerticalSlice")
		InitializeScene();
	else
		systemIsRunning_ = false;
}

void VerticalSliceSystem::OnUpdate() {
	if (!systemIsRunning_) { return; }
	const Player player = playerManager_.GetComponent(playerIndex);
		
	if(player.life <= 0) {
		ecs::SplineFollower spline = splineFollowersManager_.GetComponent(splineIndex);
		spline.speed = 0.0f;
		splineFollowersManager_.SetComponent(splineIndex, spline);

		currentTimeBeforeRestarting += time_.deltaTime.count();

		if (currentTimeBeforeRestarting >= kTimeBeforeRestarting_) {
			SceneManagerLocator::Get().LoadScene(SceneManagerLocator::Get().GetActiveSceneIndex());
			currentTimeBeforeRestarting = 0;
		}
	}	
}
	
void VerticalSliceSystem::InitializeScene() {
	//TODO (@C�dric) : Get player and camera spline index
	splineIndex = 1;
	playerIndex = 2;

	std::vector<math::Vec3> splinePoints(kCameraSplinePoints_);

	SplineStates splineStates;
	splineStates.pointStates.resize(kCameraSplineRotation_.size());
	for (size_t i = 0; i < kCameraSplineRotation_.size(); i++) {
		PointState point;
		point.pointIndex = i;
		point.rotation = kCameraSplineRotation_[i];
		point.moveSpeed = kCameraSplineMoveSpeed_[i];
		point.lerpSpeed = kCameraSplineLerpSpeed_[i];
		splineStates.pointStates[i] = point;
	}
	splineStateManager_.SetComponent(splineIndex, splineStates);
		
	ecs::SplineFollower spline;
	spline.spline = splinePoints;
	spline.speed = 0.2f;
	splineFollowersManager_.SetComponent(splineIndex, spline);
	ecsManager_.AddComponent(splineIndex, ecs::ComponentType::SPLINE_FOLLOWER | ecs::ComponentType::SPLINE_STATES);

	Player player = ecsManager_.GetComponentsManager<PlayerManager>().GetComponent(playerIndex);
	player.movementArea = { 60.0f, 30.0f };
	ecsManager_.GetComponentsManager<PlayerManager>().SetComponent(playerIndex, player);
		
	systemIsRunning_ = true;
}
} // namespace poke::game