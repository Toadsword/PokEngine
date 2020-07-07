#include <CoreEngine/core_systems_container.h>
#include <CoreEngine/engine.h>

namespace poke {
CoreSystemsContainer::CoreSystemsContainer(Engine& engine) : engine_(engine) {
    engine.AddObserver(
        observer::MainLoopSubject::ENGINE_INIT,
        [this]() {this->OnEngineInit();}
    );
}

void CoreSystemsContainer::OnEngineInit() {
	splineFollower.emplace(engine_);
	drawSystem.emplace(engine_);
	physicsSystem.emplace(engine_);
	chunksSystem.emplace(engine_);
	audioSystem.emplace(engine_);
	lightSystem.emplace(engine_);
	particlesSystem.emplace(engine_);
	trailRendererSystem.emplace(engine_);
	segmentRendererSystem.emplace(engine_);
}
}//namespace poke