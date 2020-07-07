#include <ResourcesManager/SoundsManagers/sound.h>

#include <CoreEngine/ServiceLocator/service_locator_definition.h>

namespace poke {

Sound::Sound() : AudioClip("", false, false, false) {}

Sound::Sound(const std::string& clipName)
    : AudioClip(clipName, true, false, false) { }

void Sound::Load()
{
    InternLoad(static_cast<audio::AudioEngine&>(AudioEngineLocator::Get()));
}

void Sound::Unload() const
{
    InternUnload();
}
} //namespace poke
