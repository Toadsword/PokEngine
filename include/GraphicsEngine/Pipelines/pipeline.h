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

#include <utility>
#include <cstdint>

#include <GraphicsEngine/Commands/command_buffer.h>
#include <GraphicsEngine/Shaders/shader.h>

namespace poke {
namespace graphics {
/**
 * \brief Main pipeline object, this must be bind to the command buffer.
 */
class Pipeline {
public:
    /**
     * \brief Represents position in the render structure, first value is the renderpass and the second one is the subpass
     */
    using Stage = std::pair<uint32_t, uint32_t>;

    /**
     * \brief 
     */
    explicit Pipeline() = default;

    virtual ~Pipeline() = default;

    /**
     * \brief Bind the pipeline to the command buffer.
     * \param commandBuffer 
     */
    void BindPipeline(const CommandBuffer& commandBuffer) const
    {
        vkCmdBindPipeline(commandBuffer, GetPipelineBindPoint(), GetPipeline());
    }

    virtual const Shader& GetShader() const = 0;

    virtual bool IsPushDescriptor() const = 0;

    virtual const VkDescriptorSetLayout& GetDescriptorSetLayout() const = 0;

    virtual const VkDescriptorPool& GetDescriptorPool() const = 0;

    virtual const VkPipeline& GetPipeline() const = 0;

    virtual const VkPipelineLayout& GetPipelineLayout() const = 0;

    virtual VkPipelineBindPoint GetPipelineBindPoint() const = 0;
};
} //namespace graphics
} //namespace poke
