//-----------------------------------------------------------------------------
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
// Date : 29.01.20
//-----------------------------------------------------------------------------
#pragma once

#include <string>

#include <Math/vector.h>
#include <GraphicsEngine/Images/image.h>

namespace poke {
namespace graphics {

/**
 * \brief Represents a texture
 */
class Image2d : public Image {
public:
    /**
     * \brief 
     * \param filename 
     * \param filter 
     * \param addressMode 
     * \param anisotropic 
     * \param mipmap 
     * \param load 
     */
    explicit Image2d(
        const std::string& filename,
        VkFilter filter = VK_FILTER_LINEAR,
        VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        bool anisotropic = true,
        bool mipmap = true,
        bool load = true);

    /**
     * \brief 
     * \param width 
     * \param height 
     * \param pixels 
     * \param format 
     * \param layout 
     * \param usage 
     * \param filter 
     * \param addressMode 
     * \param samples 
     * \param anisotropic 
     * \param mipmap 
     */
    Image2d(
        uint32_t width,
        uint32_t height,
        std::vector<char> pixels = {},
        VkFormat format = VK_FORMAT_R8G8B8A8_UNORM,
        VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        VkImageUsageFlags usage =
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
            VK_IMAGE_USAGE_STORAGE_BIT,
        VkFilter filter = VK_FILTER_LINEAR,
        VkSamplerAddressMode addressMode =
            VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT,
        bool anisotropic = false,
        bool mipmap = false);

	/**
	 * Creates a new 2D image.
	 * \param extent The images extent in pixels.
	 * \param format The format and type of the texel blocks that will be contained in the image.
	 * \param layout The layout that the image subresources accessible from.
	 * \param usage The intended usage of the image.
	 * \param filter The magnification/minification filter to apply to lookups.
	 * \param addressMode The addressing mode for outside [0..1] range.
	 * \param samples The number of samples per texel.
	 * \param anisotropic If anisotropic filtering is enabled.
	 * \param mipmap If mapmaps will be generated.
	 */
    explicit Image2d(
		math::Vec2Int extent, 
		VkFormat format = VK_FORMAT_R8G8B8A8_UNORM, 
		VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		VkImageUsageFlags usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT,
		VkFilter filter = VK_FILTER_LINEAR, 
		VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT, 
		bool anisotropic = false, 
		bool mipmap = false);

	~Image2d() = default;

    /**
     * \brief Load the image using an external image
     */
    void Load();

    const std::string& GetFilename() const { return filename_; }

    bool IsMipmap() const { return mipmap_; }

    uint32_t GetComponents() const { return components_; }

    uint32_t GetWidth() const { return width_; }

    uint32_t GetHeight() const { return height_; }

    const VkSampler& GetSampler() const { return sampler_; }

	bool IsAnisotropic() const { return anisotropic_; }

private:
    std::string filename_ = {};

    bool anisotropic_;
    bool mipmap_;

    uint32_t components_ = 0;
    uint32_t width_ = 0;
    uint32_t height_ = 0;
    uint32_t mipLevels_ = 0;
    std::vector<char> tmpPixelsContainer_;
};
} //namespace graphics
} //namespace poke
