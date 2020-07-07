#include <GraphicsEngine/Renderpass/swapchain.h>

#include <array>

#include <GraphicsEngine/vulkan_error_handler.h>
#include <CoreEngine/cassert.h>
#include <GraphicsEngine/Images/image.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>

namespace poke {
namespace graphics {
Swapchain::Swapchain(VkExtent2D extent)
    : extent_(extent),
      presentMode_(VK_PRESENT_MODE_FIFO_KHR),
      imageCount_(0),
      preTransform_(VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR),
      compositeAlpha_(VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR),
      activeImageIndex_(std::numeric_limits<uint32_t>::max())
{
    const auto& physicalDevice = poke::GraphicsEngineLocator::Get().
        GetPhysicalDevice();
    const auto& surface = poke::GraphicsEngineLocator::Get().GetSurface();

    auto surfaceFormat = surface.GetFormat();
    auto surfaceCapabilities = surface.GetCapabilities();
    const auto& logicalDevice = poke::GraphicsEngineLocator::Get().GetLogicalDevice();
    auto graphicsFamily = logicalDevice.GetGraphicsFamily();
    auto presentFamily = logicalDevice.GetPresentFamily();

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        physicalDevice,
        surface,
        &surfaceCapabilities);

    uint32_t physicalPresentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        physicalDevice,
        surface,
        &physicalPresentModeCount,
        nullptr);
    std::vector<VkPresentModeKHR>
        physicalPresentModes(physicalPresentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        physicalDevice,
        surface,
        &physicalPresentModeCount,
        physicalPresentModes.data());

    for (const auto& presentMode : physicalPresentModes) {
        if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            presentMode_ = presentMode;
            break;
        }

        if (presentMode_ != VK_PRESENT_MODE_MAILBOX_KHR && presentMode ==
            VK_PRESENT_MODE_IMMEDIATE_KHR) { presentMode_ = presentMode; }
    }

    auto desiredImageCount = surfaceCapabilities.minImageCount + 1;

    if (surfaceCapabilities.maxImageCount > 0 && desiredImageCount >
        surfaceCapabilities.maxImageCount) {
        desiredImageCount = surfaceCapabilities.maxImageCount;
    }

    if (surfaceCapabilities.supportedTransforms &
        VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
        // We prefer a non-rotated transform.
        preTransform_ = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    } else { preTransform_ = surfaceCapabilities.currentTransform; }

    for (const auto& compositeAlphaFlag : kCompositeAlphaFlags) {
        if (surfaceCapabilities.supportedCompositeAlpha & compositeAlphaFlag) {
            compositeAlpha_ = compositeAlphaFlag;
            break;
        }
    }

    VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.surface = surface;
    swapchainCreateInfo.minImageCount = desiredImageCount;
    swapchainCreateInfo.imageFormat = surfaceFormat.format;
    swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapchainCreateInfo.imageExtent = extent_;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainCreateInfo.preTransform = static_cast<VkSurfaceTransformFlagBitsKHR
    >(preTransform_);
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.compositeAlpha = compositeAlpha_;
    swapchainCreateInfo.presentMode = presentMode_;
    swapchainCreateInfo.clipped = VK_TRUE;
    swapchainCreateInfo.oldSwapchain = nullptr;

    if (surfaceCapabilities.supportedUsageFlags &
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT) {
        swapchainCreateInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    }

    if (surfaceCapabilities.supportedUsageFlags &
        VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
        swapchainCreateInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    }

    if (graphicsFamily != presentFamily) {
        std::array<uint32_t, 2> queueFamily = {graphicsFamily, presentFamily};
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchainCreateInfo.queueFamilyIndexCount = static_cast<uint32_t>(
            queueFamily.size());
        swapchainCreateInfo.pQueueFamilyIndices = queueFamily.data();
    } else {
        swapchainCreateInfo.queueFamilyIndexCount = 0;
        swapchainCreateInfo.pQueueFamilyIndices = {};
    }

    CheckVk(
        vkCreateSwapchainKHR(
            logicalDevice,
            &swapchainCreateInfo,
            nullptr,
            &swapchain_));

    CheckVk(
        vkGetSwapchainImagesKHR(
            logicalDevice,
            swapchain_,
            &imageCount_,
            nullptr));
    images_.resize(imageCount_);
    imageViews_.resize(imageCount_);
    CheckVk(
        vkGetSwapchainImagesKHR(
            logicalDevice,
            swapchain_,
            &imageCount_,
            images_.data()));

    for (uint32_t i = 0; i < imageCount_; i++) {
		imageViews_[i] = Image::CreateImageView(
            images_[i],
            VK_IMAGE_VIEW_TYPE_2D,
            surfaceFormat.format,
            VK_IMAGE_ASPECT_COLOR_BIT,
            1,
            0,
            1,
            0);
    }

    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    vkCreateFence(logicalDevice, &fenceCreateInfo, nullptr, &fenceImage_);
}

Swapchain::Swapchain(
    VkExtent2D extent,
    std::experimental::optional<Swapchain&> oldSwapchain)
    : extent_(extent),
      presentMode_(VK_PRESENT_MODE_FIFO_KHR),
      imageCount_(0),
      preTransform_(VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR),
      compositeAlpha_(VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR),
      activeImageIndex_(std::numeric_limits<uint32_t>::max())
{
    const auto& physicalDevice = poke::GraphicsEngineLocator::Get().
        GetPhysicalDevice();
    const auto& surface = poke::GraphicsEngineLocator::Get().GetSurface();

    auto surfaceFormat = surface.GetFormat();
    auto surfaceCapabilities = surface.GetCapabilities();
    const auto& logicalDevice = poke::GraphicsEngineLocator::Get().GetLogicalDevice();
    auto graphicsFamily = logicalDevice.GetGraphicsFamily();
    auto presentFamily = logicalDevice.GetPresentFamily();

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        physicalDevice,
        surface,
        &surfaceCapabilities);

    uint32_t physicalPresentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        physicalDevice,
        surface,
        &physicalPresentModeCount,
        nullptr);
    std::vector<VkPresentModeKHR>
        physicalPresentModes(physicalPresentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        physicalDevice,
        surface,
        &physicalPresentModeCount,
        physicalPresentModes.data());

    for (const auto& presentMode : physicalPresentModes) {
        if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            presentMode_ = presentMode;
            break;
        }

        if (presentMode_ != VK_PRESENT_MODE_MAILBOX_KHR && presentMode ==
            VK_PRESENT_MODE_IMMEDIATE_KHR) { presentMode_ = presentMode; }
    }

    auto desiredImageCount = surfaceCapabilities.minImageCount + 1;

    if (surfaceCapabilities.maxImageCount > 0 && desiredImageCount >
        surfaceCapabilities.maxImageCount) {
        desiredImageCount = surfaceCapabilities.maxImageCount;
    }

    if (surfaceCapabilities.supportedTransforms &
        VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
        // We prefer a non-rotated transform.
        preTransform_ = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    } else { preTransform_ = surfaceCapabilities.currentTransform; }

    for (const auto& compositeAlphaFlag : kCompositeAlphaFlags) {
        if (surfaceCapabilities.supportedCompositeAlpha & compositeAlphaFlag) {
            compositeAlpha_ = compositeAlphaFlag;
            break;
        }
    }

    VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.surface = surface;
    swapchainCreateInfo.minImageCount = desiredImageCount;
    swapchainCreateInfo.imageFormat = surfaceFormat.format;
    swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapchainCreateInfo.imageExtent = extent_;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainCreateInfo.preTransform = static_cast<VkSurfaceTransformFlagBitsKHR
    >(preTransform_);
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.compositeAlpha = compositeAlpha_;
    swapchainCreateInfo.presentMode = presentMode_;
    swapchainCreateInfo.clipped = VK_TRUE;
    swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    if (surfaceCapabilities.supportedUsageFlags &
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT) {
        swapchainCreateInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    }

    if (surfaceCapabilities.supportedUsageFlags &
        VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
        swapchainCreateInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    }

    if(oldSwapchain)
    swapchainCreateInfo.oldSwapchain = oldSwapchain->GetSwapchain();

    if (graphicsFamily != presentFamily) {
        std::array<uint32_t, 2> queueFamily = {graphicsFamily, presentFamily};
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchainCreateInfo.queueFamilyIndexCount = static_cast<uint32_t>(
            queueFamily.size());
        swapchainCreateInfo.pQueueFamilyIndices = queueFamily.data();
    } else {
        swapchainCreateInfo.queueFamilyIndexCount = 0;
        swapchainCreateInfo.pQueueFamilyIndices = {};
    }

    CheckVk(
        vkCreateSwapchainKHR(
            logicalDevice,
            &swapchainCreateInfo,
            nullptr,
            &swapchain_));

    CheckVk(
        vkGetSwapchainImagesKHR(
            logicalDevice,
            swapchain_,
            &imageCount_,
            nullptr));
    images_.resize(imageCount_);
    imageViews_.resize(imageCount_);
    CheckVk(
        vkGetSwapchainImagesKHR(
            logicalDevice,
            swapchain_,
            &imageCount_,
            images_.data()));

    for (uint32_t i = 0; i < imageCount_; i++) {
		imageViews_[i] = Image::CreateImageView(
            images_[i],
            VK_IMAGE_VIEW_TYPE_2D,
            surfaceFormat.format,
            VK_IMAGE_ASPECT_COLOR_BIT,
            1,
            0,
            1,
            0);
    }

    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    vkCreateFence(logicalDevice, &fenceCreateInfo, nullptr, &fenceImage_);
}

Swapchain::~Swapchain()
{
    const auto& logicalDevice = poke::GraphicsEngineLocator::Get().GetLogicalDevice();
    vkDestroySwapchainKHR(logicalDevice, swapchain_, nullptr);

    for (const auto& imageView : imageViews_) {
        vkDestroyImageView(logicalDevice, imageView, nullptr);
    }

    vkDestroyFence(logicalDevice, fenceImage_, nullptr);
}

VkResult Swapchain::AcquireNextImage(
    const VkSemaphore& presentCompleteSemaphore,
    VkFence fence)
{
    const auto& logicalDevice = poke::GraphicsEngineLocator::Get().GetLogicalDevice();
    if (fence != VK_NULL_HANDLE) {
        vkWaitForFences(
            logicalDevice,
            1,
            &fence,
            VK_TRUE,
            std::numeric_limits<std::uint64_t>::max());
    }

    const auto acquireResult = vkAcquireNextImageKHR(
        logicalDevice,
        swapchain_,
        std::numeric_limits<
            std::uint64_t>::max(),
        presentCompleteSemaphore,
        VK_NULL_HANDLE,
        &activeImageIndex_);

    cassert(
        acquireResult == VK_SUCCESS || acquireResult == VK_SUBOPTIMAL_KHR
        ||
        acquireResult == VK_ERROR_OUT_OF_DATE_KHR,
        "failed to acquire swapchain image");

    return acquireResult;
}

VkResult Swapchain::QueuePresent(
    const VkQueue& presentQueue,
    const VkSemaphore& waitSemaphore) const
{
    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &waitSemaphore;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapchain_;
    presentInfo.pImageIndices = &activeImageIndex_;
    return vkQueuePresentKHR(presentQueue, &presentInfo);
}

VkCompositeAlphaFlagBitsKHR Swapchain::GetCompositeAlpha() const
{
    return compositeAlpha_;
}

const std::vector<VkImageView>& Swapchain::GetImageViews() const
{
    return imageViews_;
}

bool Swapchain::IsSameExtent(const VkExtent2D extent2D) const
{
    return extent_.width == extent2D.width && extent_.height == extent2D.height;
}
} //namespace graphics
} //namespace poke
