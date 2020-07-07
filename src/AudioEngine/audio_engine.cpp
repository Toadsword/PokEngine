#include <AudioEngine/audio_engine.h>

#include <AudioEngine/fmod_error_handler.h>
#include <AudioEngine/audio_source.h>
#include <AudioEngine/audio_clip.h>

namespace poke {
namespace audio {
AudioEngine::AudioEngine() { }

void AudioEngine::Init() { fmodImplementation_.Init(); }
void AudioEngine::Update() { fmodImplementation_.Update(); }
void AudioEngine::Destroy() { fmodImplementation_.Destroy(); }

void AudioEngine::LoadBank(
    const std::string& bankName,
    const FMOD_STUDIO_LOAD_BANK_FLAGS flags)
{
    const auto it = fmodImplementation_.banks.find(bankName);
    if (it != fmodImplementation_.banks.end()) { return; }

    FMOD::Studio::Bank* bank;
    CheckFMOD(
        fmodImplementation_.studioSystem->loadBankFile(
            bankName.c_str(),
            flags,
            &bank)
    );

    if (bank) {
        fmodImplementation_.banks[bankName] = bank;
        LogDebug(
            "The FMOD bank [" + bankName + "] has been successfuly loaded",
            LogType::AUDIO_LOG);
    }
}

void AudioEngine::LoadEventNames()
{
    for (const auto& bank : fmodImplementation_.banks) {
        //Get the number of event
        int count;
        CheckFMOD(bank.second->getEventCount(&count));

        //If == 0, continue because there is no events in the bank
        if (count == 0)
            continue;

        //Get the event description list
        FMOD::Studio::EventDescription** events = new FMOD::Studio::EventDescription*[count];
        memset(events, 0, sizeof(events));
        CheckFMOD(bank.second->getEventList(events, count, &count));

        //Foreach event, get the path and keep it in eventNames
        for (int i = 0; i < count; i++) {
            char eventPath[256];
            int len = 0;
            CheckFMOD(events[i]->getPath(eventPath, sizeof(eventPath) - 1, &len));
            std::string name = eventPath;
            eventNames_.push_back(name);
        }

        LogDebug(
            std::to_string(count) + " events have been loaded from the FMO BANK [" + bank.first +
            "]",
            LogType::AUDIO_LOG);

        delete[] events;
    }
    LogDebug(
        "In total there is " + std::to_string(eventNames_.size()) + " events loaded.",
        LogType::AUDIO_LOG);
}

void AudioEngine::LoadSound(
    const std::string& soundName,
    Clip& clip,
    const bool is3D,
    const bool isLooping,
    const bool isStreamLoading)
{
    FMOD_MODE mode = FMOD_DEFAULT;
    mode |= is3D ? FMOD_3D : FMOD_2D;
    mode |= isLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
    mode |= isStreamLoading ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

    CheckFMOD(
        fmodImplementation_.system->createSound(
            soundName.c_str(),
            mode,
            nullptr,
            &clip));
}

void AudioEngine::PlaySound(
    const Clip& clip,
    const math::Vec3 pos,
    const float volumeDB)
{
    const int channelID = fmodImplementation_.nextChannelId++;

    FMOD::Channel* channel = nullptr;
    CheckFMOD(
        fmodImplementation_.system->playSound(
            clip,
            nullptr,
            true,
            &channel));

    if (channel) {
        FMOD_MODE currentMode;
        clip->getMode(&currentMode);

        if (currentMode & FMOD_3D) {
            FMOD_VECTOR position = Vec3ToFMOD(pos);
            CheckFMOD(
                channel->set3DAttributes(&position, nullptr));
        }

        CheckFMOD(channel->setVolume(DbToVolume(volumeDB)));
        CheckFMOD(channel->setPaused(false));
        fmodImplementation_.channels[channelID] = channel;
    }
}

int AudioEngine::PlaySound(
    const AudioSource& audioSource,
    const AudioClip& audiClip,
    const math::Vec3 pos)
{
    const int channelID = fmodImplementation_.nextChannelId++;

    FMOD::Channel* channel = nullptr;
    CheckFMOD(
        fmodImplementation_.system->playSound(
            audiClip.GetClip(),
            nullptr,
            true,
            &channel));

    if (channel) {
        FMOD_MODE currentMode;
        audiClip.GetClip()->getMode(&currentMode);

        if (currentMode & FMOD_3D) {
			CheckFMOD(channel->set3DLevel(1));
            auto position = Vec3ToFMOD(pos);
            CheckFMOD(channel->set3DAttributes(&position, nullptr));
            CheckFMOD(
                channel->set3DMinMaxDistance(
                    audioSource.GetMinDistance(),
                    audioSource.GetMaxDistance()));
        }else {
			CheckFMOD(channel->set3DLevel(0));
        }

        CheckFMOD(channel->setPitch(audioSource.GetPitch()));
        CheckFMOD(channel->setVolume(DbToVolume(audioSource.GetVolume())));
        CheckFMOD(channel->setPaused(false));

        fmodImplementation_.channels[channelID] = channel;
    }

	return channelID;
}

FMOD::Studio::EventInstance* AudioEngine::PlayEvent(const AudioSource& audioSource, const math::Vec3& pos)
{
    FMOD::Studio::EventDescription* description = nullptr;
    fmodImplementation_.studioSystem->getEvent(audioSource.eventName.c_str(), &description);

	int count = 0;
	description->getInstanceCount(&count);
	if (count >= kMaxInstanceSimultaneously_) { return nullptr; }

    FMOD::Studio::EventInstance* instance = nullptr;
    CheckFMOD(description->createInstance(&instance));

	FMOD_STUDIO_EVENT_PROPERTY property;
	//property.
	CheckFMOD(instance->setVolume(audioSource.GetVolume() / kMaxVolume));

    CheckFMOD(instance->setPitch(audioSource.GetPitch()));

    FMOD_3D_ATTRIBUTES attributes;
    attributes.position = Vec3ToFMOD(pos);
	attributes.up = { 0, 1, 0 };
	attributes.forward = { 0, 0, 1 };
    CheckFMOD(instance->set3DAttributes(&attributes));

	CheckFMOD(instance->setProperty(FMOD_STUDIO_EVENT_PROPERTY_MINIMUM_DISTANCE, audioSource.GetMinDistance()));
	CheckFMOD(instance->setProperty(FMOD_STUDIO_EVENT_PROPERTY_MAXIMUM_DISTANCE, audioSource.GetMaxDistance()));

    CheckFMOD(instance->start());

	return instance;
}

void AudioEngine::PlayEventInstance(const AudioSource& audioSource, const math::Vec3& pos)
{
	auto instance = PlayEvent(audioSource, pos);

	CheckFMOD(instance->setCallback(EventInstanceStopped, FMOD_STUDIO_EVENT_CALLBACK_SOUND_STOPPED));
}

void AudioEngine::PauseChannel(const int channelID)
{
	const auto it = fmodImplementation_.channels.find(channelID);
	if (it == fmodImplementation_.channels.end()) { return; }

	CheckFMOD(it->second->setPaused(true));
}

void AudioEngine::UnpauseChannel(const int channelID)
{
	const auto it = fmodImplementation_.channels.find(channelID);
	if (it == fmodImplementation_.channels.end()) { return; }

	CheckFMOD(it->second->setPaused(false));
}

void AudioEngine::PauseEvent(FMOD::Studio::EventInstance* instance)
{
	CheckFMOD(instance->setPaused(true));
}

void AudioEngine::UnpauseEvent(FMOD::Studio::EventInstance* instance)
{
	CheckFMOD(instance->setPaused(false));
}

void AudioEngine::StopChannel(const int channelID)
{
    const auto it = fmodImplementation_.channels.find(channelID);
    if (it == fmodImplementation_.channels.end()) { return; }

    CheckFMOD(it->second->stop());
}

void AudioEngine::StopEvent(
	FMOD::Studio::EventInstance* instance,
    const bool isStoppingImmediate)
{
    const auto mode = isStoppingImmediate ?
                          FMOD_STUDIO_STOP_IMMEDIATE :
                          FMOD_STUDIO_STOP_ALLOWFADEOUT;

    CheckFMOD(instance->stop(mode));
	CheckFMOD(instance->release());
}

float AudioEngine::GetEventParameter(
    const std::string& eventName,
    const std::string& eventParameterName)
{
    FMOD::Studio::EventDescription* description = nullptr;
    fmodImplementation_.studioSystem->getEvent(eventName.c_str(), &description);

    FMOD::Studio::EventInstance* instance = nullptr;
    CheckFMOD(description->createInstance(&instance));

    float value = 0;
    CheckFMOD(instance->getParameterByName(eventParameterName.c_str(), &value));
    return value;
}

void AudioEngine::SetEventParameter(
    const std::string& eventName,
    const std::string& eventParameterName,
    const float parameterValue)
{
    FMOD::Studio::EventDescription* description = nullptr;
    fmodImplementation_.studioSystem->getEvent(eventName.c_str(), &description);

    FMOD::Studio::EventInstance* instance = nullptr;
    CheckFMOD(description->createInstance(&instance));

    CheckFMOD(instance->setParameterByName(eventParameterName.c_str(), parameterValue));
}

void AudioEngine::StopAllChannels()
{
    for (const auto channel : fmodImplementation_.channels) { StopChannel(channel.first); }
}

void AudioEngine::SetChannelVolume(
    const int channelID,
    const float volumeDB)
{
    const auto it = fmodImplementation_.channels.find(channelID);
    if (it == fmodImplementation_.channels.end()) { return; }

    CheckFMOD(it->second->setVolume(volumeDB));
}

void AudioEngine::SetEventVolume(FMOD::Studio::EventInstance* instance, const float volumeDB)
{
	instance->setVolume(volumeDB / kMaxVolume);
}

void AudioEngine::SetChannelPitch(const int channelID, const float pitch)
{
	const auto it = fmodImplementation_.channels.find(channelID);
	if (it == fmodImplementation_.channels.end()) { return; }
	CheckFMOD(it->second->setPitch(pitch));
}

void AudioEngine::SetEventPitch(FMOD::Studio::EventInstance* instance, const float pitch)
{
	instance->setPitch(pitch);
}

void AudioEngine::SetChannel3DAttributes(
    const int channelID,
    const math::Vec3 pos,
	const math::Vec3 velocity)
{
    const auto it = fmodImplementation_.channels.find(channelID);
    if (it == fmodImplementation_.channels.end()) { return; }
    FMOD_VECTOR position = Vec3ToFMOD(pos);
    FMOD_VECTOR vel = Vec3ToFMOD(velocity);
    CheckFMOD(it->second->set3DAttributes(&position, &vel));
}

void AudioEngine::SetEvent3DAttributes(
	FMOD::Studio::EventInstance* instance,
    const math::Vec3 position,
    const math::Vec3 velocity,
    const math::Vec3 forward,
    const math::Vec3 up)
{
	FMOD_3D_ATTRIBUTES attributes = {{0}};
	attributes.position = Vec3ToFMOD(position);
	attributes.velocity = Vec3ToFMOD(velocity);
	attributes.forward = Vec3ToFMOD(forward);
	attributes.up = Vec3ToFMOD(up);
	CheckFMOD(instance->set3DAttributes(&attributes));
}

void AudioEngine::SetChannelSpatialBlend(const int channelID, const float spatialBlend)
{
	const auto it = fmodImplementation_.channels.find(channelID);
	if (it == fmodImplementation_.channels.end()) { return; }
	CheckFMOD(it->second->set3DLevel(spatialBlend));
}

void AudioEngine::SetEventSpatialBlend(FMOD::Studio::EventInstance* instance, float spatialBlend)
{
    //instance->Set
}

void AudioEngine::SetChannelMinDistance(const int channelID, const float minDistance)
{
	const auto it = fmodImplementation_.channels.find(channelID);
	if (it == fmodImplementation_.channels.end()) { return; }

	float max = 0;
	it->second->get3DMinMaxDistance(nullptr, &max);

	CheckFMOD(it->second->set3DMinMaxDistance(minDistance, max));
}

void AudioEngine::SetEventMinDistance(FMOD::Studio::EventInstance* instance, const float minDistance)
{
	CheckFMOD(instance->setProperty(FMOD_STUDIO_EVENT_PROPERTY_MINIMUM_DISTANCE, minDistance));
}

void AudioEngine::SetChannelMaxDistance(const int channelID, const float maxDistance)
{
	const auto it = fmodImplementation_.channels.find(channelID);
	if (it == fmodImplementation_.channels.end()) { return; }

	float min = 0;
	it->second->get3DMinMaxDistance(&min, nullptr);

	CheckFMOD(it->second->set3DMinMaxDistance(min, maxDistance));
}

void AudioEngine::SetEventMaxDistance(FMOD::Studio::EventInstance* instance, const float maxDistance)
{
	CheckFMOD(instance->setProperty(FMOD_STUDIO_EVENT_PROPERTY_MAXIMUM_DISTANCE, maxDistance));
}

bool AudioEngine::IsPlaying(const int channelID) const
{
    const auto it = fmodImplementation_.channels.find(channelID);
    if (it == fmodImplementation_.channels.end()) { return false; }

    auto isPlaying = false;

    CheckFMOD(it->second->isPlaying(&isPlaying));

    return isPlaying;
}

bool AudioEngine::IsEventPlaying(const FMOD::Studio::EventInstance* instance) const
{
    FMOD_STUDIO_PLAYBACK_STATE state;
    CheckFMOD(instance->getPlaybackState(&state));
    return state == FMOD_STUDIO_PLAYBACK_PLAYING;
}

float AudioEngine::DbToVolume(const float db) { return powf(10.0f, 0.05f * db); }

float AudioEngine::VolumeToDB(const float volume) { return 20.0f * log10f(volume); }

FMOD_VECTOR AudioEngine::Vec3ToFMOD(const math::Vec3 position)
{
    FMOD_VECTOR fmodPosition;
    fmodPosition.x = position.x;
    fmodPosition.y = position.y;
    fmodPosition.z = position.z;
    return fmodPosition;
}

void AudioEngine::SetListener(
    const math::Vec3 position,
    const math::Vec3 velocity,
    const math::Vec3 forward,
    const math::Vec3 up)
{
    FMOD_3D_ATTRIBUTES attributes;
    attributes.position = Vec3ToFMOD(position);
    attributes.forward = Vec3ToFMOD(forward);
    attributes.up = Vec3ToFMOD(up);
    attributes.velocity = Vec3ToFMOD(velocity);

    fmodImplementation_.studioSystem->setListenerAttributes(0, &attributes);
}

const std::vector<std::string>& AudioEngine::GetAllEventNames() { return eventNames_; }

FMOD_RESULT AudioEngine::EventInstanceStopped(
    FMOD_STUDIO_EVENT_CALLBACK_TYPE type,
    FMOD_STUDIO_EVENTINSTANCE* instance,
    void* parameters)
{
    auto eventInstance = reinterpret_cast<FMOD::Studio::EventInstance*>(instance);

	return eventInstance->release();
}
} //namespace audio
} //namespace poke
