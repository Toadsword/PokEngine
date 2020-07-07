#include <Game/prefabs_container.h>

#include <CoreEngine/ServiceLocator/service_locator_definition.h>

namespace poke {
namespace game {
void PrefabsContainer::Init() {
	IPrefabsManager& prefabsManager = PrefabsManagerLocator::Get();

	//prefabsManager.AddPrefab("LightFighter");
	//prefabsManager.AddPrefab("Player");
}
}//namespace game
}//namespace poke
