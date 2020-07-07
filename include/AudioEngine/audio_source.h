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
// Date :  18.02.20
//----------------------------------------------------------------------------------
#pragma once
#include <Utility/json_utility.h>
#include <ResourcesManager/resource_type.h>
#include <Math/vector.h>
#include <fmod_studio.hpp>

namespace poke {
namespace audio {
/**
 * \brief 
 */
struct AudioSource {
	enum class State : uint8_t {
		STOPPED = 0,
		PLAYING,
		PAUSED
	};

    AudioSource();

    ~AudioSource();

    bool operator==(const AudioSource& other) const;

    bool operator!=(const AudioSource& other) const;

    bool IsPlaying();

    bool IsPaused() const;

    void Play(const math::Vec3& worldPos);
    void PlayInstance(const math::Vec3& worldPos);

	void Pause();

	void Unpause();

	void Stop();

	float GetSpatialBlend() const { return spatialBlend_; }
	void SetSpatialBlend(float spatialBlend);

	float GetVolume() const { return volume_; }
	void SetVolume(float volumeDB);

	float GetPitch() const { return pitch_; }
	void SetPitch(float pitch);

	float GetMinDistance() const { return minDistance_; }
    void SetMinDistance(float minDistance);

	float GetMaxDistance() const { return maxDistance_; }
    void SetMaxDistance(float maxDistance);

	void Update3DAttributes(
		math::Vec3 position,
		math::Vec3 velocity,
		math::Vec3 forward,
		math::Vec3 up);

	bool isEvent = false;
    //TODO(@Nico) Store event's names in sounds manager
	ResourceID soundID = 0; 
	std::string eventName;
	bool playOnLoadScene = false;
	FMOD::Studio::EventInstance* eventInstance = nullptr;

    void SetFromJson(const json& componentJson);

    json ToJson() const;

private:
	float volume_ = 0.0f;
	float pitch_ = 1.0f;
	float spatialBlend_ = 1;
	float minDistance_ = 0.0f;
	float maxDistance_ = 500.0f;

	int channelID_;

	State state_ = State::STOPPED;
};
} //namespace audio
} //namespace poke