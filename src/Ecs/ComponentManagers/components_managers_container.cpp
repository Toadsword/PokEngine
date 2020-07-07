#include <Ecs/ComponentManagers/components_managers_container.h>

#include <CoreEngine/ServiceLocator/service_locator_definition.h>

namespace poke::ecs {
ComponentsManagersContainer::ComponentsManagersContainer() {

}

void ComponentsManagersContainer::Init() {
	CoreComponentsManagersContainer::Init();
}

void ComponentsManagersContainer::ResizeEntities(const size_t size)
{
	currentEntitiesSize_ = size;
    for (auto& componentsManager : componentsManagers_) {
        if(componentsManager) {
            componentsManager->ResizeEntities(size);
        }
    }
}

void ComponentsManagersContainer::ClearEntity(const EntityIndex entityIndex)
{
	for (auto& componentsManager : componentsManagers_) {
		if (componentsManager) {
            componentsManager->ClearEntity(entityIndex);
		}
	}
}


void ComponentsManagersContainer::SetWithArchetype(const EntityPool archetypeData, const Archetype& archetype) {
	for (auto& componentsManager : componentsManagers_) {
		if (componentsManager) {
            componentsManager->SetWithArchetype(archetypeData, archetype);
		}
	}
}

void ComponentsManagersContainer::InsertArchetype(
    const EntityIndex entity,
    const Archetype& archetype)
{
	for (auto& componentsManager : componentsManagers_) {
		if (componentsManager) {
            componentsManager->InsertArchetype(entity, archetype);
		}
	}
}

void ComponentsManagersContainer::EraseArchetype(const EntityPool pool, const size_t nbObjectToErase)
{
    for (auto& componentsManager : componentsManagers_) {
        if(componentsManager) {
            componentsManager->EraseEntities(pool, nbObjectToErase);
        }
    }
}


}//namespace poke::ecs
