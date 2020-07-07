#include <GraphicsEngine/Images/image_2d.h>

#include <CoreEngine/ServiceLocator/service_locator_definition.h>

namespace poke {
namespace graphics {
Image2d::Image2d(
    const std::string& filename,
    const VkFilter filter,
    const VkSamplerAddressMode addressMode,
    const bool anisotropic,
    const bool mipmap,
    const bool load)
    : Image(
		filter, 
		addressMode, 
		VK_SAMPLE_COUNT_1_BIT, 
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		VK_FORMAT_R8G8B8A8_UNORM, 
		1, 
		1, 
		{ 0, 0, 1 }),
      filename_(filename),
      anisotropic_(anisotropic),
      mipmap_(mipmap)
{
    if (load) { Load(); }
}

Image2d::Image2d(
    const uint32_t width,
    const uint32_t height,
    std::vector<char> pixels,
    const VkFormat format,
    const VkImageLayout layout,
    const VkImageUsageFlags usage,
    const VkFilter filter,
    const VkSamplerAddressMode addressMode,
    const VkSampleCountFlagBits samples,
    const bool anisotropic,
    const bool mipmap)
    : Image(
		filter, 
		addressMode, 
		samples, 
		layout,
		usage | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		format, 
		1, 
		1, 
		{ width, height, 1 }),
      anisotropic_(anisotropic),
      mipmap_(mipmap),
      components_(4),
      width_(width),
      height_(height),
      tmpPixelsContainer_(std::move(pixels))
{
    Load();
}

Image2d::Image2d(
    const math::Vec2Int extent,
    const VkFormat format,
    const VkImageLayout layout,
    const VkImageUsageFlags usage,
    const VkFilter filter,
    const VkSamplerAddressMode addressMode,
    const VkSampleCountFlagBits samples,
    const bool anisotropic,
    const bool mipmap) :
	Image(filter, 
		addressMode, 
		samples, 
		layout,
		usage | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		format, 
		1, 
		1, 
		{ static_cast<uint32_t>(extent.x), static_cast<uint32_t>(extent.y), 1 }),
	anisotropic_(anisotropic),
	mipmap_(mipmap),
	components_(4),
	width_(extent.x),
	height_(extent.y),
	tmpPixelsContainer_({})
{
	Load();
}

void Image2d::Load()
{
    if (!filename_.empty() && tmpPixelsContainer_.empty()) {
        auto loadedImage = LoadPixelsFromFile(filename_, FileType::TEXTURE);
        tmpPixelsContainer_ = std::move(loadedImage.pixels);
        width_ = loadedImage.width;
        height_ = loadedImage.height;
        components_ = loadedImage.components;
    }

    if (width_ == 0 && height_ == 0) { return; }

    mipLevels_ = mipmap_ ? GetMipLevels({width_, height_, 1}) : 1;

    image_ = CreateImage(
        memory_,
        {width_, height_, 1},
        format_,
        sample_,
        VK_IMAGE_TILING_OPTIMAL,
        usage_,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        mipLevels_,
        1,
        VK_IMAGE_TYPE_2D);

    sampler_ = CreateImageSampler(
        filter_,
        addressMode_,
        anisotropic_,
        mipLevels_);

	view_ = CreateImageView(
        image_,
        VK_IMAGE_VIEW_TYPE_2D,
        format_,
        VK_IMAGE_ASPECT_COLOR_BIT,
        mipLevels_,
        0,
        1,
        0);

    if (!tmpPixelsContainer_.empty() || mipmap_) {
        TransitionImageLayout(
            image_,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_ASPECT_COLOR_BIT,
            mipLevels_,
            0,
            1,
            0);
    }

    if (!tmpPixelsContainer_.empty()) {
        const VkDeviceSize imageSize = width_ * height_ * components_;

        const Buffer stagingBuffer(
            imageSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        char* data;
        stagingBuffer.MapMemory(&data);
        std::memcpy(data, tmpPixelsContainer_.data(), stagingBuffer.GetSize());
        stagingBuffer.UnmapMemory();

        CopyBufferToImage(
            stagingBuffer.GetBuffer(),
            image_,
            {width_, height_, 1},
            1,
            0);
    }

    if (mipmap_) {
        CreateMipmaps(
            image_,
            {width_, height_, 1},
            format_,
            layout_,
            mipLevels_,
            0,
			arrayLayers_);
    } else if (!tmpPixelsContainer_.empty()) {
        TransitionImageLayout(
            image_,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            layout_,
            VK_IMAGE_ASPECT_COLOR_BIT,
            mipLevels_,
            0,
			arrayLayers_,
            0);
    } else {
        TransitionImageLayout(
            image_,
            VK_IMAGE_LAYOUT_UNDEFINED,
            layout_,
            VK_IMAGE_ASPECT_COLOR_BIT,
            mipLevels_,
            0,
			arrayLayers_,
            0);
    }

    tmpPixelsContainer_.clear();
    tmpPixelsContainer_.resize(0);
}
} //namespace graphics
} //namespace poke
