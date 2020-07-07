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

#include <optional_custom.h>

#include <GraphicsEngine/Pipelines/pipeline.h>
#include <GraphicsEngine/Commands/command_buffer.h>
#include <Math/vector.h>

namespace poke {
namespace graphics {
/**
 * \brief Pipeline specific to create a compute pass
 */
class PipelineCompute : public Pipeline {
public:
    /**
     * \brief 
     * \param shaderFilePath 
     * \param pushDescriptor 
     */
    explicit PipelineCompute(
        const std::string& shaderFilePath,
        bool pushDescriptor = false);

    ~PipelineCompute();

    /**
     * \brief Get the shader stages. 
     * \return 
     */
    const std::string& GetShaderStage() const { return shaderFilename_; }

    /**
     * \brief 
     * \return 
     */
    bool IsPushDescriptor() const override { return pushDescriptor_; }

    void Draw(const CommandBuffer& commandBuffer, math::Vec2 extent) const;

    /**
     * \brief return the shader of the pipeline, the shader is optional.
     * \return 
     */
    const Shader& GetShader() const override { return shader_; }

    /**
     * \brief Get the layout of the pipeline's descriptor.
     * \return 
     */
    const VkDescriptorSetLayout& GetDescriptorSetLayout() const override;

    /**
     * \brief Get the pipeline descriptor's pool.
     * \return 
     */
    const VkDescriptorPool& GetDescriptorPool() const override;

    /**
     * \brief Get the VkPipeline object of the pipeline.
     * \return 
     */
    const VkPipeline& GetPipeline() const override { return pipeline_; }

    /**
     * \brief Get the pipeline's layout.
     * \return 
     */
    const VkPipelineLayout& GetPipelineLayout() const override;

    /**
     * \brief Get the pipeline's bind point.
     * \return 
     */
    VkPipelineBindPoint GetPipelineBindPoint() const override;

private:
    void CreateShaderProgram(const json& jsonShader);

    void CreateDescriptorLayout();

    void CreateDescriptorPool();

    void CreatePipelineLayout();

    void CreatePipelineCompute();

    std::string shaderFilename_;
    bool pushDescriptor_;

    Shader shader_;
    VkShaderModule shaderModule_ = VK_NULL_HANDLE;
    VkPipelineShaderStageCreateInfo shaderStageCreateInfo_;

    VkDescriptorSetLayout descriptorSetLayout_ = VK_NULL_HANDLE;
    VkDescriptorPool descriptorPool_ = VK_NULL_HANDLE;

    VkPipeline pipeline_ = VK_NULL_HANDLE;
    VkPipelineLayout pipelineLayout_ = VK_NULL_HANDLE;
    VkPipelineBindPoint pipelineBindPoint_;

    const int kDescriptorPoolMaxSet_ = 8192;
};
} // namespace graphics
} // namespace poke
