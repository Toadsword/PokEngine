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

#include <GraphicsEngine/Pipelines/render_pipeline.h>
#include <GraphicsEngine/Descriptors/descriptor_handle.h>
#include <GraphicsEngine/Pipelines/pipeline_graphic.h>

#include <string_view.hpp>
#include <xxhash.h>

namespace poke {
namespace graphics {

class PostFilter : public RenderPipeline {
public:
    /**
     * \brief 
     * \param pipelineStage 
     * \param shaderFileName 
     */
    PostFilter(
        Pipeline::Stage pipelineStage,
        const nonstd::string_view& shaderFileName = "");

    virtual ~PostFilter();

    const DescriptorHandle& GetDescriptorSet() const { return descriptorSet_; }

    const PipelineGraphics& GetPipeline() const { return pipeline_; }

    const IDescriptor& GetAttachment(
        const nonstd::string_view& descriptorName,
        const IDescriptor& descriptor) const;

    const IDescriptor& GetAttachment(
        const nonstd::string_view& descriptorName,
        const nonstd::string_view& rendererAttachment)
    const;

    void SetAttachment(
        const nonstd::string_view& descriptorName,
        const IDescriptor& descriptor);

    bool RemoveAttachment(const nonstd::string_view& descriptorName);

protected:
    inline static uint32_t globalSwitching_ = 0;

    void PushConditional(
        const nonstd::string_view& descriptorName1,
        const nonstd::string_view& descriptorName2,
        const nonstd::string_view& rendererAttachment1,
        const nonstd::string_view& rendererAttachment2);

    PipelineGraphics pipeline_;
    DescriptorHandle descriptorSet_;

    std::map<XXH64_hash_t, const IDescriptor&> attachments_;
};
} //namespace graphics
} //namespace poke
