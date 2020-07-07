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
#include <ResourcesManager/TextureManagers/core_textures_manager.h>

namespace poke {
namespace editor {
class EditorTexturesManager final : public CoreTexturesManager {
public:
    //------------------------------ OVERRIDE ---------------------------------
    explicit EditorTexturesManager();
    virtual ~EditorTexturesManager() = default;

    void AddTexture2D(const std::string& texturePath) override;

    void AddTextureCube(const std::string& texturePath) override;

    void Clear() override;

    void SetFromJson(const json& texturesJson) override;

    json ToJson() override;
    //-------------------------------------------------------------------------

    //---------------------------- EDITOR SPECIFICS ---------------------------
    /**
     * \brief Get all names of all image 2d loaded.
     * \return 
     */
    const std::vector<std::string>& GetImage2dNames() const;

    /**
     * \brief Get names of all image cube loaded.
     * \return 
     */
    const std::vector<std::string>& GetImageCubeNames() const;

    /**
     * \brief Get an imaged2's name from an id. If the image is not loaded it return "".
     * \param resourceID 
     * \return 
     */
    const std::string GetImage2dName(ResourceID resourceID) const;

    /**
     * \brief Get an ImageCube's name from an id. If the image is not loaded it return "".
     * \param resourceID 
     * \return 
     */
    const std::string GetImageCubeName(ResourceID resourceID) const;
    //-------------------------------------------------------------------------
private:
    std::vector<std::string> image2dNames_;
    std::vector<std::string> imageCubeNames_;
};
} //namespace editor
} //namespace poke
