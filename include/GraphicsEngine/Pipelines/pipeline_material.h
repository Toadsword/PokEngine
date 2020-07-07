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

#include <GraphicsEngine/Pipelines/pipeline_graphic.h>
#include <GraphicsEngine/RenderStage/render_stage.h>

namespace poke {
namespace graphics {
/**
 * \brief Represents a pipeline that's using a material
 */
class PipelineMaterial {
public:
    /**
     * \brief Factory to create a pipeline for materials
     * \param pipelineStage 
     * \param pipelineCreate 
     * \return 
     */
    static PipelineMaterial& Create(
        Pipeline::Stage pipelineStage,
        const PipelineGraphicsCreateInfo& pipelineCreate);

    /**
     * \brief 
     * \param pipelineStage 
     * \param pipelineCreate 
     */
    PipelineMaterial(
        Pipeline::Stage pipelineStage,
        const PipelineGraphicsCreateInfo& pipelineCreate);

    ~PipelineMaterial() = default;

    /**
     * \brief Bind the pipeline to the command buffer.
     * \param commandBuffer 
     * \return 
     */
    bool BindPipeline(const CommandBuffer& commandBuffer);

    /**
     * \brief Get the stage of the pipeline.
     * \return 
     */
    Pipeline::Stage GetStage() const { return pipelineStage_; }

    /**
     * \brief Get struct for creation of the pipeline
     * \return 
     */
    const PipelineGraphicsCreateInfo& GetPipelineGraphics() const;

    /**
     * \brief Return the PipelineGraphics.
     * \return 
     */
    const PipelineGraphics& GetPipeline() const { return *pipeline_; }
private:
    Pipeline::Stage pipelineStage_;
    PipelineGraphicsCreateInfo pipelineGraphicsCreate_;
    std::experimental::optional<const RenderStage&> renderStage_;
    std::unique_ptr<PipelineGraphics> pipeline_;
};
} //namespace graphics
} //namespace poke
