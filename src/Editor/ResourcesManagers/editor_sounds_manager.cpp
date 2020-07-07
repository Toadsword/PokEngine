#include <Editor/ResourcesManagers/editor_sounds_manager.h>
#include <Utility/log.h>
#include <json.hpp>

namespace poke {
namespace editor {

EditorSoundsManager::EditorSoundsManager() : CoreSoundsManager()
{

}

void EditorSoundsManager::AddSound(const std::string& soundName)
{
	const auto it = std::find(soundNames_.begin(), soundNames_.end(), soundName);

    if(it != soundNames_.end()) {
		return;
    }

	soundNames_.push_back(soundName);
	CoreSoundsManager::AddSound(soundName);
}

void EditorSoundsManager::AddMusic(const std::string& musicName)
{
	const auto it = std::find(musicNames_.begin(), musicNames_.end(), musicName);

	if (it != musicNames_.end()) {
		return;
	}

	musicNames_.push_back(musicName);
	CoreSoundsManager::AddMusic(musicName);
}

void EditorSoundsManager::Clear()
{
	CoreSoundsManager::Clear();

	soundNames_.clear();
	musicNames_.clear();
}

void EditorSoundsManager::SetFromJson(const json& soundsJson)
{
	if (CheckJsonExists(soundsJson, "clips")) {
        for (const auto& clipJson : soundsJson["clips"]) {
			AddSound(clipJson.get<std::string>());
        }
	}

	if (CheckJsonExists(soundsJson, "musics")) {
		for (const auto& clipJson : soundsJson["musics"]) {
			AddMusic(clipJson.get<std::string>());
		}
	}

	CoreSoundsManager::SetFromJson(soundsJson);
}

json EditorSoundsManager::ToJson()
{
	json audioJson;
    for (const auto& soundName : soundNames_) {
		audioJson["clips"].push_back(soundName);
    }

	for (const auto& musicName : musicNames_) {
		audioJson["musics"].push_back(musicName);
	}
	return audioJson;
}

const std::vector<std::string>& EditorSoundsManager::GetSoundNames() const
{
    return soundNames_;
}

const std::vector<std::string>& EditorSoundsManager::GetMusicNames() const
{
    return musicNames_;
}

const std::string EditorSoundsManager::GetSoundName(const ResourceID resourceID) const
{
    for (size_t i = 0; i < soundIDs_.size(); i++) {
        if (soundIDs_[i] == resourceID) { return soundNames_[i]; }
    }

    return "";
}

const std::string EditorSoundsManager::GetMusicName(const ResourceID resourceID) const
{
    for (size_t i = 0; i < musicIDs_.size(); i++) {
        if (musicIDs_[i] == resourceID) { return musicNames_[i]; }
    }

    return "";
}
} //namespace editor
} //namespace poke