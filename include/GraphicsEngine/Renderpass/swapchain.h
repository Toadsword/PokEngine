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

#include <vector>

#include <vulkan/vulkan.h>
#include <optional_custom.h>

namespace poke {
namespace graphics {

class Swapchain {
public:
    /**
     * \brief 
     * \param extent 
     */
    explicit Swapchain(VkExtent2D extent);

    /**
     * \brief 
     * \param extent 
     * \param oldSwapchain 
     */
    explicit Swapchain(
        VkExtent2D extent,
        std::experimental::optional<Swapchain&> oldSwapchain);

    ~Swapchain();

    VkResult AcquireNextImage(
        const VkSemaphore& presentCompleteSemaphore = VK_NULL_HANDLE,
        VkFence fence = VK_NULL_HANDLE);

    VkResult QueuePresent(
        const VkQueue& presentQueue,
        const VkSemaphore& waitSemaphore = VK_NULL_HANDLE)
    const;

    VkExtent2D GetExtent() const { return extent_; }

    uint32_t GetImageCount() const { return imageCount_; }

    VkSurfaceTransformFlagsKHR GetPreTransform() const { return preTransform_; }

    VkCompositeAlphaFlagBitsKHR GetCompositeAlpha() const;

    const std::vector<VkImage>& GetImages() const { return images_; }

    const VkImage& GetActiveImage() const { return images_[activeImageIndex_]; }

    const std::vector<VkImageView>& GetImageViews() const;

    const VkSwapchainKHR& GetSwapchain() const { return swapchain_; }

    uint32_t GetActiveImageIndex() const { return activeImageIndex_; }

    bool IsSameExtent(VkExtent2D extent2D) const;

private:
    VkExtent2D extent_;
    VkPresentModeKHR presentMode_;

    uint32_t imageCount_;
    VkSurfaceTransformFlagsKHR preTransform_;
    VkCompositeAlphaFlagBitsKHR compositeAlpha_;
    std::vector<VkImage> images_;
    std::vector<VkImageView> imageViews_;
    VkSwapchainKHR swapchain_{};

    VkFence fenceImage_{};
    uint32_t activeImageIndex_;

	inline static const std::vector<VkCompositeAlphaFlagBitsKHR> kCompositeAlphaFlags = {
	VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
	VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
	VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
	VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
	};
};
} //namespace graphics
} //namespace poke
