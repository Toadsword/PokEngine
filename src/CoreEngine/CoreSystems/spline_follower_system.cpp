#include <CoreEngine/CoreSystems/spline_follower_system.h>

#include <CoreEngine/engine.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>
#include <Utility/time_custom.h>
#include <Utility/profiler.h>

namespace poke {
SplineFollowerSystem::SplineFollowerSystem(Engine& engine)
    : System(engine),
      splinesManager_(ecsManager_.GetComponentsManager<ecs::SplineFollowersManager>())
{
    gizmoCommandBuffer_.emplace(GraphicsEngineLocator::Get().GetGizmoCommandBuffer());
	engine_.AddObserver(observer::MainLoopSubject::UPDATE, [this]() {OnUpdate(); });

    ecsManager_.RegisterObserverAddComponent(
        [this](
        const ecs::EntityIndex entityIndex,
        const ecs::ComponentMask component) {
            OnAddComponent(entityIndex, component);
        });

    ecsManager_.RegisterObserverRemoveComponent(
        [this](
        const ecs::EntityIndex entityIndex,
        const ecs::ComponentMask component) {
            OnRemoveComponent(entityIndex, component);
        });

	ObserveEntitySetActive();
	ObserveEntitySetInactive();
}

void SplineFollowerSystem::OnEntitySetActive(const ecs::EntityIndex entityIndex)
{
	if (ecsManager_.HasComponent(entityIndex, ecs::ComponentType::SPLINE_FOLLOWER)) {
		if (std::find(entities_.begin(), entities_.end(), entityIndex) != entities_.end()) return;
		entities_.push_back(entityIndex);
	}
}
void SplineFollowerSystem::OnEntitySetInactive(const ecs::EntityIndex entityIndex)
{
	if (ecsManager_.HasComponent(entityIndex, ecs::ComponentType::SPLINE_FOLLOWER)) {
		const auto it = std::find(entities_.begin(), entities_.end(), entityIndex);

		if (it != entities_.end()) {
			entities_.erase(it);
		}
	}
}

void SplineFollowerSystem::OnUpdate()
{
    pok_BeginProfiling(Spline_System, 0);

    auto dt = Time::Get().deltaTime.count();
    dt /= 1000.0f;

    for(size_t i = 0; i < entities_.size(); i++){
		const auto entity = entities_[i];
        auto splineFollower = splinesManager_.GetComponent(entity);
        auto spline = splineFollower.spline;

        if (spline.GetSize() < kMinSpline_) { break; }

        if (splineFollower.segmentPercentage < 1) {
			const float distance = splineFollower.speed * dt;
            splineFollower.segmentPercentage += distance / spline.GetSegmentLength(splineFollower.lastPoint);
			splineFollower.totalPercentage += distance / spline.GetTotalLength();
        } else {

            splineFollower.segmentPercentage = 0;
            if (splineFollower.lastPoint + kMinSpline_ - 1 < spline.GetSize()) {
                splineFollower.lastPoint++;
            } else {
                splineFollower.lastPoint = 1;
				splineFollower.totalPercentage = 0;
            }
        }

        splinesManager_.SetComponent(entity, splineFollower);
    }
    pok_EndProfiling(Spline_System);
}

void SplineFollowerSystem::OnAddComponent(
    const ecs::EntityIndex entityIndex,
    const ecs::ComponentMask component)
{
    if ((component & ecs::ComponentType::SPLINE_FOLLOWER) == ecs::ComponentType::SPLINE_FOLLOWER &&
		ecsManager_.IsEntityActive(entityIndex)) {
		if (std::find(entities_.begin(), entities_.end(), entityIndex) != entities_.end()) return;
        entities_.push_back(entityIndex);
    }
}

void SplineFollowerSystem::OnRemoveComponent(
    const ecs::EntityIndex entityIndex,
    const ecs::ComponentMask component)
{
    if ((component & ecs::ComponentType::SPLINE_FOLLOWER) == ecs::ComponentType::SPLINE_FOLLOWER) {
        const auto it = std::find(entities_.begin(),entities_.end(),entityIndex);

        if (it != entities_.end()) {
            entities_.erase(it);
        }
    }
}
} //namespace poke
