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
#include <string>

#include <ResourcesManager/resource_type.h>

namespace poke {
namespace graphics {
class Image2d;
class ImageCube;
}

class ITextureManager {
public:
    virtual ~ITextureManager() = default;

    /**
     * \brief Create a texture 2D, it's slower that using directly the hash.
     * \param texturePath It must be the full name of the texture with the extension.
     */
    virtual void AddTexture2D(const std::string& texturePath) = 0;

    /**
     * \brief Get a texture by it's hash value.
     * \param resourceID
     * \return
     */
    virtual const graphics::Image2d& GetTexture2DByID(
		ResourceID resourceID) const = 0;

    /**
     * \brief Get a texture by it's path
     * \param texturePath It's the full path with the extension
     * \return
     */
    virtual const graphics::Image2d& GetTexture2DByName(
        const std::string& texturePath) const = 0;

    /**
     * \brief Add an TextureCube in the manager
     * \param texturePath It must be only the name of the folder containing all textures.
     */
    virtual void AddTextureCube(const std::string& texturePath) = 0;

    /**
     * \brief Get an TextureCube by its hash.
     * \param resourceID
     * \return
     */
    virtual const graphics::ImageCube& GetTextureCubeByID(
		ResourceID resourceID) const = 0;

    /**
     * \brief Get an TextureCube by its name. This function is slower than using directly a hash.
     * \param texturePath
     * \return
     */
    virtual const graphics::ImageCube& GetTextureCubeByName(
        const std::string& texturePath) const = 0;

    /**
	 * \brief Delete all texture registered.
	 */
    virtual void Clear() = 0;

    /**
	 * \brief Resize the manager to the given size. If the size is smaller than the actual size and some texture are still in, they will be erased.
	 * \param newSize must be greater than 0
	 */
    virtual void Resize(size_t newSize) = 0;

	virtual void SetFromJson(const json& texturesJson) = 0;
	virtual json ToJson() = 0;
};
} //namespace poke
