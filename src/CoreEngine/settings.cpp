#include <CoreEngine/settings.h>
#include <Utility/json_utility.h>
#include "CoreEngine/ServiceLocator/service_locator_definition.h"

namespace poke {
EngineSetting::EngineSetting(
	const std::string& projectName,
    const AppType appType,
    const std::chrono::duration<double, std::milli> deltaTime,
    const int windowWidth,
    const int windowHeight,
    const std::string& windowName,
	const std::vector<SceneSetting>& sceneSettings,
	const std::string& tagFileName,
    const int availableToolFlag,
	const size_t defaultPoolSize)
	: projectName_(projectName),
      appType_(appType),
      frameRate_(deltaTime),
      windowWidth_(windowWidth),
      windowHeight_(windowHeight),
      windowName_(windowName),
      availableToolFlag_(availableToolFlag),
      scenesSetting_(sceneSettings),
      tagFileName_(tagFileName),
      defaultPoolSize_(defaultPoolSize)
{

}

bool EngineSetting::LoadSettingsFromFile(
    const char* fileName,
    const FileType fileType,
    const FolderType folderType)
{
    json outputFile = PokFileSystem::ReadJsonFile(
        fileName,
        fileType,
        folderType);
    return LoadSettingsFromJson(outputFile);
}

bool EngineSetting::LoadSettingsFromJson(json& json)
{
    if (json.empty()) { return false; }

	if (CheckJsonExists(json, "projectName") &&
		CheckJsonParameter(
			json,
			"projectName",
			nlohmann::detail::value_t::string)
		) {
		const std::string str = json["projectName"];
		projectName_ = str;
	}

    if (CheckJsonExists(json, "appType") &&
        CheckJsonNumber(json, "appType")) {
        const uint8_t res = json["appType"];
        appType_ = static_cast<AppType>(res);
    }

    if (CheckJsonExists(json, "frameRate") &&
        CheckJsonNumber(json, "frameRate")) {
        const float time = json["frameRate"];
        frameRate_ = std::chrono::duration<double, std::milli>(time);
    }

    if (CheckJsonExists(json, "windowWidth") &&
        CheckJsonNumber(json, "windowWidth")) {
        const int val = json["windowWidth"];
        windowWidth_ = val;
    }

    if (CheckJsonExists(json, "windowHeight") &&
        CheckJsonNumber(json, "windowHeight")) {
        const int val = json["windowHeight"];
        windowHeight_ = val;
    }

    if (CheckJsonExists(json, "windowName") &&
        CheckJsonParameter(
            json,
            "windowName",
            nlohmann::detail::value_t::string)
    ) {
        const std::string str = json["windowName"];
        windowName_ = str;
    }

	if (CheckJsonExists(json, "sceneSettings") &&
		CheckJsonParameter(
			json,
			"sceneSettings",
			nlohmann::detail::value_t::array) &&
		json["sceneSettings"].size() > 0) {
		json::array_t scenes = json["sceneSettings"];
		size_t numScenes = json["sceneSettings"].size();
		for (size_t i = 0; i < numScenes; i++) {
			json::object_t sceneObject = scenes[i];

			if (CheckJsonExists(sceneObject, "index") &&
				CheckJsonNumber(sceneObject, "index") &&
				CheckJsonExists(sceneObject, "name") &&
				CheckJsonParameter(
					sceneObject,
					"name",
					nlohmann::detail::value_t::string) &&
				CheckJsonExists(sceneObject, "fileName") &&
				CheckJsonParameter(
					sceneObject,
					"fileName",
					nlohmann::detail::value_t::string)
				) {
				SceneSetting newSceneSetting(
					{
						sceneObject["index"],
						sceneObject["name"],
						sceneObject["fileName"]
					});
				scenesSetting_.push_back(newSceneSetting);
			}
		}
	}

    if (CheckJsonExists(json, "availableTools") &&
        CheckJsonNumber(json, "availableTools")) {
        const int val = json["availableTools"];
        availableToolFlag_ = val;
    }

	if (CheckJsonExists(json, "tagFileName") &&
		CheckJsonParameter(
			json,
			"tagFileName",
			nlohmann::detail::value_t::string)) {
		const std::string tagFileName = json["tagFileName"];
		tagFileName_ = tagFileName;
	}

	if (CheckJsonExists(json, "defaultPoolSize") &&
		CheckJsonNumber(json, "defaultPoolSize")) {
		defaultPoolSize_ = json["defaultPoolSize"];
	}else { //TODO(@Nico) Security to write to every project the the value, remove once all project have been infected.
		defaultPoolSize_ = 8000;
	}

    return true;
}

json EngineSetting::SaveSettingsToJson() const
{
    json outputJson = json();

	outputJson["projectName"] = projectName_;
    outputJson["appType"] = static_cast<int>(appType_);
    outputJson["frameRate"] = frameRate_.count();
    outputJson["windowWidth"] = windowWidth_;
    outputJson["windowHeight"] = windowHeight_;
    outputJson["windowName"] = windowName_;
    outputJson["availableTools"] = availableToolFlag_;
	outputJson["tagFileName"] = tagFileName_;
	//outputJson["defaultPoolSize"] = defaultPoolSize_;
    
	outputJson["sceneSettings"] = json::array();
	const std::vector<scene::Scene>& currentScenes = SceneManagerLocator::Get().GetScenes();
	size_t numScenes = currentScenes.size();

	for (size_t i = 0; i < numScenes; i++) {
        if(currentScenes[i].GetSceneIndex() >= 50) {
			continue;
        }

		outputJson["sceneSettings"][i]["index"] = currentScenes[i].GetSceneIndex();
		outputJson["sceneSettings"][i]["name"] = currentScenes[i].GetSceneName();
		outputJson["sceneSettings"][i]["fileName"] = currentScenes[i].GetSceneFileName();
	}

    return outputJson;
}

}
