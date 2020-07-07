#include <ResourcesManager/resources_manager_container.h>

#include <CoreEngine/engine.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>

namespace poke {

ResourcesManagerContainer::ResourcesManagerContainer(Engine& engine)
{
    
}

void ResourcesManagerContainer::Init()
{
	SceneManagerLocator::Get().AddOnUnloadObserver([this]() {OnUnloadScene(); });

    meshManager.Init();
}
void ResourcesManagerContainer::OnUnloadScene()
{
	meshManager.Clear();
	materialsManager.Clear();
	textureManager.Clear();
}
} //namespace poke
