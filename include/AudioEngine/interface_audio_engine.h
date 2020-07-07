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

#include <fmod_studio.hpp>
#include <Math/vector.h>
#include <AudioEngine/implementation.h>

namespace poke {
namespace audio {
class AudioClip;
struct AudioSource;

class IAudioEngine {
public:
    virtual ~IAudioEngine() = default;
    /**
	 * \brief This function initialized everything needed for the audio engine. This function must be called if the audio en
	 */
    virtual void Init() = 0;
    /**
	 * \brief Call this function every frame for a normal usage of the audi engine.
	 */
    virtual void Update() = 0;
    /**
	 * \brief Destroy the audio engine when it's not needed anymore.
	 */
    virtual void Destroy() = 0;

    /**
     * \brief Load an audio Band from FMOD STUDIO.
     * \param bankName 
     * \param flags 
     */
    virtual void LoadBank(
        const std::string& bankName,
        FMOD_STUDIO_LOAD_BANK_FLAGS flags) = 0;

    /**
	 * \brief Loads all event names from loaded bank
	 * \details This must be called after every bank has been loaded. It reads all bank and gets every events names. They can be access with the function GetAllEventNames().
	 */
	virtual void LoadEventNames() = 0;

    /**
	 * \brief Return a list of all event's names
	 * \return 
	 */
	virtual const std::vector<std::string>& GetAllEventNames() = 0;

    /**
     * \brief Load a sound with the file name.
     * \param soundName
     * \param clip 
     * \param is3D 
     * \param looping 
     * \param stream
     */
    virtual void LoadSound(
        const std::string& soundName,
		Clip& clip,
        bool is3D = true,
        bool looping = false,
        bool stream = false) = 0;

    /**
     * \brief Set all data for a Listener, this must be updated every frame.
     * \param position 
     * \param velocity 
     * \param forward 
     * \param up 
     */
    virtual void SetListener(
        math::Vec3 position = math::Vec3{0, 0, 0},
        math::Vec3 velocity = math::Vec3{0, 0, 0},
        math::Vec3 forward = math::Vec3{0, 0, 0},
        math::Vec3 up = math::Vec3{0, 0, 0}) = 0;

    /**
     * \brief Play a sound by its given name.
     * \param clip 
     * \param pos 
     * \param volumeDB 
     */
    virtual void PlaySound(
        const Clip& clip,
        math::Vec3 pos = math::Vec3{0, 0, 0},
        float volumeDB = 0) = 0;

    /**
	 * \brief Play a sound by its given audioSource. This mode is more complex and give more control over the sound.
	 * \param audioSource
	 * \param audiClip 
	 * \param pos
	 * \return channelID
	 */
	virtual int PlaySound(
        const AudioSource& audioSource,
        const AudioClip& audiClip,
        math::Vec3 pos) = 0;

	/**
	* \brief Play an event by its given name.
	* \param audioSource
	* \param pos
	*/
	virtual FMOD::Studio::EventInstance* PlayEvent(const AudioSource& audioSource, const math::Vec3& pos) = 0;

	/**
	* \brief Play an instance of an event by its given name.
	* \details Once this function called it's impossible to interact with the instance anymore.
	* \param audioSource
	* \param pos
	*/
	virtual void PlayEventInstance(const AudioSource& audioSource, const math::Vec3& pos) = 0;

    /**
	 * \brief Pause the channel
	 * \param channelID 
	 */
	virtual void PauseChannel(int channelID) = 0;

	/**
	 * \brief Pause the channel
	 * \param channelID
	 */
	virtual void UnpauseChannel(int channelID) = 0;

	/**
	 * \brief Pause the event
	 * \param instance
	 */
	virtual void PauseEvent(FMOD::Studio::EventInstance* instance) = 0;

	/**
	 * \brief Pause the event
	 * \param instance
	 */
	virtual void UnpauseEvent(FMOD::Studio::EventInstance* instance) = 0;

    /**
     * \brief Stop a channel. It doesn't release its data.
     * \param channelID 
     */
    virtual void StopChannel(int channelID) = 0;

    /**
     * \brief Stop an event.
     * \param instance 
     * \param isStoppingImmediate 
     */
    virtual void StopEvent(
		FMOD::Studio::EventInstance* instance,
        bool isStoppingImmediate) = 0;

    /**
     * \brief Get an event's parameter from its name.
     * \param eventName 
     * \param eventParameterName 
     * \return 
     */
    virtual float GetEventParameter(
        const std::string& eventName,
        const std::string& eventParameterName) = 0;

    /**
     * \brief Set an event's parameter value.
     * \param eventName 
     * \param eventParameterName 
     * \param parameterValue 
     */
    virtual void SetEventParameter(
        const std::string& eventName,
        const std::string& eventParameterName,
        float parameterValue) = 0;

    /**
     * \brief Stop all channels.
     */
    virtual void StopAllChannels() = 0;

    /**
     * \brief Set a channel volume.
     * \param channelID 
     * \param volumeDB 
     */
    virtual void SetChannelVolume(int channelID, float volumeDB) = 0;

	/**
	 * \brief Set an event's volume.
	 * \param instance
	 * \param volumeDB
	 */
	virtual void SetEventVolume(FMOD::Studio::EventInstance* instance, float volumeDB) = 0;

	/**
     * \brief Set a channel spatial blend.
     * \param channelID
     * \param spatialBlend
     */
	virtual void SetChannelSpatialBlend(int channelID, float spatialBlend) = 0;

	/**
	 * \brief Set an event's spacial blend.
	 * \param instance
	 * \param spatialBlend
	 */
	virtual void SetEventSpatialBlend(FMOD::Studio::EventInstance* instance, float spatialBlend) = 0;

	/**
     * \brief Set a channel pitch.
     * \param channelID
     * \param pitch
     */
	virtual void SetChannelPitch(int channelID, float pitch) = 0;

	/**
	 * \brief Set an event's pitch.
	 * \param instance
	 * \param pitch
	 */
	virtual void SetEventPitch(FMOD::Studio::EventInstance* instance, float pitch) = 0;

	/**
     * \brief Set a channel mini distance for 3d.
     * \param channelID
     * \param minDistance
     */
	virtual void SetChannelMinDistance(int channelID, float minDistance) = 0;

	/**
	 * \brief Set an event's pitch.
	 * \param instance
	 * \param minDistance
	 */
	virtual void SetEventMinDistance(FMOD::Studio::EventInstance* instance, float minDistance) = 0;

	/**
     * \brief Set a channel mini distance for 3d.
     * \param channelID
     * \param maxDistance
     */
	virtual void SetChannelMaxDistance(int channelID, float maxDistance) = 0;

	/**
	 * \brief Set an event's pitch.
	 * \param instance
	 * \param maxDistance
	 */
	virtual void SetEventMaxDistance(FMOD::Studio::EventInstance* instance, float maxDistance) = 0;

    /**
	 * \brief Set the 3D attribute for a playing event
	 * \param instance
	 * \param position 
	 * \param velocity 
	 * \param forward 
	 * \param up 
	 */
	virtual void SetEvent3DAttributes(
		FMOD::Studio::EventInstance* instance,
		math::Vec3 position = math::Vec3{ 0, 0, 0 },
		math::Vec3 velocity = math::Vec3{ 0, 0, 0 },
		math::Vec3 forward = math::Vec3{ 0, 0, 0 },
		math::Vec3 up = math::Vec3{ 0, 0, 0 }) = 0;

	/**
     * \brief Set a channel its position in the world.
     * \param channelID
     * \param position
     * \param velocity
     */
	virtual void SetChannel3DAttributes(int channelID, math::Vec3 position, math::Vec3 velocity) = 0;

    /**
     * \brief Check if the given channel is playing.
     * \param channelID 
     * \return 
     */
    virtual bool IsPlaying(int channelID) const = 0;

    /**
     * \brief Check if the given event is playing.
     * \param instance 
     * \return 
     */
    virtual bool IsEventPlaying(const FMOD::Studio::EventInstance* instance) const = 0;
};
} //namespace audio
} //namespace poke
