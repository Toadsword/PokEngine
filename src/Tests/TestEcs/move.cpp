#include <Tests/TestEcs/move.h>

#include <Ecs/core_ecs_manager.h>
#include <Utility/log.h>

#include <CoreEngine/engine.h>

namespace poke {
Move::Move(Engine& engine)
    : System(engine)
{

	engine_.AddObserver(observer::MainLoopSubject::UPDATE, [this]() {OnUpdate(); });

    entities_.reserve(ecs::kEntityBaseSize);
}

void Move::OnUpdate() { }
} //namespace poke::ecs
