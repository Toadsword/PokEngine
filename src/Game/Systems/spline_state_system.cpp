#include <Game/Systems/spline_state_system.h>
#include <CoreEngine/engine.h>
#include <Utility/time_custom.h>
#include <Game/game.h>
#include <Utility/log.h>
#include <Utility/profiler.h>

namespace poke {
namespace game {
SplineStateSystem::SplineStateSystem(Engine& engine, Game& game) :
	GameSystem(engine, game),
	splineStatesManager_(ecsManager_.GetComponentsManager<SplineStateManager>()),
    transformsManager_(ecsManager_.GetComponentsManager<ecs::TransformsManager>()),
    splineFollowersManager_(ecsManager_.GetComponentsManager<ecs::SplineFollowersManager>()),
    gameCameraManager_(ecsManager_.GetComponentsManager<GameCameraManager>())
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
}

void SplineStateSystem::OnUpdate() {
	pok_BeginProfiling(SplineState_System, 0);
	const float dt = static_cast<float>(Time::Get().deltaTime.count() / 1000.0);

	GameCamera gameCamera;
    if(cameraIndex_ != ecs::kNoEntity) {
		gameCamera = gameCameraManager_.GetComponent(cameraIndex_);
    }

    for(ecs::EntityIndex entityIndex : entityIndexes_) {
		SplineStates splineStates = splineStatesManager_.GetComponent(entityIndex);
		ecs::SplineFollower splineFollower = splineFollowersManager_.GetComponent(entityIndex);

		const PointState newPointState = splineStates.pointStates[splineFollower.lastPoint + 1];
		const PointState oldPointState = splineStates.pointStates[splineFollower.lastPoint];
    	
		splineFollower.speed = oldPointState.moveSpeed + (newPointState.moveSpeed - oldPointState.moveSpeed) * splineFollower.segmentPercentage;

		gameCamera.Up = oldPointState.rotation + (newPointState.rotation - oldPointState.rotation) * splineFollower.segmentPercentage;

    	
		//// Check if it need to go to the next point id
		//if (splineFollower.speed - newPointState.moveSpeed <= kSpeedApproximationTolerance_) {
		//	splineFollower.speed = newPointState.moveSpeed;
		//	splineStates.currentPointState++;
		//	if (splineStates.currentPointState >= splineFollower.spline.GetSize() - 3) {
		//		splineStates.currentPointState = 1;
		//	}
		//}

		splineFollowersManager_.SetComponent(entityIndex, splineFollower);
    }

    if(cameraIndex_ != ecs::kNoEntity) {
		gameCameraManager_.SetComponent(cameraIndex_, gameCamera);  
    }
	pok_EndProfiling(SplineState_System);
}

void SplineStateSystem::OnEntitySetActive(const ecs::EntityIndex entityIndex) {
	//Check if the activated entity is a missile
	if (ecsManager_.HasComponent(entityIndex, ecs::ComponentType::SPLINE_STATES)) { 
        if(!entityIndexes_.exist(entityIndex)) {
	        //Add the entity to the list of entities and sort it
	        entityIndexes_.insert(entityIndex);
        }
	}

    if(cameraIndex_ == ecs::kNoEntity &&
		ecsManager_.HasComponent(entityIndex, ecs::ComponentType::GAME_CAMERA)) {
		cameraIndex_ = entityIndex;
    }
}

void SplineStateSystem::OnEntitySetInactive(const ecs::EntityIndex entityIndex) {
	//Check if the entity is in the list of entities
	if (entityIndexes_.exist(entityIndex)) {
	    //remove the deactivated entity for the list of entities
	    const auto it = entityIndexes_.find(entityIndex);
	    entityIndexes_.erase(it);
	}
	if (cameraIndex_ != ecs::kNoEntity &&
		ecsManager_.HasComponent(entityIndex, ecs::ComponentType::GAME_CAMERA)) {
		cameraIndex_ = ecs::kNoEntity;
	}
}

void SplineStateSystem::OnEntityAddComponent(const ecs::EntityIndex entityIndex, const ecs::ComponentMask componentMask) {
	//Check if the entity have the component required
	if ((componentMask & ecs::ComponentType::SPLINE_STATES) == ecs::ComponentType::SPLINE_STATES &&
		ecsManager_.HasComponent(entityIndex, ecs::EntityFlag::IS_ACTIVE) &&
		!entityIndexes_.exist(entityIndex))
	{
        //Add the entity to the list of entities and sort it
        entityIndexes_.insert(entityIndex);
	}
	if (cameraIndex_ == ecs::kNoEntity &&
		(componentMask & ecs::ComponentType::SPLINE_STATES) == ecs::ComponentType::SPLINE_STATES &&
		ecsManager_.HasComponent(entityIndex, ecs::EntityFlag::IS_ACTIVE)) {
		cameraIndex_ = entityIndex;
	}
}

void SplineStateSystem::OnEntityRemoveComponent(const ecs::EntityIndex entityIndex, const ecs::ComponentMask componentMask) {
	if ((componentMask & ecs::ComponentType::SPLINE_STATES) == ecs::ComponentType::SPLINE_STATES &&
		entityIndexes_.exist(entityIndex))
	{
	    //remove if one of the removed components
	    const auto it = entityIndexes_.find(entityIndex);
	    entityIndexes_.erase(it);
	}

	if (cameraIndex_ == ecs::kNoEntity &&
		(componentMask & ecs::ComponentType::SPLINE_STATES) == ecs::ComponentType::SPLINE_STATES) {
		cameraIndex_ = ecs::kNoEntity;
	}
}
}//namespace game
}//namespace poke