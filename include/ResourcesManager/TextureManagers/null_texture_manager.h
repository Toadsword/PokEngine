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
// Date : 30.01.20
//----------------------------------------------------------------------------------
#pragma once
#include <ResourcesManager/TextureManagers/interface_texture_manager.h>
#include <CoreEngine/cassert.h>

namespace poke {
class NullTextureManager final : public ITextureManager {
public:
    void AddTexture2D(const std::string& texturePath) override
    {
		texturePath;
    }

    const graphics::Image2d& GetTexture2DByID(ResourceID resourceID) const
    override
    {
		resourceID;
		cassert(false, "Impossible to access to NulLTextureManager");
		abort();
    }

    const graphics::Image2d& GetTexture2DByName(
        const std::string& texturePath) const override
    {
		texturePath;
		cassert(false, "Impossible to access to NulLTextureManager");
		abort();
    }

    void AddTextureCube(const std::string& texturePath) override
    {
		texturePath;
    }

    const graphics::ImageCube& GetTextureCubeByID(ResourceID resourceID) const
    override
    {
		resourceID;
		cassert(false, "Impossible to access to NulLTextureManager");
		abort();
    }

    const graphics::ImageCube& GetTextureCubeByName(
        const std::string& texturePath) const override
    {
		texturePath;
		cassert(false, "Impossible to access to NulLTextureManager");
		abort();
    }

    void Clear() override {}

    void Resize(size_t newSize) override
    {
		newSize;
    }

	void SetFromJson(const json& texturesJson) override
    {
		texturesJson;
    }
	json ToJson() override { return json(); }
};
} //namespace poke