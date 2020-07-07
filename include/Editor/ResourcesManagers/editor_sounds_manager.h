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
// Date : 03.03.2020
//----------------------------------------------------------------------------------
#pragma once

#include <ResourcesManager/SoundsManagers/core_sounds_manager.h>
#include <json.hpp>

namespace poke {
namespace editor {
class EditorSoundsManager : public CoreSoundsManager {
public:
    //------------------------------ OVERRIDE ---------------------------------
    EditorSoundsManager();
    ~EditorSoundsManager() = default;

    void AddSound(const std::string& soundName) override;
    void AddMusic(const std::string& musicName) override;

    void Clear() override;

    void SetFromJson(const json& soundsJson) override;

    json ToJson() override;
    //-------------------------------------------------------------------------

    //---------------------------- EDITOR SPECIFICS ---------------------------
    /**
     * \brief Get all names of sounds.
     * \return 
     */
    const std::vector<std::string>& GetSoundNames() const;
    /**
     * \brief Get all names of musics.
     * \return 
     */
    const std::vector<std::string>& GetMusicNames() const;

    /**
     * \brief Get a sound's name from an id. If the sounds is not loaded it return "".
     * \param resourceID 
     * \return 
     */
    const std::string GetSoundName(ResourceID resourceID) const;

    /**
     * \brief Get a music's name from an id. If the music is not loaded it return "".
     * \param resourceID 
     * \return 
     */
    const std::string GetMusicName(ResourceID resourceID) const;
    //-------------------------------------------------------------------------
private:
    std::vector<std::string> soundNames_;
    std::vector<std::string> musicNames_;
};
} //namespace editor
} //namespace poke
