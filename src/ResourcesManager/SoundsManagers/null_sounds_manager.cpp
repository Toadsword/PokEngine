#include <ResourcesManager/SoundsManagers/null_sounds_manager.h>

namespace poke {

void NullSoundsManager::AddSound(const std::string& soundName)
{
    static_cast<std::string>(soundName);
}

void NullSoundsManager::AddMusic(const std::string& musicName)
{
    static_cast<std::string>(musicName);
}

const audio::AudioClip& NullSoundsManager::GetAudioClip(ResourceID resourceID) const
{
    static_cast<XXH64_hash_t>(resourceID);
    cassert(false, "Impossible to use null sounds manager");
	abort();
}

const audio::AudioClip& NullSoundsManager::GetAudioClipByName(
    const std::string& soundName) const
{
    static_cast<std::string>(soundName);
    cassert(false, "Impossible to use null sounds manager");
	abort();
}
} //namespace poke
