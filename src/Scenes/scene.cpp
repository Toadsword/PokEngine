#include <Scenes/scene.h>

#include <Utility/log.h>
#include <CoreEngine/engine.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>

namespace poke {
namespace scene {

Scene::Scene(
    const SceneIndex sceneIndex,
    const std::string& sceneName,
    const std::string& fileName)
    : sceneIndex_(sceneIndex),
      fileName_(fileName),
      name_(sceneName) { }

void Scene::Load()
{
    MaterialsManagerLocator::Get().GetDefaultMaterialID();
    MaterialsManagerLocator::Get().GetDefaultSkyboxMaterialID();
	SoundsManagerLocator::Get().LoadDefaultSound();
    
	// Get managers and .pokscene .
    auto& ecsManager = EcsManagerLocator::Get();

    //Loading

    //If the filename is empty => load a default scene
    if (fileName_.empty()) {
		fileName_ = "Default";
    }

    const json sceneJson = LoadJson(fileName_, FileType::SCENE, FolderType::ROM);

	cassert(!sceneJson.is_null(), "Scene not found");
    //Loading resources
    if (CheckJsonExists(sceneJson, "resources")) {
        auto resourcesJson = sceneJson["resources"];

		if (CheckJsonExists(resourcesJson, "prefabNames")) {
			PrefabsManagerLocator::Get().SetFromJson(resourcesJson["prefabNames"]);
		}

        if(CheckJsonExists(resourcesJson, "meshNames")) {
			MeshManagerLocator::Get().SetFromJson(resourcesJson["meshNames"]);
        }

		if (CheckJsonExists(resourcesJson, "materialNames")) {
			MaterialsManagerLocator::Get().SetFromJson(resourcesJson["materialNames"]);
		}

		if (CheckJsonExists(resourcesJson, "soundNames")) {
			SoundsManagerLocator::Get().SetFromJson(resourcesJson["soundNames"]);
		}
    }

    //Load game objects.
	std::map<ecs::EntityIndex, ecs::EntityIndex> baseParentToOffsetParent;
    if (CheckJsonExists(sceneJson, "gameObjects")) {
		baseParentToOffsetParent = ecsManager.SetEntitiesFromJson(sceneJson["gameObjects"]);
    }

	//Load archetypes.
	if (CheckJsonExists(sceneJson, "archetypes")) {
		ArchetypesManagerLocator::Get().SetFromJson(sceneJson["archetypes"]);
	}

	//Load all chunks
	if (CheckJsonExists(sceneJson, "chunks")) {
		auto& manager = ChunksManagerLocator::Get();
		manager.SetFromJson(sceneJson["chunks"], baseParentToOffsetParent);
	}

	std::cout << "Load Scene\n";
	Time::Get().ResetFrame();
}

json Scene::ToJson() const
{
    json sceneJson;

    //Resources

    //Meshes
	const json meshesJson = MeshManagerLocator::Get().ToJson();
    if(!meshesJson.is_null()) {
		sceneJson["resources"]["meshNames"] = meshesJson;
    }

    //Materials
	const json materialsJson = MaterialsManagerLocator::Get().ToJson();
	if (!materialsJson.is_null()) {
		sceneJson["resources"]["materialNames"] = materialsJson;
	}

    //Sounds
	const json soundsJson = SoundsManagerLocator::Get().ToJson();
	if (!soundsJson.is_null()) {
		sceneJson["resources"]["soundNames"] = soundsJson;
	}

	const json prefabsJson = PrefabsManagerLocator::Get().ToJson();
	if (!prefabsJson.is_null()) {
		sceneJson["resources"]["prefabNames"] = prefabsJson;
	}

    //Save gameObjects
	std::map<ecs::EntityIndex, ecs::EntityIndex> baseEntityIdToOffsetId;
	json gameObjectsJson;
	std::tie(gameObjectsJson, baseEntityIdToOffsetId) = EcsManagerLocator::Get().EntitiesToJson();
	if (!gameObjectsJson.is_null()) {
		sceneJson["gameObjects"] = gameObjectsJson;
	}

	//Save Archetypes
    /*
	const json archetypesJson = ArchetypesManager::Get().ToJson();
	if (!archetypesJson.is_null()) {
		sceneJson["archetypes"] = archetypesJson;
	}
    */
    
	//Save all chunks
	const json chunksJson = ChunksManagerLocator::Get().ToJson(baseEntityIdToOffsetId);
	if (!chunksJson.is_null()) {
		sceneJson["chunks"] = chunksJson;
	}

    return sceneJson;
}

void Scene::UnLoad() {
	LogDebug("Called Scene::UnLoad on : " + name_ + ". This function doesn't do anything.", LogType::SCENE_LOG);
}
} //namespace scene
} //namespace poke
