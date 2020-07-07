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

#include <GraphicsEngine/RenderStage/attachment.h>
#include <GraphicsEngine/RenderStage/subpass_type.h>
#include <GraphicsEngine/RenderStage/viewport.h>
#include <GraphicsEngine/Renderpass/renderpass.h>
#include <GraphicsEngine/Renderpass/framebuffers.h>
#include <GraphicsEngine/Images/image_depth.h>

#include <xxhash.h>
#include <string_view.hpp>

namespace poke {
namespace graphics {
class Swapchain;

class RenderStage {
public:
    explicit RenderStage(
        const std::vector<Attachment>& images = {},
        const std::vector<SubpassType>& subpasses = {},
        const Viewport& viewport = Viewport());
	~RenderStage() = default;

    void Update();

    void Rebuild(const Swapchain& swapchain);

    const Attachment& GetAttachment(const nonstd::string_view& name) const;

    const Attachment& GetAttachment(uint32_t binding) const;

    const std::vector<Attachment>& GetAttachments() const;

    const std::vector<SubpassType>& GetSubpasses() const { return subpasses_; }

    Viewport& GetViewport() { return viewport_; }

    void SetViewPort(const Viewport& viewport) { viewport_ = viewport; }

    math::Vec2Int GetSize() const { return size_; }

    float GetAspectRatio() const { return aspectRatio_; }

    bool IsOutOfDate() const { return outOfDate_; }

    const RenderPass* GetRenderPass() const { return renderPass_.get(); }

    const ImageDepth& GetDepthStencil() const { return *depthStencil_; }

    const Framebuffers& GetFramebuffers() const { return *framebuffers_; }

    const IDescriptor& GetDescriptor(const nonstd::string_view& name) const;

    const VkFramebuffer& GetActiveFramebuffer(
        const uint32_t& activeSwapchainImage
    ) const;

    const std::vector<VkClearValue>& GetClearValues() const;

    uint32_t GetAttachmentCount(uint32_t subpass) const;

    bool HasDepth() const;

    bool HasSwapchain() const;

    bool IsMultisampled(uint32_t subpass) const;

private:
    friend class GraphicEngine;

    std::vector<Attachment> attachments_;
    std::vector<SubpassType> subpasses_;

    Viewport viewport_;

	std::unique_ptr<RenderPass> renderPass_;
    std::unique_ptr<ImageDepth> depthStencil_;
    std::unique_ptr<Framebuffers> framebuffers_;

    std::map<XXH64_hash_t, const IDescriptor&> descriptors_;

    std::vector<VkClearValue> clearValues_;
    std::vector<uint32_t> subpassAttachmentCount_;
    Attachment depthAttachment_;
    Attachment swapchainAttachment_;

    math::Vec2Int size_;
    float aspectRatio_;
    bool outOfDate_;
};
} //namespace graphics
} //namespace poke
