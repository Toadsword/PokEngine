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
#include <optional_custom.h>

#include <vulkan/vulkan.h>

namespace poke {
namespace graphics {
class ImageDepth;
class RenderStage;

/**
 * \brief represents all pass that can be drawn
 */
class RenderPass {
public:
    /**
	 * \brief Description of a subpass
	 */
    class SubpassDescription {
    public:
        SubpassDescription(
            const VkPipelineBindPoint bindPoint,
            const std::vector<VkAttachmentReference>& colorAttachments,
            std::experimental::optional<uint32_t> depthAttachment)
            : subpassDescription_({})
            , colorAttachments_(colorAttachments)
            , depthStencilAttachment_({}) {
            subpassDescription_.pipelineBindPoint = bindPoint;
            subpassDescription_.colorAttachmentCount = static_cast<uint32_t>(
                colorAttachments_.size());
            subpassDescription_.pColorAttachments = colorAttachments_.data();

            if (depthAttachment) {
                depthStencilAttachment_.attachment = *depthAttachment;
                depthStencilAttachment_.layout =
                    VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                subpassDescription_.pDepthStencilAttachment = &
                    depthStencilAttachment_;
            }
        }

        const VkSubpassDescription& GetSubpassDescription() const {
            return subpassDescription_;
        }

    private:
        VkSubpassDescription subpassDescription_;
        std::vector<VkAttachmentReference> colorAttachments_;
        VkAttachmentReference depthStencilAttachment_;
    };

    /**
     * \brief 
     */
	explicit RenderPass(const RenderStage& renderStage,
		VkFormat depthFormat,
		VkFormat surfaceFormat,
		VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT);

    ~RenderPass();

	operator const VkRenderPass &() const { return renderPass_; }

    const VkRenderPass& GetRenderPass() const { return renderPass_; }

private:
    VkRenderPass renderPass_ = VK_NULL_HANDLE;
};
} //namespace graphics
} //namespace poke
