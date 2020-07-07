#include <GraphicsEngine/Images/image_depth.h>

#include <CoreEngine/cassert.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>

namespace poke {
namespace graphics {

ImageDepth::ImageDepth(const math::Vec2& extent, const VkSampleCountFlagBits samples)
    : Image(
        VK_FILTER_LINEAR,
        VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        samples,
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        FindSupportedFormat(
			kTryFormats,
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT),
        1,
        1,
		VkExtent3D{static_cast<uint32_t>(extent.x), static_cast<uint32_t>(extent.y), 1})
{
	cassert(format_ != VK_FORMAT_UNDEFINED, "No depth format has been found");

    VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    if (HasStencil(format_))
        aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;

    image_ = CreateImage(
        memory_,
        extent_,
        format_,
        sample_,
        VK_IMAGE_TILING_OPTIMAL,
        usage_,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        1,
        1,
        VK_IMAGE_TYPE_2D);

	sampler_ = CreateImageSampler(filter_, addressMode_, false, 1);
	
	view_ = CreateImageView(
        image_,
        VK_IMAGE_VIEW_TYPE_2D,
        format_,
        VK_IMAGE_ASPECT_DEPTH_BIT,
        1,
        0,
        1,
        0);
    TransitionImageLayout(
        image_,
        VK_IMAGE_LAYOUT_UNDEFINED,
        layout_,
        aspectMask,
        1,
        0,
        1,
        0);
}
} //namespace graphics
} //namespace poke
