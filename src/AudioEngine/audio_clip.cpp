#include <AudioEngine/audio_clip.h>
#include <AudioEngine/fmod_error_handler.h>

namespace poke {
namespace audio {

AudioClip::AudioClip()
    : clip_(nullptr),
      clipName_(""),
      is3D_(false),
      isLooping_(false),
      isStreamingLoading_(false) { }

AudioClip::AudioClip(
    const std::string& clipName,
    const bool is3D,
    const bool isLooping,
    const bool isStreamingLoading)
    : clip_(nullptr),
      clipName_(clipName),
      is3D_(is3D),
      isLooping_(isLooping),
      isStreamingLoading_(isStreamingLoading) { }

AudioClip::~AudioClip() { if (clip_) { CheckFMOD(clip_->release()); } }
Clip AudioClip::GetClip() const { return clip_; }

void AudioClip::InternLoad(AudioEngine& audioEngine)
{
    audioEngine.LoadSound(
        clipName_,
        clip_,
        is3D_,
        isLooping_,
        isStreamingLoading_);
}

void AudioClip::InternUnload() const
{
    CheckFMOD(clip_->release());
}
} //namespace audio
} //namespace poke
