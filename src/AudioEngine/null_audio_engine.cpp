#include <AudioEngine/null_audio_engine.h>

namespace poke {
namespace audio {

void NullAudioEngine::LoadBank(const std::string& bankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags)
{
    bankName;
    flags;
}

void NullAudioEngine::Init() {}
void NullAudioEngine::Update() {}
void NullAudioEngine::Destroy() {}

void NullAudioEngine::LoadSound(
    const std::string& soundName,
    Clip& clip,
    bool is3D,
    bool looping,
    bool stream)
{
    soundName;
    clip;
    is3D;
    looping;
    stream;
    cassert(false, "Cannont load a sound in the null audio engine");
}

void NullAudioEngine::SetListener(
    math::Vec3 position,
    math::Vec3 velocity,
    math::Vec3 forward,
    math::Vec3 up)
{
    position;
    velocity;
    forward;
    up;
}

void NullAudioEngine::PlaySound(const Clip& clip, math::Vec3 pos, float volumeDB)
{
    clip;
    pos;
    volumeDB;
}

bool NullAudioEngine::IsPlaying(int channelID) const
{
    channelID;
    return false;
}

void NullAudioEngine::SetChannelVolume(int channelID, float volumeDB)
{
    channelID;
    volumeDB;
}

void NullAudioEngine::SetChannel3DAttributes(int channelID, math::Vec3 position, math::Vec3 velocity)
{
    channelID;
    position;
	velocity;
}

void NullAudioEngine::SetEventParameter(
    const std::string& eventName,
    const std::string& eventParameterName,
    float parameterValue)
{
    eventName;
    eventParameterName;
    parameterValue;
}

int NullAudioEngine::PlaySound(
    const AudioSource& audioSource,
    const AudioClip& audioClip,
    math::Vec3 pos)
{
    pos;
    audioClip;
    audioSource;
    return -1;
}

void NullAudioEngine::StopChannel(int channelID) { channelID; }

void NullAudioEngine::StopEvent(
    FMOD::Studio::EventInstance* instance,
    bool isStoppingImmediate)
{
    isStoppingImmediate;
    instance;
}

float NullAudioEngine::GetEventParameter(
    const std::string& eventName,
    const std::string& eventParameterName)
{
    eventName;
    eventParameterName;
    return 0;
}

void NullAudioEngine::StopAllChannels() {}

bool NullAudioEngine::IsEventPlaying(const FMOD::Studio::EventInstance* instance) const
{
    instance;
    return false;
}

void NullAudioEngine::LoadEventNames() {}

const std::vector<std::string>& NullAudioEngine::GetAllEventNames() { return {}; }

FMOD::Studio::EventInstance* NullAudioEngine::PlayEvent(
    const AudioSource& audioSource,
    const math::Vec3& pos)
{
    audioSource;
    pos;
	return nullptr;
}

void NullAudioEngine::PauseChannel(int channelID) { channelID; }

void NullAudioEngine::UnpauseChannel(int channelID) { channelID; }

void NullAudioEngine::SetEventVolume(FMOD::Studio::EventInstance* instance, float volumeDB)
{
    instance;
    volumeDB;
}

void NullAudioEngine::PauseEvent(FMOD::Studio::EventInstance* instance) { instance; }
void NullAudioEngine::UnpauseEvent(FMOD::Studio::EventInstance* instance) { instance; }

void NullAudioEngine::SetChannelPitch(int channelID, float pitch)
{
    channelID;
    pitch;
}

void NullAudioEngine::SetEventPitch(FMOD::Studio::EventInstance* instance, float pitch)
{
    instance;
    pitch;
}

void NullAudioEngine::SetEvent3DAttributes(
	FMOD::Studio::EventInstance* instance,
    math::Vec3 position,
    math::Vec3 velocity,
    math::Vec3 forward,
    math::Vec3 up)
{
	instance;
    position;
    velocity;
    forward;
    up;
}

void NullAudioEngine::SetChannelSpatialBlend(int channelID, float spatialBlend)
{
	channelID;
	spatialBlend;
}

void NullAudioEngine::SetEventSpatialBlend(
    FMOD::Studio::EventInstance* instance,
    float spatialBlend)
{
	instance;
	spatialBlend;
}

void NullAudioEngine::SetChannelMinDistance(int channelID, float minDistance)
{
	channelID;
	minDistance;
}

void NullAudioEngine::SetEventMinDistance(
    FMOD::Studio::EventInstance* instance,
    float minDistance)
{
	instance;
	minDistance;
}

void NullAudioEngine::SetChannelMaxDistance(int channelID, float maxDistance)
{
	channelID;
	maxDistance;
}

void NullAudioEngine::SetEventMaxDistance(
    FMOD::Studio::EventInstance* instance,
    float maxDistance)
{
	instance;
	maxDistance;
}

void NullAudioEngine::PlayEventInstance(const AudioSource& audioSource, const math::Vec3& pos)
{
	audioSource;
	pos;
}
} //namespace audio
} //namespace poke
