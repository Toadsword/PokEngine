#include <GraphicsEngine/Pipelines/pipeline_graphic.h>

#include <iostream>

#include <GraphicsEngine/vulkan_error_handler.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>

namespace poke {
namespace graphics {
const std::vector<VkDynamicState> kDynamicStates = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR,
    VK_DYNAMIC_STATE_LINE_WIDTH
};

PipelineGraphics::PipelineGraphics(
    const Stage stage,
    const PipelineGraphicsCreateInfo& createInfo)
    : stage_(stage),
      shaderStages_(createInfo.shaderStages),
      vertexInputs_(createInfo.vertexInputs),
      mode_(createInfo.mode),
      depth_(createInfo.depth),
      topology_(createInfo.topology),
      polygonMode_(createInfo.polygonMode),
      cullMode_(createInfo.cullMode),
      frontFace_(createInfo.frontFace),
      pushDescriptors_(createInfo.isPushDescriptor),
      dynamicStates_(std::vector<VkDynamicState>(kDynamicStates)),
      pipelineBindPoint_(VK_PIPELINE_BIND_POINT_GRAPHICS),
      vertexInputStateCreateInfo_({}),
      inputAssemblyStateCreateInfo_({}),
      rasterizationStateCreateInfo_({}),
      blendAttachmentStates_({}),
      colorBlendStateCreateInfo_({}),
      depthStencilStateCreateInfo_({}),
      viewportStateCreateInfo_({}),
      multisampleStateCreateInfo_({}),
      dynamicStateCreateInfo_({}),
      tessellationStateCreateInfo_({})
{
    std::sort(vertexInputs_.begin(), vertexInputs_.end());
    CreateShaderProgram(createInfo.jsonShader);
    CreateDescriptorLayout();
    CreateDescriptorPool();
    CreatePipelineLayout();
    CreateAttributes();

    switch (mode_) {
    case Mode::POLYGON:
        CreatePipelinePolygon();
        break;
    case Mode::MRT:
        CreatePipelineMrt();
        break;
    default:
        cassert(false, "Unknown pipeline mode");
    }
}

PipelineGraphics::PipelineGraphics(
    const Stage stage,
    const std::string& shaderFilePath,
    const std::vector<VertexInput>& vertexInputs,
    const Mode mode,
    const Depth depthMode,
    const VkPrimitiveTopology topology,
    const VkPolygonMode polygonMode,
    const VkCullModeFlags cullMode,
    const VkFrontFace frontFace,
    const bool pushDescriptors)
    : stage_(stage),
      vertexInputs_(vertexInputs),
      mode_(mode),
      depth_(depthMode),
      topology_(topology),
      polygonMode_(polygonMode),
      cullMode_(cullMode),
      frontFace_(frontFace),
      pushDescriptors_(pushDescriptors),
      dynamicStates_(std::vector<VkDynamicState>(kDynamicStates)),
      pipelineBindPoint_(VK_PIPELINE_BIND_POINT_GRAPHICS),
      vertexInputStateCreateInfo_({}),
      inputAssemblyStateCreateInfo_({}),
      rasterizationStateCreateInfo_({}),
      blendAttachmentStates_({}),
      colorBlendStateCreateInfo_({}),
      depthStencilStateCreateInfo_({}),
      viewportStateCreateInfo_({}),
      multisampleStateCreateInfo_({}),
      dynamicStateCreateInfo_({}),
      tessellationStateCreateInfo_({})
{
    //load shaderStage
    json jsonShader = PokFileSystem::ReadJsonFile(
        shaderFilePath,
        FileType::SHADER);

    if (CheckJsonExists(jsonShader, "vert")) {
        shaderStages_.emplace_back(jsonShader["vert"].get<std::string>());
    }

    if (CheckJsonExists(jsonShader, "frag")) {
        shaderStages_.emplace_back(jsonShader["frag"].get<std::string>());
    }

    std::sort(vertexInputs_.begin(), vertexInputs_.end());
    CreateShaderProgram(jsonShader);
    CreateDescriptorLayout();
    CreateDescriptorPool();
    CreatePipelineLayout();
    CreateAttributes();

    switch (mode_) {
    case Mode::POLYGON:
        CreatePipelinePolygon();
        break;
    case Mode::MRT:
        CreatePipelineMrt();
        break;
    default:
        cassert(false, "Unknown pipeline mode");
    }
}

PipelineGraphics::~PipelineGraphics()
{
    const auto& logicalDevice = poke::GraphicsEngineLocator::Get().GetLogicalDevice();

    for (const auto& shaderModule : modules_) {
        vkDestroyShaderModule(logicalDevice, shaderModule, nullptr);
    }

    vkDestroyDescriptorPool(logicalDevice, descriptorPool_, nullptr);
    vkDestroyPipeline(logicalDevice, pipeline_, nullptr);
    vkDestroyPipelineLayout(logicalDevice, pipelineLayout_, nullptr);
    vkDestroyDescriptorSetLayout(logicalDevice, descriptorSetLayout_, nullptr);
}

const ImageDepth& PipelineGraphics::GetDepthStencil(
    std::experimental::optional<uint32_t> stage) const
{
    return poke::GraphicsEngineLocator::Get().GetRenderStage(
                                           stage ? *stage : stage_.first).
                                       GetDepthStencil();
}

const Image2d& PipelineGraphics::GetTexture(
    const uint32_t index,
    std::experimental::optional<uint32_t> stage) const
{
    return poke::GraphicsEngineLocator::Get().GetRenderStage(
                                           stage ? *stage : stage_.first).
                                       GetFramebuffers().
                                       GetAttachment(index);
}

math::Vec2 PipelineGraphics::GetSize(
    std::experimental::optional<uint32_t> stage) const
{
    return poke::GraphicsEngineLocator::Get().GetRenderStage(
                                           stage ? *stage : stage_.first).
                                       GetSize();
}

float PipelineGraphics::GetAspectRatio(
    std::experimental::optional<uint32_t> stage) const
{
    return poke::GraphicsEngineLocator::Get().GetRenderStage(
                                           stage ? *stage : stage_.first).
                                       GetAspectRatio();
}

const std::vector<std::string>& PipelineGraphics::GetShaderStages() const
{
    return shaderStages_;
}

const std::vector<VertexInput>& PipelineGraphics::GetVertexInputs() const
{
    return vertexInputs_;
}

const VkDescriptorSetLayout& PipelineGraphics::GetDescriptorSetLayout() const
{
    return descriptorSetLayout_;
}

const VkDescriptorPool& PipelineGraphics::GetDescriptorPool() const
{
    return descriptorPool_;
}

const VkPipelineLayout& PipelineGraphics::GetPipelineLayout() const
{
    return pipelineLayout_;
}

VkPipelineBindPoint PipelineGraphics::GetPipelineBindPoint() const
{
    return pipelineBindPoint_;
}

void PipelineGraphics::CreateShaderProgram(const json& jsonShader)
{
    for (const auto& shaderStage : shaderStages_) {
        const auto stageFlag = Shader::GetShaderStage(shaderStage);

        VkShaderModule shaderModule = shader_.ProcessShader(
            jsonShader,
            shaderStage,
            stageFlag);

        VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo = {};
        pipelineShaderStageCreateInfo.sType =
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        pipelineShaderStageCreateInfo.stage = stageFlag;
        pipelineShaderStageCreateInfo.module = shaderModule;
        pipelineShaderStageCreateInfo.pName = "main";
        stages_.emplace_back(pipelineShaderStageCreateInfo);
        modules_.emplace_back(shaderModule);
    }

    shader_.ProcessShader();
}

void PipelineGraphics::CreateDescriptorLayout()
{
    const auto& logicalDevice = poke::GraphicsEngineLocator::Get().GetLogicalDevice();

    auto& descriptorSetLayouts = shader_.GetDescriptorSetLayouts();

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
    descriptorSetLayoutCreateInfo.sType =
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.flags = pushDescriptors_ ?
                                              VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR :
                                              0;
    descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(
        descriptorSetLayouts.size());
    descriptorSetLayoutCreateInfo.pBindings = descriptorSetLayouts.data();
    CheckVk(
        vkCreateDescriptorSetLayout(
            logicalDevice,
            &descriptorSetLayoutCreateInfo,
            nullptr,
            &descriptorSetLayout_));
}

void PipelineGraphics::CreateDescriptorPool()
{
    const auto& logicalDevice = poke::GraphicsEngineLocator::Get().GetLogicalDevice();

    auto descriptorPools = shader_.GetDescriptorPools();

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
    descriptorPoolCreateInfo.sType =
        VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.flags =
        VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    descriptorPoolCreateInfo.maxSets = 8192; // 16384;
    descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(
        descriptorPools.size());
    descriptorPoolCreateInfo.pPoolSizes = descriptorPools.data();
    CheckVk(
        vkCreateDescriptorPool(
            logicalDevice,
            &descriptorPoolCreateInfo,
            nullptr,
            &descriptorPool_));
}

void PipelineGraphics::CreatePipelineLayout()
{
    const auto& logicalDevice = poke::GraphicsEngineLocator::Get().GetLogicalDevice();

    auto pushConstantRanges = shader_.GetPushConstantRanges();

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.setLayoutCount = 1;
    pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout_;
    pipelineLayoutCreateInfo.pushConstantRangeCount = static_cast<uint32_t>(
        pushConstantRanges.size());
    pipelineLayoutCreateInfo.pPushConstantRanges = pushConstantRanges.data();
    CheckVk(
        vkCreatePipelineLayout(
            logicalDevice,
            &pipelineLayoutCreateInfo,
            nullptr,
            &pipelineLayout_));
}

void PipelineGraphics::CreateAttributes()
{
    const auto& logicalDevice = poke::GraphicsEngineLocator::Get().GetLogicalDevice();
    const auto& physicalDevice = poke::GraphicsEngineLocator::Get().
        GetPhysicalDevice();

    cassert(
        polygonMode_ != VK_POLYGON_MODE_LINE || logicalDevice.
        GetEnabledFeatures().fillModeNonSolid,
        "Cannot create graphics pipeline with line polygon mode when logical device does not support non solid fills.")

    inputAssemblyStateCreateInfo_.sType =
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyStateCreateInfo_.topology = topology_;
    inputAssemblyStateCreateInfo_.primitiveRestartEnable = VK_FALSE;

    rasterizationStateCreateInfo_.sType =
        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationStateCreateInfo_.depthClampEnable = VK_FALSE;
    rasterizationStateCreateInfo_.rasterizerDiscardEnable = VK_FALSE;
    rasterizationStateCreateInfo_.polygonMode = polygonMode_;
    rasterizationStateCreateInfo_.cullMode = cullMode_;
    rasterizationStateCreateInfo_.frontFace = frontFace_;
    rasterizationStateCreateInfo_.depthBiasEnable = VK_FALSE;
    rasterizationStateCreateInfo_.lineWidth = 1.0f;

    blendAttachmentStates_[0].blendEnable = VK_TRUE;
    blendAttachmentStates_[0].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    blendAttachmentStates_[0].dstAlphaBlendFactor =
        VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    blendAttachmentStates_[0].colorBlendOp = VK_BLEND_OP_ADD;
    blendAttachmentStates_[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    blendAttachmentStates_[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_DST_ALPHA;
    blendAttachmentStates_[0].alphaBlendOp = VK_BLEND_OP_MAX;
    blendAttachmentStates_[0].colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
                                               VK_COLOR_COMPONENT_G_BIT |
                                               VK_COLOR_COMPONENT_B_BIT |
                                               VK_COLOR_COMPONENT_A_BIT;

    colorBlendStateCreateInfo_.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendStateCreateInfo_.logicOpEnable = VK_FALSE;
    colorBlendStateCreateInfo_.logicOp = VK_LOGIC_OP_COPY;
    colorBlendStateCreateInfo_.attachmentCount = static_cast<uint32_t>(blendAttachmentStates_.size());
    colorBlendStateCreateInfo_.pAttachments = blendAttachmentStates_.data();
    colorBlendStateCreateInfo_.blendConstants[0] = 0.0f;
    colorBlendStateCreateInfo_.blendConstants[1] = 0.0f;
    colorBlendStateCreateInfo_.blendConstants[2] = 0.0f;
    colorBlendStateCreateInfo_.blendConstants[3] = 0.0f;

    depthStencilStateCreateInfo_.sType =
        VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilStateCreateInfo_.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    depthStencilStateCreateInfo_.front = depthStencilStateCreateInfo_.back;
    depthStencilStateCreateInfo_.back.compareOp = VK_COMPARE_OP_ALWAYS;

    switch (depth_) {
    case Depth::NONE:
        depthStencilStateCreateInfo_.depthTestEnable = VK_FALSE;
        depthStencilStateCreateInfo_.depthWriteEnable = VK_FALSE;
        break;
    case Depth::READ:
        depthStencilStateCreateInfo_.depthTestEnable = VK_TRUE;
        depthStencilStateCreateInfo_.depthWriteEnable = VK_FALSE;
        break;
    case Depth::WRITE:
        depthStencilStateCreateInfo_.depthTestEnable = VK_FALSE;
        depthStencilStateCreateInfo_.depthWriteEnable = VK_TRUE;
        break;
    case Depth::READ_WRITE:
        depthStencilStateCreateInfo_.depthTestEnable = VK_TRUE;
        depthStencilStateCreateInfo_.depthWriteEnable = VK_TRUE;
        break;
    default: ;
    }

    viewportStateCreateInfo_.sType =
        VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportStateCreateInfo_.viewportCount = 1;
    viewportStateCreateInfo_.scissorCount = 1;

    auto& renderStage = GraphicsEngineLocator::Get().GetRenderStage(stage_.first);
    const auto multisampled = renderStage.IsMultisampled(stage_.second);

    multisampleStateCreateInfo_.sType =
        VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleStateCreateInfo_.rasterizationSamples = multisampled ?
                                                           physicalDevice.
                                                           GetMsaaSamples() :
                                                           VK_SAMPLE_COUNT_1_BIT;
    multisampleStateCreateInfo_.sampleShadingEnable = VK_FALSE;

    dynamicStateCreateInfo_.sType =
        VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateCreateInfo_.dynamicStateCount = static_cast<uint32_t>(
        dynamicStates_.size());
    dynamicStateCreateInfo_.pDynamicStates = dynamicStates_.data();

    tessellationStateCreateInfo_.sType =
        VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
    tessellationStateCreateInfo_.patchControlPoints = 3;
}

void PipelineGraphics::CreatePipeline()
{
    const auto& logicalDevice = GraphicsEngineLocator::Get().GetLogicalDevice();
    const auto& pipelineCache = GraphicsEngineLocator::Get().GetPipelineCache();
    auto& renderStage = GraphicsEngineLocator::Get().GetRenderStage(stage_.first);

    std::vector<VkVertexInputBindingDescription> bindingDescriptions;
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
    uint32_t lastAttribute = 0;

    for (const auto& vertexInput : vertexInputs_) {
        for (const auto& binding : vertexInput.GetBindingDescriptions()) {
            bindingDescriptions.emplace_back(binding);
        }

        for (const auto& attribute : vertexInput.GetAttributeDescriptions()) {
            attributeDescriptions.push_back(attribute);
            attributeDescriptions[attributeDescriptions.size() - 1].location += lastAttribute;
        }

        if (!vertexInput.GetAttributeDescriptions().empty()) {
            lastAttribute = attributeDescriptions.back().location + 1;
        }
    }

    vertexInputStateCreateInfo_.sType =
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputStateCreateInfo_.vertexBindingDescriptionCount = static_cast<
        uint32_t>(bindingDescriptions.size());
    vertexInputStateCreateInfo_.pVertexBindingDescriptions = bindingDescriptions.data();
    vertexInputStateCreateInfo_.vertexAttributeDescriptionCount = static_cast<
        uint32_t>(attributeDescriptions.size());
    vertexInputStateCreateInfo_.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.stageCount = static_cast<uint32_t>(stages_.size());
    pipelineCreateInfo.pStages = stages_.data();

    pipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo_;
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo_;
    pipelineCreateInfo.pTessellationState = &tessellationStateCreateInfo_;
    pipelineCreateInfo.pViewportState = &viewportStateCreateInfo_;
    pipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo_;
    pipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo_;
    pipelineCreateInfo.pDepthStencilState = &depthStencilStateCreateInfo_;
    pipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo_;
    pipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo_;

    pipelineCreateInfo.layout = pipelineLayout_;
    pipelineCreateInfo.renderPass = *renderStage.GetRenderPass();
    pipelineCreateInfo.subpass = stage_.second;
    pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineCreateInfo.basePipelineIndex = -1;

    CheckVk(
        vkCreateGraphicsPipelines(
            logicalDevice,
            pipelineCache,
            1,
            &pipelineCreateInfo,
            nullptr,
            &pipeline_));
}

void PipelineGraphics::CreatePipelinePolygon() { CreatePipeline(); }

void PipelineGraphics::CreatePipelineMrt()
{
    auto& renderStage = GraphicsEngineLocator::Get().GetRenderStage(stage_.first);
    const auto attachmentCount = renderStage.GetAttachmentCount(stage_.second);

    std::vector<VkPipelineColorBlendAttachmentState> blendAttachmentStates;
    blendAttachmentStates.reserve(attachmentCount);

    for (uint32_t i = 0; i < attachmentCount; i++) {
        VkPipelineColorBlendAttachmentState blendAttachmentState = {};
        blendAttachmentState.blendEnable = VK_TRUE;
        blendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        blendAttachmentState.dstColorBlendFactor =
            VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        blendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
        blendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        blendAttachmentState.dstAlphaBlendFactor =
            VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        blendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
        blendAttachmentState.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        blendAttachmentStates.emplace_back(blendAttachmentState);
    }

    colorBlendStateCreateInfo_.attachmentCount = static_cast<uint32_t>(
        blendAttachmentStates.size());
    colorBlendStateCreateInfo_.pAttachments = blendAttachmentStates.data();

    CreatePipeline();
}
} //namespace graphics
} //namespace poke
