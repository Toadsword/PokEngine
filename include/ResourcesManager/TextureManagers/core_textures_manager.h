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
// Date : 12.02.20
//----------------------------------------------------------------------------------
#pragma once

#include <GraphicsEngine/Images/image_2d.h>
#include <GraphicsEngine/Images/image_cube.h>
#include <ResourcesManager/TextureManagers/interface_texture_manager.h>

namespace poke {
/**
 * \brief Manages Image2D and ImageCube objects.
 */
class CoreTexturesManager : public ITextureManager{
public:
    /**
     * \brief 
     */
    explicit CoreTexturesManager();
    virtual ~CoreTexturesManager() = default;

    /**
     * \brief Create a texture 2D
     * \param texturePath 
     */
    void AddTexture2D(const std::string& texturePath) override;

    /**
     * \brief Get a texture by it's hash value
     * \param resourceID 
     * \return 
     */
    const graphics::Image2d& GetTexture2DByID(ResourceID resourceID) const override;

    /**
     * \brief Get a texture by it's path
     * \param texturePath 
     * \return 
     */
    const graphics::Image2d& GetTexture2DByName(
        const std::string& texturePath) const override;

    /**
     * \brief Create an TextureCube.
     * \param texturePath 
     */
    void AddTextureCube(const std::string& texturePath) override;

    /**
     * \brief Get an TextureCube by its hash.
     * \param resourceID 
     * \return 
     */
    const graphics::ImageCube& GetTextureCubeByID(ResourceID resourceID) const override;

    /**
     * \brief Get an TextureCube by its name. 
     * \param texturePath 
     * \return 
     */
    const graphics::ImageCube& GetTextureCubeByName(
        const std::string& texturePath) const override;

    void Clear() override;

    void Resize(size_t newSize) override;

    void SetFromJson(const json& texturesJson) override;

    json ToJson() override;

protected:
	std::vector<ResourceID> image2dIDs_;
    //TODO(@Nico) remove unique_ptr
	std::vector<std::unique_ptr<graphics::Image2d>> image2ds_;

	size_t vectorEnd_ = 0;

	std::vector<ResourceID> imageCubeIDs_;
	std::vector<graphics::ImageCube> imagesCubes_;

	inline static const size_t kImageDefaultSize = 200;
	inline static const size_t kImageCubeDefaultSize = 2;
};
} //namespace poke
