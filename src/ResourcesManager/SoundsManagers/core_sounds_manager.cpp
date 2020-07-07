#include <ResourcesManager/SoundsManagers/core_sounds_manager.h>

#include <Math/math.h>
#include <Utility/log.h>
#include <json.hpp>

namespace poke {

CoreSoundsManager::CoreSoundsManager() {}

void CoreSoundsManager::AddSound(const std::string& soundName)
{
    auto hash = math::HashString(soundName);

    soundIDs_.emplace_back(hash);
	sounds_.emplace_back(Sound(PokFileSystem::GetAbsoluteFullPath(
		soundName.c_str(),
		FileType::SOUNDS,
		FolderType::ROM)));
    sounds_.back().Load();
}

void CoreSoundsManager::AddMusic(const std::string& musicName)
{
    auto hash = math::HashString(musicName);

    musicIDs_.emplace_back(hash);
	musics_.emplace_back(Music(PokFileSystem::GetAbsoluteFullPath(
		musicName.c_str(),
		FileType::SOUNDS,
		FolderType::ROM)));
    musics_.back().Load();
}

const audio::AudioClip& CoreSoundsManager::GetAudioClip(
    ResourceID resourceID) const
{
    for (size_t i = 0; i < soundIDs_.size(); i++) {
        if (soundIDs_[i] == resourceID) { return sounds_[i]; }
    }

    for (size_t i = 0; i < musicIDs_.size(); i++) {
        if (musicIDs_[i] == resourceID) { return musics_[i]; }
    }
    return musics_[0];
    cassert(false, "Unable to find the sound");
}

const audio::AudioClip& CoreSoundsManager::GetAudioClipByName(
    const std::string& soundName) const
{
    const auto hash = math::HashString(soundName);

    return GetAudioClip(hash);
}

void CoreSoundsManager::Clear()
{
    soundIDs_.clear();
    sounds_.clear();

    musicIDs_.clear();
    musics_.clear();
}

void CoreSoundsManager::Resize(const size_t newSize)
{
    if (soundIDs_.size() > newSize) {
        soundIDs_.resize(newSize);
        sounds_.resize(newSize);

        musicIDs_.resize(newSize);
        musics_.resize(newSize);
    } else {
        soundIDs_.reserve(newSize);
        sounds_.reserve(newSize);

        musicIDs_.reserve(newSize);
        musics_.reserve(newSize);
    }
}

void CoreSoundsManager::SetFromJson(const json& soundsJson)
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
}

json CoreSoundsManager::ToJson()
{
    LogWarning("You're trying to save using the CoreSoundsManager, nothing will be saved.");
    return json();
}

void CoreSoundsManager::LoadDefaultSound()
{
    const auto masterPath = PokFileSystem::GetAbsoluteFullPath(
        "pok_level_1_with_guitar_draft.wav",
        FileType::SOUNDS,
        FolderType::ROM);

    const auto hash = math::HashString("pok_level_1_with_guitar_draft.wav");
    musicIDs_.emplace_back(hash);
    musics_.emplace_back(Music(masterPath));
    musics_.back().Load();
}
} //namespace poke
