#include <ResourcesManager/SoundsManagers/music.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>

namespace poke {

Music::Music() : AudioClip("", false, true, true) {}

Music::Music(const std::string& clipName) : AudioClip(clipName, false, true, true) { }

void Music::Load() { InternLoad(static_cast<audio::AudioEngine&>(AudioEngineLocator::Get())); }

void Music::Unload() const { InternUnload(); }
} //namespace poke
