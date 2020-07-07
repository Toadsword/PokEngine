#include <GraphicsEngine/Images/image.h>

#include <iostream>
#include <algorithm>

#include <GraphicsEngine/vulkan_error_handler.h>
#include <Utility/file_system.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace poke {
namespace graphics {
Image::Image(
    const VkFilter filter,
    const VkSamplerAddressMode addressMode,
    const VkSampleCountFlagBits samples,
    const VkImageLayout layout,
    const VkImageUsageFlags usage,
    const VkFormat format,
    const uint32_t mipLevels,
    const uint32_t arrayLayers,
    const VkExtent3D& extent)
    : extent_(extent),
      format_(format),
      sample_(samples),
      usage_(usage),
      mipLevels_(mipLevels),
      arrayLayers_(arrayLayers),
      filter_(filter),
      addressMode_(addressMode),
      layout_(layout) {}

Image::~Image()
{
    const auto& logicalDevice = GraphicsEngineLocator::Get().GetLogicalDevice();

    vkDestroyImageView(logicalDevice, view_, nullptr);
    vkDestroySampler(logicalDevice, sampler_, nullptr);
    vkFreeMemory(logicalDevice, memory_, nullptr);
    vkDestroyImage(logicalDevice, image_, nullptr);
}

VkDescriptorSetLayoutBinding Image::GetDescriptorSetLayout(
    const uint32_t binding,
    const VkDescriptorType descriptorType,
    const VkShaderStageFlags stage)
{
    VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
    descriptorSetLayoutBinding.binding = binding;
    descriptorSetLayoutBinding.descriptorType = descriptorType;
    descriptorSetLayoutBinding.descriptorCount = 1;
    descriptorSetLayoutBinding.stageFlags = stage;
    descriptorSetLayoutBinding.pImmutableSamplers = nullptr;

    return descriptorSetLayoutBinding;
}

WriteDescriptorSet Image::GetWriteDescriptor(
    const uint32_t binding,
    const VkDescriptorType descriptorType) const
{
    VkDescriptorImageInfo imageInfo;
    imageInfo.sampler = sampler_;
    imageInfo.imageView = view_;
    imageInfo.imageLayout = layout_;

    VkWriteDescriptorSet descriptorWrite = {};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = VK_NULL_HANDLE;
    descriptorWrite.dstBinding = binding;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.descriptorType = descriptorType;

    return WriteDescriptorSet(descriptorWrite, imageInfo);
}

LoadedImageInfos Image::LoadPixelsFromFile(const std::string& filename, const FileType fileType)
{
    auto file = PokFileSystem::ReadFile(filename, fileType);

    LoadedImageInfos loadedPixelInfo;

    cassert(!file.empty(), "image " << filename << " could not be loaded");

    std::unique_ptr<uint8_t> stbiData =
        std::unique_ptr<uint8_t>(
            stbi_load_from_memory(
                reinterpret_cast<stbi_uc const*>(file.data()),
                static_cast<uint32_t>(file.size()),
                reinterpret_cast<int32_t*>(&loadedPixelInfo.width),
                reinterpret_cast<int32_t*>(&loadedPixelInfo.height),
                reinterpret_cast<int32_t*>(&loadedPixelInfo.components),
                STBI_rgb_alpha));


    loadedPixelInfo.components = 4;
    loadedPixelInfo.pixels.resize(
        loadedPixelInfo.width * loadedPixelInfo.height
        * loadedPixelInfo.components);

    std::memcpy(
        loadedPixelInfo.pixels.data(),
        stbiData.get(),
        loadedPixelInfo.pixels.size());

    if (loadedPixelInfo.pixels.empty()) { std::cout << "Unable to load image\n"; }

    return loadedPixelInfo;
}

uint32_t Image::GetMipLevels(const VkExtent3D extent)
{
    return static_cast<uint32_t>(
		floor(log2(std::max(extent.width,std::max(extent.height, extent.depth)))) + 1);
}

bool Image::HasDepth(const VkFormat format)
{
    return std::find(kDepthFormat.begin(), kDepthFormat.end(), format) != std::
           end(kDepthFormat);
}

bool Image::HasStencil(const VkFormat format)
{
    return std::find(kStencilFormats.begin(), kStencilFormats.end(), format) !=
           std::end(kStencilFormats);
}

VkImage Image::CreateImage(
    VkDeviceMemory& memory,
    const VkExtent3D extent,
    const VkFormat format,
    const VkSampleCountFlagBits samples,
    const VkImageTiling tiling,
    const VkImageUsageFlags usage,
    const VkMemoryPropertyFlags properties,
    const uint32_t mipLevels,
    const uint32_t arrayLayers,
    const VkImageType type)
{
    const auto& logicalDevice = GraphicsEngineLocator::Get().GetLogicalDevice();

    VkImageCreateInfo imageCreateInfo = {};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.flags = arrayLayers == 6 ?
                                VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT :
                                0;
    imageCreateInfo.imageType = type;
    imageCreateInfo.format = format;
    imageCreateInfo.extent = extent;
    imageCreateInfo.mipLevels = mipLevels;
    imageCreateInfo.arrayLayers = arrayLayers;
    imageCreateInfo.samples = samples;
    imageCreateInfo.tiling = tiling;
    imageCreateInfo.usage = usage;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    VkImage image;
    CheckVk(vkCreateImage(logicalDevice, &imageCreateInfo, nullptr, &image));

    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(logicalDevice, image, &memoryRequirements);

    VkMemoryAllocateInfo memoryAllocateInfo = {};
    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.allocationSize = memoryRequirements.size;
    memoryAllocateInfo.memoryTypeIndex = Buffer::FindMemoryType(
        memoryRequirements
        .memoryTypeBits,
        properties);
    CheckVk(
        vkAllocateMemory(
            logicalDevice,
            &memoryAllocateInfo,
            nullptr,
            &memory));

    CheckVk(vkBindImageMemory(logicalDevice, image, memory, 0));

    return image;
}

VkSampler Image::CreateImageSampler(
    const VkFilter filter,
    const VkSamplerAddressMode addressMode,
    const bool anisotropic,
    const uint32_t mipLevels)
{
    const auto& physicalDevice = GraphicsEngineLocator::Get().GetPhysicalDevice();
    const auto& logicalDevice = GraphicsEngineLocator::Get().GetLogicalDevice();

    VkSamplerCreateInfo samplerCreateInfo = {};
    samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerCreateInfo.magFilter = filter;
    samplerCreateInfo.minFilter = filter;
    samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerCreateInfo.addressModeU = addressMode;
    samplerCreateInfo.addressModeV = addressMode;
    samplerCreateInfo.addressModeW = addressMode;
    samplerCreateInfo.mipLodBias = 0.0f;
    samplerCreateInfo.anisotropyEnable = static_cast<VkBool32>(anisotropic);
    samplerCreateInfo.maxAnisotropy =
        (anisotropic && logicalDevice.GetEnabledFeatures().samplerAnisotropy) ?
            std::min(
                kAnisotropy,
                physicalDevice
                .GetProperties().limits.maxSamplerAnisotropy) :
            1.0f;
    samplerCreateInfo.minLod = 0.0f;
    samplerCreateInfo.maxLod = static_cast<float>(mipLevels);
    samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
    samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;

    VkSampler sampler;
    CheckVk(
        vkCreateSampler(
            logicalDevice,
            &samplerCreateInfo,
            nullptr,
            &sampler));

    return sampler;
}

VkImageView Image::CreateImageView(
    const VkImage& image,
    const VkImageViewType type,
    const VkFormat format,
    const VkImageAspectFlags imageAspect,
    const uint32_t mipLevels,
    const uint32_t baseMipLevel,
    const uint32_t layerCount,
    const uint32_t baseArrayLayer)
{
	VkImageView imageView;
    const auto& logicalDevice = GraphicsEngineLocator::Get().GetLogicalDevice();

    VkImageViewCreateInfo imageViewCreateInfo = {};
    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.image = image;
    imageViewCreateInfo.viewType = type;
    imageViewCreateInfo.format = format;
    imageViewCreateInfo.components = {
        VK_COMPONENT_SWIZZLE_R,
        VK_COMPONENT_SWIZZLE_G,
        VK_COMPONENT_SWIZZLE_B,
        VK_COMPONENT_SWIZZLE_A
    };
    imageViewCreateInfo.subresourceRange.aspectMask = imageAspect;
    imageViewCreateInfo.subresourceRange.baseMipLevel = baseMipLevel;
    imageViewCreateInfo.subresourceRange.levelCount = mipLevels;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = baseArrayLayer;
    imageViewCreateInfo.subresourceRange.layerCount = layerCount;

    CheckVk(
        vkCreateImageView(
            logicalDevice,
            &imageViewCreateInfo,
            nullptr,
            &imageView));

    return imageView;
}

void Image::CreateMipmaps(
    const VkImage& image,
    VkExtent3D extent,
    VkFormat format,
    VkImageLayout dstImageLayout,
    uint32_t mipLevels,
    uint32_t baseArrayLayer,
    uint32_t layerCount)
{
    const auto& physicalDevice = GraphicsEngineLocator::Get().GetPhysicalDevice();

    // Get device properties for the requested texture format.
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(
        physicalDevice,
        format,
        &formatProperties);

    // Mip-chain generation requires support for blit source and destination
    cassert(
        formatProperties.optimalTilingFeatures ||
        VK_FORMAT_FEATURE_BLIT_SRC_BIT,
        "Vulkan error: hardware don't support blit");
    cassert(
        formatProperties.optimalTilingFeatures ||
        VK_FORMAT_FEATURE_BLIT_DST_BIT,
        "Vulkan error: hardware don't support blit");

    CommandBuffer commandBuffer = CommandBuffer();

    for (uint32_t i = 1; i < mipLevels; i++) {
        VkImageMemoryBarrier barrier0 = {};
        barrier0.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier0.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier0.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier0.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier0.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier0.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier0.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier0.image = image;
        barrier0.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier0.subresourceRange.baseMipLevel = i - 1;
        barrier0.subresourceRange.levelCount = 1;
        barrier0.subresourceRange.baseArrayLayer = baseArrayLayer;
        barrier0.subresourceRange.layerCount = layerCount;
        vkCmdPipelineBarrier(
            commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            0,
            0,
            nullptr,
            0,
            nullptr,
            1,
            &barrier0);

        VkImageBlit imageBlit = {};
        imageBlit.srcOffsets[1] = {
            int32_t(extent.width >> (i - 1)),
            int32_t(extent.height >> (i - 1)),
            1
        };
        imageBlit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageBlit.srcSubresource.mipLevel = i - 1;
        imageBlit.srcSubresource.baseArrayLayer = baseArrayLayer;
        imageBlit.srcSubresource.layerCount = layerCount;
        imageBlit.dstOffsets[1] = {
            int32_t(extent.width >> i),
            int32_t(extent.height >> i),
            1
        };
        imageBlit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageBlit.dstSubresource.mipLevel = i;
        imageBlit.dstSubresource.baseArrayLayer = baseArrayLayer;
        imageBlit.dstSubresource.layerCount = layerCount;
        vkCmdBlitImage(
            commandBuffer,
            image,
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &imageBlit,
            VK_FILTER_LINEAR);

        VkImageMemoryBarrier barrier1 = {};
        barrier1.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier1.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier1.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        barrier1.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier1.newLayout = dstImageLayout;
        barrier1.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier1.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier1.image = image;
        barrier1.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier1.subresourceRange.baseMipLevel = i - 1;
        barrier1.subresourceRange.levelCount = 1;
        barrier1.subresourceRange.baseArrayLayer = baseArrayLayer;
        barrier1.subresourceRange.layerCount = layerCount;
        vkCmdPipelineBarrier(
            commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            0,
            0,
            nullptr,
            0,
            nullptr,
            1,
            &barrier1);
    }

    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = dstImageLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = mipLevels - 1;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = baseArrayLayer;
    barrier.subresourceRange.layerCount = layerCount;
    vkCmdPipelineBarrier(
        commandBuffer,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        0,
        0,
        nullptr,
        0,
        nullptr,
        1,
        &barrier);

    commandBuffer.SubmitIdle();
}

void Image::TransitionImageLayout(
    const VkImage& image,
    const VkImageLayout srcImageLayout,
    const VkImageLayout dstImageLayout,
    const VkImageAspectFlags imageAspect,
    const uint32_t mipLevels,
    const uint32_t baseMipLevel,
    const uint32_t layerCount,
    const uint32_t baseArrayLayer)
{
    CommandBuffer commandBuffer = CommandBuffer();

    VkImageMemoryBarrier imageMemoryBarrier = {};
    imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageMemoryBarrier.oldLayout = srcImageLayout;
    imageMemoryBarrier.newLayout = dstImageLayout;
    imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.image = image;
    imageMemoryBarrier.subresourceRange.aspectMask = imageAspect;
    imageMemoryBarrier.subresourceRange.baseMipLevel = baseMipLevel;
    imageMemoryBarrier.subresourceRange.levelCount = mipLevels;
    imageMemoryBarrier.subresourceRange.baseArrayLayer = baseArrayLayer;
    imageMemoryBarrier.subresourceRange.layerCount = layerCount;

    // Source access mask controls actions that have to be finished on the old layout before it will be transitioned to the new layout.
    switch (srcImageLayout) {
    case VK_IMAGE_LAYOUT_UNDEFINED:
        imageMemoryBarrier.srcAccessMask = 0;
        break;
    case VK_IMAGE_LAYOUT_PREINITIALIZED:
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
        break;
    case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
        imageMemoryBarrier.srcAccessMask =
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        break;
    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
        imageMemoryBarrier.srcAccessMask =
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        break;
    case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        break;
    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        break;
    case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
        break;
    default:
        break;
    }

    // Destination access mask controls the dependency for the new image layout.
    switch (dstImageLayout) {
    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        break;
    case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        break;
    case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
        imageMemoryBarrier.dstAccessMask =
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        break;
    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
        imageMemoryBarrier.dstAccessMask =
            imageMemoryBarrier.dstAccessMask |
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        break;
    case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
        if (imageMemoryBarrier.srcAccessMask == 0) {
            imageMemoryBarrier.srcAccessMask =
                VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
        }

        imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        break;
    default:
        break;
    }

    const VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    const VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

    vkCmdPipelineBarrier(
        commandBuffer,
        srcStageMask,
        dstStageMask,
        0,
        0,
        nullptr,
        0,
        nullptr,
        1,
        &imageMemoryBarrier);

    commandBuffer.SubmitIdle();
}

void Image::InsertImageMemoryBarrier(
    const CommandBuffer& commandBuffer,
    const VkImage& image,
    const VkAccessFlags srcAccessMask,
    const VkAccessFlags dstAccessMask,
    const VkImageLayout oldImageLayout,
    const VkImageLayout newImageLayout,
    const VkPipelineStageFlags srcStageMask,
    const VkPipelineStageFlags dstStageMask,
    const VkImageAspectFlags imageAspect,
    const uint32_t mipLevels,
    const uint32_t baseMipLevel,
    const uint32_t layerCount,
    const uint32_t baseArrayLayer)
{
    VkImageMemoryBarrier imageMemoryBarrier = {};
    imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageMemoryBarrier.srcAccessMask = srcAccessMask;
    imageMemoryBarrier.dstAccessMask = dstAccessMask;
    imageMemoryBarrier.oldLayout = oldImageLayout;
    imageMemoryBarrier.newLayout = newImageLayout;
    imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.image = image;
    imageMemoryBarrier.subresourceRange.aspectMask = imageAspect;
    imageMemoryBarrier.subresourceRange.baseMipLevel = baseMipLevel;
    imageMemoryBarrier.subresourceRange.levelCount = mipLevels;
    imageMemoryBarrier.subresourceRange.baseArrayLayer = baseArrayLayer;
    imageMemoryBarrier.subresourceRange.layerCount = layerCount;
    vkCmdPipelineBarrier(
        commandBuffer,
        srcStageMask,
        dstStageMask,
        0,
        0,
        nullptr,
        0,
        nullptr,
        1,
        &imageMemoryBarrier);
}

void Image::CopyBufferToImage(
    const VkBuffer& buffer,
    const VkImage& image,
    const VkExtent3D extent,
    const uint32_t layerCount,
    const uint32_t baseArrayLayer)
{
    auto commandBuffer = CommandBuffer();

    VkBufferImageCopy region = {};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = baseArrayLayer;
    region.imageSubresource.layerCount = layerCount;
    region.imageOffset = {0, 0, 0};
    region.imageExtent = extent;
    vkCmdCopyBufferToImage(
        commandBuffer,
        buffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region);

    commandBuffer.SubmitIdle();
}

bool Image::CopyImage(
    const VkImage& srcImage,
    VkImage& dstImage,
    VkDeviceMemory& dstImageMemory,
    const VkFormat srcFormat,
    const VkExtent3D extent,
    const VkImageLayout srcImageLayout,
    const uint32_t mipLevel,
    const uint32_t arrayLayer)
{
    const auto& physicalDevice = GraphicsEngineLocator::Get().GetPhysicalDevice();
    const auto& surface = GraphicsEngineLocator::Get().GetSurface();

    // Checks blit swapchain support.
    bool supportsBlit = true;
    VkFormatProperties formatProperties;

    // Check if the device supports blitting from optimal images (the swapchain images are in optimal format).
    vkGetPhysicalDeviceFormatProperties(
        physicalDevice,
        surface.GetFormat().format,
        &formatProperties);

    if (!(formatProperties.optimalTilingFeatures &
          VK_FORMAT_FEATURE_BLIT_SRC_BIT)) {
        std::cout <<
            "Device does not support blitting from optimal tiled images, using copy instead of blit!\n";
        supportsBlit = false;
    }

    // Check if the device supports blitting to linear images.
    vkGetPhysicalDeviceFormatProperties(
        physicalDevice,
        srcFormat,
        &formatProperties);

    if (!(formatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT)) {
        std::cout <<
            "Device does not support blitting to linear tiled images, using copy instead of blit!\n";
        supportsBlit = false;
    }

    dstImage = CreateImage(
        dstImageMemory,
        extent,
        VK_FORMAT_R8G8B8A8_UNORM,
        VK_SAMPLE_COUNT_1_BIT,
        VK_IMAGE_TILING_LINEAR,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT |
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        1,
        1,
        VK_IMAGE_TYPE_2D);

    // Do the actual blit from the swapchain image to our host visible destination image.
    CommandBuffer commandBuffer = CommandBuffer();

    // Transition destination image to transfer destination layout.
    InsertImageMemoryBarrier(
        commandBuffer,
        dstImage,
        0,
        VK_ACCESS_TRANSFER_WRITE_BIT,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_IMAGE_ASPECT_COLOR_BIT,
        1,
        0,
        1,
        0);

    // Transition image from previous usage to transfer source layout
    InsertImageMemoryBarrier(
        commandBuffer,
        srcImage,
        VK_ACCESS_MEMORY_READ_BIT,
        VK_ACCESS_TRANSFER_READ_BIT,
        srcImageLayout,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_IMAGE_ASPECT_COLOR_BIT,
        1,
        mipLevel,
        1,
        arrayLayer);

    // If source and destination support blit we'll blit as this also does automatic format conversion (e.g. from BGR to RGB).
    if (supportsBlit) {
        // Define the region to blit (we will blit the whole swapchain image).
        const VkOffset3D blitSize = {
            static_cast<int32_t>(extent.width),
            static_cast<int32_t>(extent.height),
            static_cast<int32_t>(extent.depth)
        };
        VkImageBlit imageBlitRegion = {};
        imageBlitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageBlitRegion.srcSubresource.mipLevel = mipLevel;
        imageBlitRegion.srcSubresource.baseArrayLayer = arrayLayer;
        imageBlitRegion.srcSubresource.layerCount = 1;
        imageBlitRegion.srcOffsets[1] = blitSize;
        imageBlitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageBlitRegion.dstSubresource.mipLevel = 0;
        imageBlitRegion.dstSubresource.baseArrayLayer = 0;
        imageBlitRegion.dstSubresource.layerCount = 1;
        imageBlitRegion.dstOffsets[1] = blitSize;
        vkCmdBlitImage(
            commandBuffer,
            srcImage,
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            dstImage,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &imageBlitRegion,
            VK_FILTER_NEAREST);
    } else {
        // Otherwise use image copy (requires us to manually flip components).
        VkImageCopy imageCopyRegion = {};
        imageCopyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageCopyRegion.srcSubresource.mipLevel = mipLevel;
        imageCopyRegion.srcSubresource.baseArrayLayer = arrayLayer;
        imageCopyRegion.srcSubresource.layerCount = 1;
        imageCopyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageCopyRegion.dstSubresource.mipLevel = 0;
        imageCopyRegion.dstSubresource.baseArrayLayer = 0;
        imageCopyRegion.dstSubresource.layerCount = 1;
        imageCopyRegion.extent = extent;
        vkCmdCopyImage(
            commandBuffer,
            srcImage,
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            dstImage,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &imageCopyRegion);
    }

    // Transition destination image to general layout, which is the required layout for mapping the image memory later on.
    InsertImageMemoryBarrier(
        commandBuffer,
        dstImage,
        VK_ACCESS_TRANSFER_WRITE_BIT,
        VK_ACCESS_MEMORY_READ_BIT,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_GENERAL,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_IMAGE_ASPECT_COLOR_BIT,
        1,
        0,
        1,
        0);

    // Transition back the image after the blit is done.
    InsertImageMemoryBarrier(
        commandBuffer,
        srcImage,
        VK_ACCESS_TRANSFER_READ_BIT,
        VK_ACCESS_MEMORY_READ_BIT,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        srcImageLayout,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_IMAGE_ASPECT_COLOR_BIT,
        1,
        mipLevel,
        1,
        arrayLayer);

    commandBuffer.SubmitIdle();

    return supportsBlit;
}

VkFormat Image::FindSupportedFormat(
    const std::vector<VkFormat>& candidates,
    const VkImageTiling tiling,
    const VkFormatFeatureFlags features)
{
    const auto& physicalDevice = GraphicsEngineLocator::Get().GetPhysicalDevice();

    for (const auto& format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
            return format;
        if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
            return format;
    }

    return VK_FORMAT_UNDEFINED;
}
} //namespace graphics
} //namespace poke
