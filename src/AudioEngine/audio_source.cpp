#include <AudioEngine/audio_source.h>

#include <CoreEngine/ServiceLocator/service_locator_definition.h>
#include <Utility/log.h>

namespace poke {
namespace audio {

AudioSource::AudioSource()
    : isEvent(false),
      soundID(0) {}

AudioSource::~AudioSource() { }

bool AudioSource::operator==(const AudioSource& other) const
{
    if (isEvent != other.isEvent) { return false; }
    if (isEvent && eventName != other.eventName) { return false; }
    if (!isEvent && soundID != other.soundID) { return false; }

    return soundID == other.soundID
           && playOnLoadScene == other.playOnLoadScene
           && minDistance_ == other.minDistance_
           && maxDistance_ == other.maxDistance_
           && volume_ == other.volume_
           && pitch_ == other.pitch_
           && state_ == other.state_
           && channelID_ == other.channelID_
           && eventInstance == other.eventInstance
           && spatialBlend_ == other.spatialBlend_;
}

bool AudioSource::operator!=(const AudioSource& other) const { return !(*this == other); }

bool AudioSource::IsPlaying()
{
	bool isPlaying = false;

	if (isEvent) {
		if (eventInstance)
			isPlaying =  AudioEngineLocator::Get().IsEventPlaying(eventInstance);
	}
	else {
		isPlaying = AudioEngineLocator::Get().IsPlaying(channelID_);
	}

    if(state_ == State::PAUSED) {
		return isPlaying;
    }

    state_ = isPlaying ? State::PLAYING : State::STOPPED;

    return isPlaying;
}

bool AudioSource::IsPaused() const { return state_ == State::PAUSED; }

void AudioSource::Play(const math::Vec3& worldPos)
{
    if (isEvent) {
        if (eventName != "")
            eventInstance = AudioEngineLocator::Get().PlayEvent(*this, worldPos);
    } else {
        if (soundID != 0)
            channelID_ = AudioEngineLocator::Get().PlaySound(
                *this,
                SoundsManagerLocator::Get().GetAudioClip(soundID),
                worldPos);
    }

    state_ = State::PLAYING;
}

void AudioSource::PlayInstance(const math::Vec3& worldPos)
{
	if (isEvent) {
		AudioEngineLocator::Get().PlayEventInstance(*this, worldPos);
	}
	else {
		LogWarning("Impossible to play an instance of a sound that is not a FMOD event.", LogType::AUDIO_LOG);
	}
}

void AudioSource::Pause()
{
    if (isEvent) {
        if(eventInstance)
		    AudioEngineLocator::Get().PauseEvent(eventInstance);
    } else {
        AudioEngineLocator::Get().PauseChannel(channelID_);
    }

    state_ = State::PAUSED;
}

void AudioSource::Unpause()
{
	if (isEvent) {
		if (eventInstance)
		    AudioEngineLocator::Get().UnpauseEvent(eventInstance);
	}
	else {
	    AudioEngineLocator::Get().UnpauseChannel(channelID_);
	}

	state_ = State::PLAYING;
}

void AudioSource::Stop()
{
    if (isEvent && eventInstance) {
        AudioEngineLocator::Get().StopEvent(eventInstance, true);
		eventInstance = nullptr;
    } else {
        AudioEngineLocator::Get().StopChannel(channelID_);
    }

    state_ = State::STOPPED;
}

void AudioSource::SetSpatialBlend(const float spatialBlend)
{
	spatialBlend_ = spatialBlend;

    if(!isEvent) {
		AudioEngineLocator::Get().SetChannelSpatialBlend(channelID_, spatialBlend);
    }
}

void AudioSource::SetVolume(const float volumeDB)
{
	volume_ = volumeDB;

	if (state_ != State::PLAYING) return;
	if (isEvent && eventInstance) {
		AudioEngineLocator::Get().SetEventVolume(eventInstance, volumeDB);
	}
	else {
		AudioEngineLocator::Get().SetChannelVolume(channelID_, volumeDB);
	}
}

void AudioSource::SetPitch(const float pitch)
{
	pitch_ = pitch;

	if (state_ != State::PLAYING) return;
	if (isEvent && eventInstance) {
		AudioEngineLocator::Get().SetEventPitch(eventInstance, pitch_);
	}
	else {
		AudioEngineLocator::Get().SetChannelPitch(channelID_, pitch_);
	}
}

void AudioSource::SetMinDistance(const float minDistance)
{
    minDistance_ = minDistance;

	if (isEvent && eventInstance) {
		AudioEngineLocator::Get().SetEventMinDistance(eventInstance, minDistance_);
	}
	else {
		if (spatialBlend_ == 0.0f) return;
		AudioEngineLocator::Get().SetChannelMinDistance(channelID_, minDistance_);
	}
}

void AudioSource::SetMaxDistance(const float maxDistance)
{
    maxDistance_ = maxDistance;

	if (isEvent && eventInstance) {
		AudioEngineLocator::Get().SetEventMaxDistance(eventInstance, maxDistance_);
	}
	else {
		if (spatialBlend_ == 0.0f) return;
		AudioEngineLocator::Get().SetChannelMaxDistance(channelID_, maxDistance_);
	}
}

void AudioSource::Update3DAttributes(
    const math::Vec3 position,
    const math::Vec3 velocity,
    const math::Vec3 forward,
    const math::Vec3 up)
{
	if (state_ != State::PLAYING) return;
	if (isEvent && eventInstance) {
		AudioEngineLocator::Get().SetEvent3DAttributes(eventInstance, position, velocity, forward, up);
	}
	else {
	    if (spatialBlend_ == 0.0f) return;
		    AudioEngineLocator::Get().SetChannel3DAttributes(channelID_, position, velocity);
	}
}

void AudioSource::SetFromJson(const json& componentJson)
{
    isEvent = componentJson["isEvent"];
    eventName = componentJson["eventName"].get<std::string>();
    soundID = componentJson["audioClipName"];
    playOnLoadScene = componentJson["playOnAwake"];
    minDistance_ = componentJson["minDistance"];
    maxDistance_ = componentJson["maxDistance"];
    volume_ = componentJson["volume"];
    pitch_ = componentJson["pitch"];
	spatialBlend_ = componentJson["spatialBlend"];
}

json AudioSource::ToJson() const
{
    json componentJson;

    componentJson["isEvent"] = isEvent;
    componentJson["eventName"] = eventName;
    componentJson["audioClipName"] = soundID;

    componentJson["playOnAwake"] = playOnLoadScene;
    componentJson["minDistance"] = minDistance_;
    componentJson["maxDistance"] = maxDistance_;
    componentJson["volume"] = volume_;
    componentJson["pitch"] = pitch_;
	componentJson["spatialBlend"] = spatialBlend_;

    return componentJson;
}
} //namespace audio
} //namespace poke
