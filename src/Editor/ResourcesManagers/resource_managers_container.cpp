#include <Editor/ResourcesManagers/resource_managers_container.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>

namespace poke {
namespace editor {

ResourcesManagerContainer::ResourcesManagerContainer() {}
void ResourcesManagerContainer::Init()
{
	SceneManagerLocator::Get().AddOnUnloadObserver([this]() {OnUnloadScene(); });
	editorMeshesManager.Init();
}

void ResourcesManagerContainer::OnUnloadScene()
{
	editorMaterialsManager.Clear();
	editorSoundsManager.Clear();
	editorMeshesManager.Clear();
	editorTexturesManager.Clear();
	editorPrefabsManager.Clear();

	editorMaterialsManager.SetBaseMaterialNames();
}
} //namespace editor
} //namespace poke