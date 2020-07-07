#include <Ecs/system.h>

#include <Ecs/core_ecs_manager.h>
#include <CoreEngine/engine.h>

namespace poke::ecs {
System::System(Engine& engine)
    : Module(engine),
      ecsManager_(EcsManagerLocator::Get()) { }

//-------------------- Observer Part ------------------------
void System::ObserveEntityInit()
{
    ecsManager_.AddObserver(
        observer::EntitiesSubjects::INIT,
        [this](const EntityIndex entityIndex) { this->OnEntityInit(entityIndex); }
    );
}

void System::ObserveEntityDestroy()
{
    ecsManager_.AddObserver(
        observer::EntitiesSubjects::DESTROY,
        [this](const EntityIndex entityIndex) { this->OnEntityDestroy(entityIndex); }
    );
}

void System::ObserveEntitySetActive()
{
    ecsManager_.AddObserver(
        observer::EntitiesSubjects::SET_ACTIVE,
        [this](const EntityIndex entityIndex) { this->OnEntitySetActive(entityIndex); }
    );
}

void System::ObserveEntitySetInactive()
{
    ecsManager_.AddObserver(
        observer::EntitiesSubjects::SET_INACTIVE,
        [this](const EntityIndex entityIndex) { this->OnEntitySetInactive(entityIndex); }
    );
}

void System::ObserveEntityAddComponent()
{
    ecsManager_.RegisterObserverAddComponent(
        [this](const EntityIndex entityIndex, const ComponentMask component) {
            this->OnEntityAddComponent(entityIndex, component);
        }
    );
}

void System::ObserveEntityRemoveComponent()
{
    ecsManager_.RegisterObserverRemoveComponent(
        [this](const EntityIndex entityIndex, const ComponentMask component) {
            this->OnEntityRemoveComponent(entityIndex, component);
        }
    );
}
} // namespace poke::ecs
