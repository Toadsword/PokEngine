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

#include <GraphicsEngine/Pipelines/pipeline.h>
#include <Math/vector.h>
#include <GraphicsEngine/Shaders/shader_vertex_input.h>

namespace poke {
namespace graphics {
class ImageDepth;
class Image2d;
struct PipelineGraphicsCreateInfo;

/**
 * \brief Default pipeline to draw any kind of mesh
 */
class PipelineGraphics : public Pipeline {
public:
    /**
     * \brief Mode the the pipeline.
     */
    enum class Mode : uint8_t {
        POLYGON = 0,
        MRT
    };

    /**
     * \brief Depth write/readability
     */
    enum class Depth : uint8_t {
        NONE = 0,
        READ = 1,
        WRITE = 2,
        READ_WRITE = READ | WRITE
    };

    /**
     * \brief 
     * \param stage 
     * \param createInfo 
     */
    PipelineGraphics(
        Stage stage,
        const PipelineGraphicsCreateInfo& createInfo);

    /**
     * \brief s
     * \param stage 
     * \param shaderFilePath 
     * \param vertexInputs 
     * \param mode 
     * \param depthMode 
     * \param topology 
     * \param polygonMode 
     * \param cullMode 
     * \param frontFace 
     * \param pushDescriptors 
     */
    PipelineGraphics(
        Stage stage,
        const std::string& shaderFilePath,
        const std::vector<VertexInput>& vertexInputs,
        Mode mode = Mode::POLYGON,
        Depth depthMode = Depth::READ_WRITE,
        VkPrimitiveTopology topology =
            VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL,
        VkCullModeFlags cullMode = VK_CULL_MODE_BACK_BIT,
        VkFrontFace frontFace = VK_FRONT_FACE_CLOCKWISE,
        bool pushDescriptors = false);

    ~PipelineGraphics();

    /**
     * \brief Get depth image of the pipeline.
     * \param stage 
     * \return 
     */
    const ImageDepth& GetDepthStencil(
        std::experimental::optional<uint32_t> stage = std::experimental::
            nullopt) const;

    /**
     * \brief Get the texture write on.
     * \param index 
     * \param stage 
     * \return 
     */
    const Image2d& GetTexture(
        uint32_t index,
        std::experimental::optional<uint32_t> stage = std::experimental::
            nullopt) const;

    /**
     * \brief Get the size of screen space where the pipeline is drawn.
     * \param stage 
     * \return 
     */
    math::Vec2 GetSize(
        std::experimental::optional<uint32_t> stage = std::experimental::
            nullopt) const;

    /**
     * \brief Get ratio of with/height.
     * \param stage 
     * \return 
     */
    float GetAspectRatio(
        std::experimental::optional<uint32_t> stage = std::experimental::
            nullopt) const;

    /**
     * \brief Get pipeline's stage.
     * \return 
     */
    Stage GetStage() const { return stage_; }

    /**
     * \brief Get pipeline's shaders stages.
     * \return 
     */
    const std::vector<std::string>& GetShaderStages() const;

    /**
     * \brief Get pipeline vertex input of .vert stage.
     * \return 
     */
    const std::vector<VertexInput>& GetVertexInputs() const;

    /**
     * \brief Get pipeline's mode.
     * \return 
     */
    Mode GetMode() const { return mode_; }

    /**
     * \brief Get pipeline's depth mode.
     * \return 
     */
    Depth GetDepth() const { return depth_; }

    /**
     * \brief Get pipeline's topology.
     * \return 
     */
    VkPrimitiveTopology GetTopology() const { return topology_; }

    /**
     * \brief Get pipeline's polygon's mode.
     * \return 
     */
    VkPolygonMode GetPolygonMode() const { return polygonMode_; }

    /**
     * \brief Get pipeline culling mode.
     * \return 
     */
    VkCullModeFlags GetCullMode() const { return cullMode_; }

    /**
     * \brief Get the first step of polygon rasterization.
     * \return 
     */
    VkFrontFace GetFrontFace() const { return frontFace_; }

    /**
     * \brief Get if the pipeline is a push descriptor.
     * \return 
     */
    bool IsPushDescriptor() const override { return pushDescriptors_; }

    /**
     * \brief Get pipeline's shader. Shader is optional.
     * \return 
     */
    const Shader& GetShader() const override { return shader_; }

    /**
     * \brief Get pipeline's descriptor set layout.
     * \return 
     */
    const VkDescriptorSetLayout& GetDescriptorSetLayout() const override;

    /**
     * \brief Get pipeline descriptor's pool.
     * \return 
     */
    const VkDescriptorPool& GetDescriptorPool() const override;

    /**
     * \brief Get the vulkan's pipeline object.
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

    void CreateAttributes();

    void CreatePipeline();

    void CreatePipelinePolygon();

    void CreatePipelineMrt();

    Stage stage_;
    std::vector<std::string> shaderStages_;
    std::vector<VertexInput> vertexInputs_;
    Mode mode_;
    Depth depth_;
    VkPrimitiveTopology topology_;
    VkPolygonMode polygonMode_;
    VkCullModeFlags cullMode_;
    VkFrontFace frontFace_;
    bool pushDescriptors_;

    Shader shader_;

    std::vector<VkDynamicState> dynamicStates_;

    std::vector<VkShaderModule> modules_;
    std::vector<VkPipelineShaderStageCreateInfo> stages_;

    VkDescriptorSetLayout descriptorSetLayout_ = VK_NULL_HANDLE;
    VkDescriptorPool descriptorPool_ = VK_NULL_HANDLE;

    VkPipeline pipeline_ = VK_NULL_HANDLE;
    VkPipelineLayout pipelineLayout_ = VK_NULL_HANDLE;
    VkPipelineBindPoint pipelineBindPoint_;

    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo_;
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo_;
    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo_;
    std::array<VkPipelineColorBlendAttachmentState, 1> blendAttachmentStates_;
    VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo_;
    VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo_;
    VkPipelineViewportStateCreateInfo viewportStateCreateInfo_;
    VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo_;
    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo_;
    VkPipelineTessellationStateCreateInfo tessellationStateCreateInfo_;
};

/**
 * \brief Structure use to store data to build a graphics pipeline.
 */
struct PipelineGraphicsCreateInfo {
    PipelineGraphicsCreateInfo(
        const std::string& shaderFilePath,
        const std::vector<VertexInput>& vertexInputs = {},
        const PipelineGraphics::Mode mode = PipelineGraphics::Mode::POLYGON,
        const PipelineGraphics::Depth depth = PipelineGraphics::Depth::
            READ_WRITE,
        const VkPrimitiveTopology topology =
            VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        const VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL,
        const VkCullModeFlags cullMode = VK_CULL_MODE_BACK_BIT,
        const VkFrontFace frontFace = VK_FRONT_FACE_CLOCKWISE,
        const bool pushDescriptors = false)
        : vertexInputs(vertexInputs),
          mode(mode),
          depth(depth),
          topology(topology),
          polygonMode(polygonMode),
          cullMode(cullMode),
          frontFace(frontFace),
          isPushDescriptor(pushDescriptors)
    {
        //load shaderStage
        jsonShader = PokFileSystem::ReadJsonFile(
            shaderFilePath,
            FileType::SHADER);

        if (CheckJsonExists(jsonShader, "vert")) {
            shaderStages.emplace_back(jsonShader["vert"].get<std::string>());
        }

        if (CheckJsonExists(jsonShader, "frag")) {
            shaderStages.emplace_back(jsonShader["frag"].get<std::string>());
        }
    }

    bool operator==(const PipelineGraphicsCreateInfo& other) const
    {
        return
            shaderStages == other.shaderStages &&
            mode == other.mode &&
            depth == other.depth &&
            topology == other.topology &&
            polygonMode == other.polygonMode &&
            cullMode == other.cullMode &&
            frontFace == other.frontFace &&
            isPushDescriptor == other.isPushDescriptor;
    }

    bool operator!=(const PipelineGraphicsCreateInfo& other) const
    {
        return !(*this == other);
    }

    std::vector<std::string> shaderStages;
    std::vector<VertexInput> vertexInputs;

    PipelineGraphics::Mode mode;
    PipelineGraphics::Depth depth;
    VkPrimitiveTopology topology;
    VkPolygonMode polygonMode;
    VkCullModeFlags cullMode;
    VkFrontFace frontFace;
    bool isPushDescriptor;
    json jsonShader;
};
} // namespace graphics
} // namespace poke
