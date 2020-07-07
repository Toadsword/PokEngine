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
// Date : 19.02.20
//----------------------------------------------------------------------------------
#pragma once
#include <string>

#include <ResourcesManager/SoundsManagers/music.h>
#include <ResourcesManager/resource_type.h>

namespace poke {
class ISoundsManager {
public:
    /**
     * \brief Add a sound from a path
     * \param soundPath
     */
    virtual void AddSound(const std::string& soundPath) = 0;
    /**
     * \brief Add a music from a pth
     * \param musicPath
     */
    virtual void AddMusic(const std::string& musicPath) = 0;

    /**
     * \brief Get a sound from its ID
     * \param resourceID
     * \return
     */
    virtual const audio::AudioClip& GetAudioClip(ResourceID resourceID) const = 0;
    /**
     * \brief Get a sound from its path. This function is slower than the version with the hash.
     * \param soundPath
     * \return
     */
    virtual const audio::AudioClip& GetAudioClipByName(
        const std::string& soundPath) const = 0;

    /**
     * \brief Clear all clip registered
     */
    virtual void Clear() = 0;

    /**
     * \brief Resize the manager to a new size. Doesn't erase data.
     * \param newSize
     */
    virtual void Resize(size_t newSize) = 0;

    virtual void SetFromJson(const json& soundsJson) = 0;

    virtual json ToJson() = 0;

    virtual void LoadDefaultSound() = 0;
};
} //namespace poke
