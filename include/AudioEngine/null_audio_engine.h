//----------------------------------------------------------------------------------
// Copyright (c) 2019-2020, POK Family. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of POK Family nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author : Nicolas Schneider
// Co-Author :
// Date : 18.02.20
//----------------------------------------------------------------------------------
#pragma once

#include <string>

#include <AudioEngine/interface_audio_engine.h>

namespace poke {
namespace audio {
class NullAudioEngine : public IAudioEngine {
public:
    void Init() override;

    void Update() override;

    void Destroy() override;

    void LoadBank(
        const std::string& bankName,
        FMOD_STUDIO_LOAD_BANK_FLAGS flags) override;

    void LoadSound(
        const std::string& soundName,
        Clip& clip,
        bool is3D,
        bool looping,
        bool stream) override;

    void SetListener(
        math::Vec3 position = math::Vec3{0, 0, 0},
        math::Vec3 velocity = math::Vec3{0, 0, 0},
        math::Vec3 forward = math::Vec3{0, 0, 0},
        math::Vec3 up = math::Vec3{0, 0, 0}) override;

    void PlaySound(
        const Clip& clip,
        math::Vec3 pos,
        float volumeDB) override;

    int PlaySound(
        const AudioSource& audioSource,
        const AudioClip& audioClip,
        math::Vec3 pos) override;

    void StopChannel(int channelID) override;

    void StopEvent(
		FMOD::Studio::EventInstance* instance,
        bool isStoppingImmediate) override;

    float GetEventParameter(
        const std::string& eventName,
        const std::string& eventParameterName) override;

    void SetEventParameter(
        const std::string& eventName,
        const std::string& eventParameterName,
        float parameterValue) override;

    void StopAllChannels() override;

    void SetChannel3DAttributes(int channelID, math::Vec3 position, math::Vec3 velocity) override;

    void SetChannelVolume(int channelID, float volumeDB) override;

    bool IsPlaying(int channelID) const override;

    bool IsEventPlaying(const FMOD::Studio::EventInstance* instance) const override;

    void LoadEventNames() override;

    const std::vector<std::string>& GetAllEventNames() override;

	FMOD::Studio::EventInstance* PlayEvent(const AudioSource& audioSource, const math::Vec3& pos) override;
    void PauseChannel(int channelID) override;
    void UnpauseChannel(int channelID) override;
    void SetEventVolume(FMOD::Studio::EventInstance* instance, float volumeDB) override;
    void PauseEvent(FMOD::Studio::EventInstance* instance) override;

    void UnpauseEvent(FMOD::Studio::EventInstance* instance) override;

    void SetChannelPitch(int channelID, float pitch) override;
    void SetEventPitch(FMOD::Studio::EventInstance* instance, float pitch) override;

    void SetEvent3DAttributes(
		FMOD::Studio::EventInstance* instance,
        math::Vec3 position,
        math::Vec3 velocity,
        math::Vec3 forward,
        math::Vec3 up) override;

    void SetChannelSpatialBlend(int channelID, float spatialBlend) override;
    void SetEventSpatialBlend(FMOD::Studio::EventInstance* instance, float spatialBlend) override;

    void SetChannelMinDistance(int channelID, float minDistance) override;
    void SetEventMinDistance(FMOD::Studio::EventInstance* instance, float minDistance) override;

    void SetChannelMaxDistance(int channelID, float maxDistance) override;
    void SetEventMaxDistance(FMOD::Studio::EventInstance* instance, float maxDistance) override;

    void PlayEventInstance(const AudioSource& audioSource, const math::Vec3& pos) override;
};
} //namespace audio
} //namespace poke