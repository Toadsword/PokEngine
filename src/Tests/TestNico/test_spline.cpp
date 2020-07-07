#include <Tests/TestNico/test_spline.h>
#include <CoreEngine/engine.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>

namespace poke {

TestSpline::TestSpline(Engine& engine) : System(engine)
{
	ObserveLoadScene();
	engine_.AddObserver(observer::MainLoopSubject::UPDATE, [this]() {OnUpdate(); });
}
void TestSpline::OnAppBuild()
{
	
}

void TestSpline::OnLoadScene()
{
	auto& ecsManager = EcsManagerLocator::Get();

	auto& splinesManager = ecsManager.GetComponentsManager<poke::ecs::SplineFollowersManager>();

    //Entity 0
	std::vector<math::Vec3> points{
		math::Vec3(0, 0, 0),
		math::Vec3(1, 0, 0),
		math::Vec3(2, 0, 0),
		math::Vec3(3, 0, 0),
		math::Vec3(4, 0, 0),
		math::Vec3(10, 0, 0),
		math::Vec3(20, 0, 0),
		math::Vec3(50, 0, 0),
	};
	ecs::SplineFollower spline(points);
	spline.speed = 1;
	entity0_ = ecsManager.AddEntity();
	splinesManager.SetComponent(entity0_, spline);
	ecsManager.AddComponent(entity0_, ecs::ComponentType::SPLINE_FOLLOWER);
	ecsManager.AddComponent(entity0_, ecs::ComponentType::TRANSFORM);

    //Entity 1
	points = std::vector<math::Vec3>{
		math::Vec3(0, 1, 0),
		math::Vec3(1, 1, 0),
		math::Vec3(2, 1, 0),
		math::Vec3(3, 1, 0),
		math::Vec3(4, 1, 0),
		math::Vec3(10, 1, 0),
		math::Vec3(20, 1, 0),
		math::Vec3(50, 1, 0),
	};
	spline = ecs::SplineFollower(points);
	entity1_ = ecsManager.AddEntity();
	splinesManager.SetComponent(entity1_, spline);
	ecsManager.AddComponent(entity1_, ecs::ComponentType::SPLINE_FOLLOWER);
	ecsManager.AddComponent(entity1_, ecs::ComponentType::TRANSFORM);
}

void TestSpline::OnUpdate()
{
	auto& ecsManager = EcsManagerLocator::Get();

	auto& splinesManager = ecsManager.GetComponentsManager<poke::ecs::SplineFollowersManager>();

	auto splineFollower = splinesManager.GetComponent(entity1_);

    if(splineFollower.lastPoint > 4) {
		splineFollower.speed = 0.5f;
    }else {
		splineFollower.speed = 1.5f;
    }

	splinesManager.SetComponent(entity1_, splineFollower);
}
}
