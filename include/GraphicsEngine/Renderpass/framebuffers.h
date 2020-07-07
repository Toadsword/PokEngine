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

#include <GraphicsEngine/Images/image_2d.h>
#include <GraphicsEngine/Renderpass/swapchain.h>

namespace poke {
namespace graphics {
class ImageDepth;
class RenderPass;
class RenderStage;
class Renderer;

/**
 * \brief Memory buffer to draw a single frame
 */
class Framebuffers {
public:
    /**
     * \brief 
     */
	explicit Framebuffers(uint32_t width,
		uint32_t height,
		const RenderStage& renderStage,
		const RenderPass& renderpass,
		const Swapchain& swapchain,
		const ImageDepth& depthStencil,
		VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT);

    ~Framebuffers();

    const Image2d& GetAttachment(uint32_t index) const;

    const std::vector<VkFramebuffer>& GetFramebuffers() const;

private:
    std::vector<std::unique_ptr<Image2d>> imageAttachments_;
    std::vector<VkFramebuffer> framebuffers_;
};
} //namespace graphics
} //namespace poke
