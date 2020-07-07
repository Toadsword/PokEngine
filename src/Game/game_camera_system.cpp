#include <Game/game_camera_system.h>
#include <Game/game.h>
#include <Utility/profiler.h>

namespace poke::game {
	GameCameraSystem::GameCameraSystem(Engine& engine, Game& game)
		: GameSystem(engine, game),
		gameCamera_(*game.GetCameraPtr()),
		transformsManager_(ecsManager_.GetComponentsManager<ecs::TransformsManager>()),
		splineFollowersManager_(ecsManager_.GetComponentsManager<ecs::SplineFollowersManager>()),
		gameCameraManager_(ecsManager_.GetComponentsManager<GameCameraManager>()),
		gizmoCommandBuffer_(GraphicsEngineLocator::Get().GetGizmoCommandBuffer()) {

		ecsManager_.RegisterObserverAddComponent(
			[this](ecs::EntityIndex entityIndex, ecs::ComponentMask component)
		{OnAddComponent(entityIndex, component); }
		);
		ecsManager_.RegisterObserverRemoveComponent(
			[this](ecs::EntityIndex entityIndex, ecs::ComponentMask component)
		{OnRemoveComponent(entityIndex, component); }
		);
		
		game.RegisterObserverUpdate([this] { this->OnUpdate(); });
	}

	void GameCameraSystem::OnUpdate() {
		pok_BeginProfiling(Game_Camera_System, 0);
		for (const ecs::EntityIndex entityIndex : entityIndexes_) {
			if (!ecsManager_.HasComponent(entityIndex, ecs::ComponentType::SPLINE_FOLLOWER))
				continue;

			gameCameraData_ = gameCameraManager_.GetComponent(entityIndex);
			
			// Set the position of the "Camera Entity" with the current spline position
			const ecs::SplineFollower splineFollower = splineFollowersManager_.GetComponent(entityIndex);
			math::Transform transform = transformsManager_.GetComponent(entityIndex);
			transform.SetLocalPosition(splineFollower.spline.Lerp(splineFollower.lastPoint, splineFollower.segmentPercentage));
		
			// Set the position of the camera
			gameCamera_.SetPosition(transformsManager_.GetWorldPosition(entityIndex));

			size_t lastpoint = splineFollower.lastPoint;
			float percentage = splineFollower.segmentPercentage;
			
			const size_t length = splineFollower.spline.GetPoints().size();
			if (lastpoint == length - 2 && percentage > kMaxSplinePercentage_) {
				lastpoint++;
				percentage = 0.0f;
			}
			else {
				percentage += kAugmentationPercentage_;
				if (percentage > 1.0f) {
					percentage--;
					lastpoint++;
				}
			}

			gameCamera_.SetViewMatrix(
				math::Matrix4::LookAt(transform.GetLocalPosition(), splineFollower.spline.Lerp(lastpoint, percentage), gameCameraData_.Up));


			transform.SetLocalRotation(
				math::Quaternion::LookRotation(
				(splineFollower.spline.Lerp(lastpoint, percentage) - transform.GetLocalPosition()).Normalize(),
					gameCameraData_.Up
				).GetEulerAngles()
			);

			transformsManager_.SetComponent(entityIndex, transform);

			math::Matrix4 worldMatrix = transformsManager_.GetComponent(entityIndex).GetWorldMatrix();

   //         //Debug gizmos show front and up
			//gizmoCommandBuffer_.SetColor(Color::white);
			//gizmoCommandBuffer_.DrawWireSphere(
			//	transform.GetLocalPosition() +
			//	gameCamera_.GetFront() * -1.0f * 10,
			//	2.0f);


			//gizmoCommandBuffer_.DrawWireSphere(
			//	transform.GetLocalPosition() +
			//	gameCamera_.GetFront() * -1.0f * 10 + gameCamera_.GetUp() * 4.0f,
			//	1.0f);

			//gizmoCommandBuffer_.DrawWireSphere(
			//	transform.GetLocalPosition() +
			//	gameCamera_.GetFront() * -1.0f * 10 + gameCamera_.GetRight() * 4.0f,
			//	1.0f);

			// Update the chunks
		}	
		pok_EndProfiling(Game_Camera_System);
	}

	void GameCameraSystem::OnAddComponent(ecs::EntityIndex entityIndex, ecs::ComponentMask component) {
		//Check if the entity have the component required
		if ((component & ecs::ComponentType::GAME_CAMERA) != ecs::ComponentType::GAME_CAMERA) { return; }

		//check if the entity has already been added the the system by the past
		const auto itRegisteredEntities = std::find(registeredEntities_.begin(), registeredEntities_.end(), entityIndex);
		if (itRegisteredEntities == registeredEntities_.end()) {
			//Add the entity to the list of registered entities and sort it
			registeredEntities_.insert(entityIndex);
		}

		//Check if the entity is active
		if (!ecsManager_.HasComponent(entityIndex, ecs::EntityFlag::IS_ACTIVE)) { return; }

		if (entityIndexes_.exist(entityIndex)) { return; } 
		
		// Set the base position and rotation to the camera
		/*gameCamera_.SetPosition(transformsManager_.GetWorldPosition(entityIndex));
		gameCamera_.SetRotation(transformsManager_.GetWorldRotation(entityIndex));
		gameCamera_.SetViewMatrix(math::Matrix4::LookAt(transformsManager_.GetWorldPosition(entityIndex), gameCamera_.GetFront() * -1.0f));
		gameCamera_.Update();*/
		
		//Add the entity to the list of entities and sort it
		entityIndexes_.insert(entityIndex);
	}

	void GameCameraSystem::OnRemoveComponent(ecs::EntityIndex entityIndex, ecs::ComponentMask component) {
		if ((component & ecs::ComponentType::GAME_CAMERA) != ecs::ComponentType::GAME_CAMERA) {
			return;
		}
		//Check if the entity is in the list of entities
		if (!entityIndexes_.exist(entityIndex)) { return; }

		const auto it = std::find(entityIndexes_.begin(), entityIndexes_.end(), entityIndex);

		//remove if one of the removed components
		entityIndexes_.erase(it);
	}

	void GameCameraSystem::OnEntitySetActive(const ecs::EntityIndex entityIndex) {
		//Check if the activated entity is a missile
		if (!ecsManager_.HasComponent(entityIndex, ecs::ComponentType::GAME_CAMERA)) { return; }

		if (entityIndexes_.exist(entityIndex)) { return; }

		//Add the entity to the list of entities and sort it
		entityIndexes_.insert(entityIndex);
	}

	void GameCameraSystem::OnEntitySetInactive(const ecs::EntityIndex entityIndex) {
		//Check if the entity is in the list of entities
		if (!entityIndexes_.exist(entityIndex)) { return; }

		const auto it = std::find(entityIndexes_.begin(), entityIndexes_.end(), entityIndex);

		//remove the deactivated entity for the list of entities
		entityIndexes_.erase(it);
	}
} //namespace poke::game
