#include <GraphicsEngine/Buffers/uniform_buffer.h>

namespace poke {
namespace graphics {
UniformBuffer::UniformBuffer(
    const VkDeviceSize size,
    const std::vector<char>& uniformData)
    : Buffer(
        size,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        uniformData.data()) {}

void UniformBuffer::Update(const std::vector<char>& newUniformData) const
{
    char* dataPtr;
    MapMemory(&dataPtr);
    std::memcpy(
        dataPtr,
        newUniformData.data(),
        static_cast<std::size_t>(size_));
    UnmapMemory();
}

VkDescriptorSetLayoutBinding UniformBuffer::GetDescriptorSetLayout(
    const uint32_t binding,
    const VkDescriptorType descriptorType,
    const VkShaderStageFlags stage)
{
    VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
    descriptorSetLayoutBinding.binding = binding;
    descriptorSetLayoutBinding.descriptorType = descriptorType;
    descriptorSetLayoutBinding.descriptorCount = 1;
    descriptorSetLayoutBinding.stageFlags = stage;
    descriptorSetLayoutBinding.pImmutableSamplers = nullptr;
    return descriptorSetLayoutBinding;
}

WriteDescriptorSet UniformBuffer::GetWriteDescriptor(
    const uint32_t binding,
    const VkDescriptorType descriptorType) const
{
    VkDescriptorBufferInfo bufferInfo = {};
    bufferInfo.buffer = buffer_;
    bufferInfo.offset = 0;
    bufferInfo.range = size_;

    VkWriteDescriptorSet descriptorWrite = {};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = VK_NULL_HANDLE;
    descriptorWrite.dstBinding = binding;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.descriptorType = descriptorType;
    return WriteDescriptorSet(descriptorWrite, bufferInfo);
}
} //namespace graphics
} //namespace poke
