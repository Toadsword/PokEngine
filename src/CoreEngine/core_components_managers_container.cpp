#include <CoreEngine/core_components_managers_container.h>
 
#include <CoreEngine/ServiceLocator/service_locator_definition.h>
#include <Ecs/ComponentManagers/trail_renderer_manager.h>
#include <Ecs/ComponentManagers/segment_renderer_manager.h>

namespace poke {
namespace ecs {
void CoreComponentsManagersContainer::Init() {
	auto& ecsManager = EcsManagerLocator::Get();
	ecsManager.RegisterManager(TransformsManager());
	ecsManager.RegisterManager(RigidbodyManager());
	ecsManager.RegisterManager(CollidersManager());
	ecsManager.RegisterManager(ModelsManager());
	ecsManager.RegisterManager(SplineFollowersManager());
	ecsManager.RegisterManager(LightsManager());
	ecsManager.RegisterManager(AudioSourcesManager());
	ecsManager.RegisterManager(ParticleSystemsManager());
	ecsManager.RegisterManager(TrailRendererManager());
	ecsManager.RegisterManager(SegmentRendererManager());
}
}//namespace ecs
}//namespace poke