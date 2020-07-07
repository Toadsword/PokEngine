#include <GraphicsEngine/Descriptors/descriptor_set.h>

#include <GraphicsEngine/graphic_engine.h>
#include <GraphicsEngine/vulkan_error_handler.h>

#include <CoreEngine/ServiceLocator/service_locator_definition.h>

namespace poke {
namespace graphics {

DescriptorSet::DescriptorSet(const Pipeline& pipeline)
    : pipelineLayout_(pipeline.GetPipelineLayout()),
      pipelineBindPoint_(pipeline.GetPipelineBindPoint()),
      descriptorPool_(pipeline.GetDescriptorPool()) { Initialize(pipeline); }

DescriptorSet::~DescriptorSet()
{
    vkFreeDescriptorSets(
        GraphicsEngineLocator::Get().GetLogicalDevice(),
        descriptorPool_,
        1,
        &descriptorSet_);
}

void DescriptorSet::Initialize(const Pipeline& pipeline)
{
    VkDescriptorSetLayout layouts[1] = {pipeline.GetDescriptorSetLayout()};

    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
    descriptorSetAllocateInfo.sType =
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.descriptorPool = descriptorPool_;
    descriptorSetAllocateInfo.descriptorSetCount = 1;
    descriptorSetAllocateInfo.pSetLayouts = layouts;

    CheckVk(
        vkAllocateDescriptorSets(
            GraphicsEngineLocator::Get().GetLogicalDevice(),
            &descriptorSetAllocateInfo,
            &descriptorSet_));
}

void DescriptorSet::Update(
    const std::vector<VkWriteDescriptorSet>& descriptor) const
{
    vkUpdateDescriptorSets(
        GraphicsEngineLocator::Get().GetLogicalDevice(),
        static_cast<uint32_t>(descriptor.size()),
        descriptor.data(),
        0,
        nullptr);
}

void DescriptorSet::BindDescriptor(const CommandBuffer& commandBuffer) const
{
    vkCmdBindDescriptorSets(
        commandBuffer,
        pipelineBindPoint_,
        pipelineLayout_,
        0,
        1,
        &descriptorSet_,
        0,
        nullptr);
}
} //namespace graphics
} //namespace poke
