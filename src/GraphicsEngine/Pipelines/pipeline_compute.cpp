#include <GraphicsEngine/Pipelines/pipeline_compute.h>

#include <cmath>

#include <GraphicsEngine/vulkan_error_handler.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>

namespace poke {
namespace graphics {
PipelineCompute::PipelineCompute(
    const std::string& shaderFilePath,
    const bool pushDescriptor)
    : pushDescriptor_(pushDescriptor),
      shaderStageCreateInfo_({}),
      pipelineBindPoint_(VK_PIPELINE_BIND_POINT_COMPUTE)
{
    //load shaderStage
    json jsonShader = PokFileSystem::ReadJsonFile(
        shaderFilePath,
        FileType::SHADER);

    if (CheckJsonExists(jsonShader, "vert")) {
        shaderFilename_ = jsonShader["vert"].get<std::string>();
    }

    if (CheckJsonExists(jsonShader, "frag")) {
        shaderFilename_ = jsonShader["frag"].get<std::string>();
    }

    if (CheckJsonExists(jsonShader, "comp")) {
        shaderFilename_ = jsonShader["comp"].get<std::string>();
    }

    CreateShaderProgram(jsonShader);
    CreateDescriptorLayout();
    CreateDescriptorPool();
    CreatePipelineLayout();
    CreatePipelineCompute();
}

PipelineCompute::~PipelineCompute()
{
    const auto& logicalDevice = poke::GraphicsEngineLocator::Get().GetLogicalDevice();

    vkDestroyShaderModule(logicalDevice, shaderModule_, nullptr);

    vkDestroyDescriptorSetLayout(logicalDevice, descriptorSetLayout_, nullptr);
    vkDestroyDescriptorPool(logicalDevice, descriptorPool_, nullptr);
    vkDestroyPipeline(logicalDevice, pipeline_, nullptr);
    vkDestroyPipelineLayout(logicalDevice, pipelineLayout_, nullptr);
}

void PipelineCompute::Draw(
    const CommandBuffer& commandBuffer,
    const math::Vec2 extent) const
{
    const auto groupCountX = static_cast<uint32_t>(std::ceil(
        static_cast<float>(extent.x) / static_cast<float>(*shader_.
            GetLocalSizes()[0])));

    const auto groupCountY = static_cast<uint32_t>(std::ceil(
        static_cast<float>(extent.y) / static_cast<float>(*shader_.
            GetLocalSizes()[1])));

    vkCmdDispatch(commandBuffer, groupCountX, groupCountY, 1);
}

const VkDescriptorSetLayout& PipelineCompute::GetDescriptorSetLayout() const
{
    return descriptorSetLayout_;
}

const VkDescriptorPool& PipelineCompute::GetDescriptorPool() const
{
    return descriptorPool_;
}

const VkPipelineLayout& PipelineCompute::GetPipelineLayout() const
{
    return pipelineLayout_;
}

VkPipelineBindPoint PipelineCompute::GetPipelineBindPoint() const
{
    return pipelineBindPoint_;
}

void PipelineCompute::CreateShaderProgram(const json& jsonShader)
{
    const auto stageFlag = Shader::GetShaderStage(shaderFilename_);

    shaderModule_ = shader_.ProcessShader(
        jsonShader,
        shaderFilename_,
        stageFlag);

    shaderStageCreateInfo_.sType =
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageCreateInfo_.stage = stageFlag;
    shaderStageCreateInfo_.module = shaderModule_;
    shaderStageCreateInfo_.pName = "main";

    shader_.ProcessShader();
}

void PipelineCompute::CreateDescriptorLayout()
{
    const auto& logicalDevice = poke::GraphicsEngineLocator::Get().GetLogicalDevice();

    auto descriptorSetLayouts = shader_.GetDescriptorSetLayouts();

    VkDescriptorSetLayoutCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    createInfo.flags = pushDescriptor_ ?
                           VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR :
                           0;
    createInfo.bindingCount = static_cast<uint32_t>(descriptorSetLayouts.size()
    );
    createInfo.pBindings = descriptorSetLayouts.data();
    CheckVk(
        vkCreateDescriptorSetLayout(
            logicalDevice,
            &createInfo,
            nullptr,
            &descriptorSetLayout_));
}

void PipelineCompute::CreateDescriptorPool()
{
    const auto& logicalDevice = poke::GraphicsEngineLocator::Get().GetLogicalDevice();

    auto descriptorPool = shader_.GetDescriptorPools();

    VkDescriptorPoolCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    createInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    createInfo.maxSets = kDescriptorPoolMaxSet_;
    createInfo.poolSizeCount = static_cast<uint32_t>(descriptorPool.size());
    createInfo.pPoolSizes = descriptorPool.data();

    CheckVk(
        vkCreateDescriptorPool(
            logicalDevice,
            &createInfo,
            nullptr,
            &descriptorPool_));
}

void PipelineCompute::CreatePipelineLayout()
{
    const auto& logicalDevice = poke::GraphicsEngineLocator::Get().GetLogicalDevice();

    auto pushConstantRanges = shader_.GetPushConstantRanges();

    VkPipelineLayoutCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    createInfo.setLayoutCount = 1;
    createInfo.pSetLayouts = &descriptorSetLayout_;
    createInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges
        .size());
    createInfo.pPushConstantRanges = pushConstantRanges.data();

    CheckVk(
        vkCreatePipelineLayout(
            logicalDevice,
            &createInfo,
            nullptr,
            &pipelineLayout_));
}

void PipelineCompute::CreatePipelineCompute()
{
    const auto& logicalDevice = poke::GraphicsEngineLocator::Get().GetLogicalDevice();
    const auto pipelineCache = poke::GraphicsEngineLocator::Get().GetPipelineCache();

    VkComputePipelineCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    createInfo.stage = shaderStageCreateInfo_;
    createInfo.layout = pipelineLayout_;
    createInfo.basePipelineHandle = VK_NULL_HANDLE;
    createInfo.basePipelineIndex = -1;

    CheckVk(
        vkCreateComputePipelines(
            logicalDevice,
            pipelineCache,
            1,
            &createInfo,
            nullptr,
            &pipeline_));
}
} //namespace graphics
} //namespace poke
