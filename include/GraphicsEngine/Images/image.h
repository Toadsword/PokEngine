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
#include <vector>

#include <GraphicsEngine/Descriptors/interface_descriptor.h>

#include <vulkan/vulkan.h>
#include <utility/file_system.h>

namespace poke {
namespace graphics {

class CommandBuffer;

/**
 * \brief Image loaded from a files, contains pixels, width, height, byte per pixels and image format
 */
struct LoadedImageInfos {
    std::vector<char> pixels = {};
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t components = 0;
};

/**
 * \brief Interface to represent an image (use for texture, skybox, depth). This class cannot be used directly
 */
class Image : public IDescriptor {
public:

	/**
	 * \brief Creates a new image object.
	 * \param filter The magnification/minification filter to apply to lookups.
	 * \param addressMode The addressing mode for outside [0..1] range.
	 * \param samples The number of samples per texel.
	 * \param layout The layout that the image subresources accessible from.
	 * \param usage The intended usage of the image.
	 * \param format The format and type of the texel blocks that will be contained in the image.
	 * \param mipLevels The number of levels of detail available for minified sampling of the image.
	 * \param arrayLayers The number of layers in the image.
	 * \param extent The number of data elements in each dimension of the base level.
	 */
	Image(
		VkFilter filter, 
		VkSamplerAddressMode addressMode, 
		VkSampleCountFlagBits samples, 
		VkImageLayout layout,
		VkImageUsageFlags usage,
		VkFormat format, 
		uint32_t mipLevels, 
		uint32_t arrayLayers, 
		const VkExtent3D &extent);

    ~Image();

    static VkDescriptorSetLayoutBinding GetDescriptorSetLayout(
        uint32_t binding,
        VkDescriptorType
        descriptorType,
        VkShaderStageFlags
        stage);

    /**
     * \brief Return a write descriptor for an image
     * \param binding 
     * \param descriptorType 
     * \return 
     */
    WriteDescriptorSet GetWriteDescriptor(
        uint32_t binding,
        VkDescriptorType descriptorType) const
    override;

    VkExtent3D GetExtent() const { return extent_; }

    VkFormat GetFormat() const { return format_; }

    VkSampleCountFlagBits GetSamples() const { return sample_; }

    VkImageUsageFlags GetUsage() const { return usage_; }

    uint32_t GetMipLevels() const { return mipLevels_; }

    int32_t GetArrayLevels() const { return arrayLayers_; }

    VkFilter GetFilter() const { return filter_; }

    VkSamplerAddressMode GetAddressMode() const { return addressMode_; }

    VkImageLayout GetLayout() const { return layout_; }

    const VkImage& GetImage() const { return image_; }

    const VkDeviceMemory& GetMemory() const { return memory_; }

    VkSampler& GetSampler() { return sampler_; }

    const VkImageView& GetView() const { return view_; }

    /**
     * \brief Loads pixel from a file
     * \param filename 
     * \return 
     */
    static LoadedImageInfos LoadPixelsFromFile(
		const std::string& filename,
        FileType fileType
	);

    static uint32_t GetMipLevels(VkExtent3D extent);

    static bool HasDepth(VkFormat format);

    static bool HasStencil(VkFormat format);

    /**
     * \brief Create an image 
     * \param memory 
     * \param extent 
     * \param format 
     * \param samples 
     * \param tiling 
     * \param usage 
     * \param properties 
     * \param mipLevels 
     * \param arrayLayers 
     * \param type 
     */
    static VkImage CreateImage(
        VkDeviceMemory& memory,
        VkExtent3D extent,
        VkFormat format,
        VkSampleCountFlagBits samples,
        VkImageTiling tiling,
        VkImageUsageFlags usage,
        VkMemoryPropertyFlags properties,
        uint32_t mipLevels,
        uint32_t arrayLayers,
        VkImageType type);

    /**
     * \brief Create an image already sampled
     * \param filter 
     * \param addressMode 
     * \param anisotropic 
     * \param mipLevels 
     */
    static VkSampler CreateImageSampler(
        VkFilter filter,
        VkSamplerAddressMode addressMode,
        bool anisotropic,
        uint32_t mipLevels);

    /**
     * \brief Create an image as it's given
     * \param image 
     * \param type 
     * \param format 
     * \param imageAspect 
     * \param mipLevels 
     * \param baseMipLevel 
     * \param layerCount 
     * \param baseArrayLayer 
     */
    static VkImageView CreateImageView(
        const VkImage& image,
        VkImageViewType type,
        VkFormat format,
        VkImageAspectFlags imageAspect,
        uint32_t mipLevels,
        uint32_t baseMipLevel,
        uint32_t layerCount,
        uint32_t baseArrayLayer);

    /**
     * \brief Create different mipmap level for an image
     * \param image 
     * \param extent 
     * \param format 
     * \param dstImageLayout 
     * \param mipLevels 
     * \param baseArrayLayer 
     * \param layerCount 
     */
    static void CreateMipmaps(
        const VkImage& image,
        VkExtent3D extent,
        VkFormat format,
        VkImageLayout dstImageLayout,
        uint32_t mipLevels,
        uint32_t baseArrayLayer,
        uint32_t layerCount);

    /**
     * \brief Change the image layout
     * \param image 
     * \param srcImageLayout 
     * \param dstImageLayout 
     * \param imageAspect 
     * \param mipLevels 
     * \param baseMipLevel 
     * \param layerCount 
     * \param baseArrayLayer 
     */
    static void TransitionImageLayout(
        const VkImage& image,
        VkImageLayout srcImageLayout,
        VkImageLayout dstImageLayout,
        VkImageAspectFlags imageAspect,
        uint32_t mipLevels,
        uint32_t baseMipLevel,
        uint32_t layerCount,
        uint32_t baseArrayLayer);

    /**
     * \brief Create a barrier for the image for a secure usage in the physical device
     * \param commandBuffer 
     * \param image 
     * \param srcAccessMask 
     * \param dstAccessMask 
     * \param oldImageLayout 
     * \param newImageLayout 
     * \param srcStageMask 
     * \param dstStageMask 
     * \param imageAspect 
     * \param mipLevels 
     * \param baseMipLevel 
     * \param layerCount 
     * \param baseArrayLayer 
     */
    static void InsertImageMemoryBarrier(
        const CommandBuffer& commandBuffer,
        const VkImage& image,
        VkAccessFlags srcAccessMask,
        VkAccessFlags dstAccessMask,
        VkImageLayout oldImageLayout,
        VkImageLayout newImageLayout,
        VkPipelineStageFlags srcStageMask,
        VkPipelineStageFlags dstStageMask,
        VkImageAspectFlags imageAspect,
        uint32_t mipLevels,
        uint32_t baseMipLevel,
        uint32_t layerCount,
        uint32_t baseArrayLayer);

    /**
     * \brief Copy a given buffer to an image
     * \param buffer 
     * \param image 
     * \param extent 
     * \param layerCount 
     * \param baseArrayLayer 
     */
    static void CopyBufferToImage(
        const VkBuffer& buffer,
        const VkImage& image,
        VkExtent3D extent,
        uint32_t layerCount,
        uint32_t baseArrayLayer);

    /**
     * \brief Copy an image to a new image. It's mainly use for mipmap images
     * \param srcImage 
     * \param dstImage 
     * \param dstImageMemory 
     * \param srcFormat 
     * \param extent 
     * \param srcImageLayout 
     * \param mipLevel 
     * \param arrayLayer 
     * \return 
     */
    static bool CopyImage(
        const VkImage& srcImage,
        VkImage& dstImage, //TODO(@Nico) Remove out parameter.
        VkDeviceMemory& dstImageMemory, //TODO(@Nico) Remove out parameter.
        VkFormat srcFormat,
        VkExtent3D extent,
        VkImageLayout srcImageLayout,
        uint32_t mipLevel,
        uint32_t arrayLayer);

	/**
	 * \brief Find a format in the candidates list that fits the tiling and features required.
	 * \param candidates Formats that are tested for features, in order of preference.
	 * \param tiling Tiling mode to test features in.
	 * \param features The features to test for.
	 * \return The format found, or VK_FORMAT_UNDEFINED.
	 */
	static VkFormat FindSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

protected:
    VkExtent3D extent_;
    VkFormat format_;
    VkSampleCountFlagBits sample_;
    VkImageUsageFlags usage_;
    uint32_t mipLevels_;
    uint32_t arrayLayers_;

    VkFilter filter_;
    VkSamplerAddressMode addressMode_;

    VkImageLayout layout_;

    VkImage image_ = VK_NULL_HANDLE;
    VkDeviceMemory memory_ = VK_NULL_HANDLE;
    VkSampler sampler_ = VK_NULL_HANDLE;
    VkImageView view_ = VK_NULL_HANDLE;

	inline static const float kAnisotropy = 16.0f;

    inline static const std::vector<VkFormat> kDepthFormat = {
        VK_FORMAT_D16_UNORM,
        VK_FORMAT_X8_D24_UNORM_PACK32,
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D16_UNORM_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT,
        VK_FORMAT_D32_SFLOAT_S8_UINT
    };

    inline static const std::vector<VkFormat> kStencilFormats = {
        VK_FORMAT_S8_UINT,
        VK_FORMAT_D16_UNORM_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT,
        VK_FORMAT_D32_SFLOAT_S8_UINT
    };
};
} //namespace graphics
} //namespace poke
