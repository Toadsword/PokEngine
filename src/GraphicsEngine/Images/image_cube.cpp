#include <GraphicsEngine/Images/image_cube.h>

#include <CoreEngine/ServiceLocator/service_locator_definition.h>

namespace poke {
namespace graphics {
ImageCube::ImageCube(
    const std::string& filename,
    const std::string& fileSuffix,
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
		6, 
		{ 0, 0, 1 }),
      filename_(filename),
      fileSuffix_(fileSuffix),
      fileSides_(
          std::vector<std::string>{
              "Right",
              "Left",
              "Top",
              "Bottom",
              "Back",
              "Front"
          }),
      anisotropic_(anisotropic),
      mipmap_(mipmap)
{
    if (load) { Load(); }
}

ImageCube::ImageCube(
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
		6, 
		{ width,height, 1 }),
      filename_(""),
      fileSuffix_(""),
      anisotropic_(anisotropic),
      mipmap_(mipmap),
      width_(width),
      height_(height),
      tmpPixelsContainer_(std::move(pixels))
{
    Load();
}

void ImageCube::Load()
{
    if (!filename_.empty() && tmpPixelsContainer_.empty()) {
        LoadedImageInfos loadedImageInfos = LoadPixels(
            filename_,
            fileSuffix_,
            fileSides_);

        tmpPixelsContainer_ = std::move(loadedImageInfos.pixels);
        width_ = loadedImageInfos.width;
        height_ = loadedImageInfos.height;
        components_ = loadedImageInfos.components;
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
        kNbCubeFace_,
        VK_IMAGE_TYPE_2D);

    sampler_ = CreateImageSampler(filter_, addressMode_, anisotropic_, mipLevels_);

	view_ = CreateImageView(
        image_,
        VK_IMAGE_VIEW_TYPE_CUBE,
        format_,
        VK_IMAGE_ASPECT_COLOR_BIT,
        mipLevels_,
        0,
        arrayLayers_,
        0);

    if (!tmpPixelsContainer_.empty() || mipmap_) {
        TransitionImageLayout(
            image_,
            VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_ASPECT_COLOR_BIT,
            mipLevels_,
            0,
            arrayLayers_,
            0);
    }

    if (!tmpPixelsContainer_.empty()) {
        const VkDeviceSize imageSize = width_ * height_ * components_;

        const auto stagingBuffer = Buffer(
            imageSize * kNbCubeFace_,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        char* pixelsPtr;
        stagingBuffer.MapMemory(&pixelsPtr);
        std::memcpy(
            pixelsPtr,
            tmpPixelsContainer_.data(),
            stagingBuffer.GetSize());
        stagingBuffer.UnmapMemory();

        CopyBufferToImage(
            stagingBuffer.GetBuffer(),
            image_,
            {width_, height_, 1},
            kNbCubeFace_,
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
            kNbCubeFace_,
            0);
    }

    //Clear tmp pixels and resize
    tmpPixelsContainer_.clear();
    tmpPixelsContainer_.resize(0);
}

void ImageCube::SetPixels(
    const uint8_t* pixels,
    const uint32_t layerCount,
    const uint32_t baseArrayLayer) const
{
    const VkDeviceSize imageSize =
        width_ * height_ * components_ * kNbCubeFace_;
    const Buffer stagingBuffer(
        imageSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    char* pixelsPtr;
    stagingBuffer.MapMemory(&pixelsPtr);
    std::memcpy(pixelsPtr, pixels, stagingBuffer.GetSize());
    stagingBuffer.UnmapMemory();

    CopyBufferToImage(
        stagingBuffer.GetBuffer(),
        image_,
        {width_, height_, 1},
        layerCount,
        baseArrayLayer);
}

LoadedImageInfos ImageCube::LoadPixels(
    const std::string& filename,
    const std::string& fileSuffix,
    const std::vector<std::string>& fileSides)
{
    LoadedImageInfos loadedImageInfos;
    uint8_t* offset = nullptr;

    for (const auto& side : fileSides) {
        auto filenameSide = std::string(filename)
                            .append("/").append(side).append(fileSuffix);

        auto loadedImage = LoadPixelsFromFile(filenameSide, FileType::SKYBOX);
        auto resultSide = std::move(loadedImage.pixels);
        loadedImageInfos.width = loadedImage.width;
        loadedImageInfos.height = loadedImage.height;
        loadedImageInfos.components = loadedImage.components;
        const int32_t sizeSide =
            loadedImageInfos.width * loadedImageInfos.height * loadedImageInfos.
            components;

        if (loadedImageInfos.pixels.empty()) {
            loadedImageInfos.pixels = std::vector<char>(
                sizeSide * fileSides.size());
            offset = reinterpret_cast<uint8_t*>(&loadedImageInfos.pixels[0]);
        }

        memcpy(offset, &resultSide[0], sizeSide);
        offset += sizeSide;
    }

    return loadedImageInfos;
}
} //namespace graphics
} //namespace poke
