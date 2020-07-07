#include <GraphicsEngine/Buffers/storage_buffer.h>

namespace poke {
namespace graphics {
StorageBuffer::StorageBuffer(
    const VkDeviceSize& size,
    const std::vector<char>& data)
    : Buffer(
        size,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT |
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        data.data()) {}

void StorageBuffer::Update(const std::vector<char>& newStorageData) const
{
    char* dataPtr;
    MapMemory(&dataPtr);
    std::memcpy(
        dataPtr,
        newStorageData.data(),
        static_cast<std::size_t>(size_));
    UnmapMemory();
}

VkDescriptorSetLayoutBinding StorageBuffer::GetDescriptorSetLayout(
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

WriteDescriptorSet StorageBuffer::GetWriteDescriptor(
    const uint32_t binding,
    const VkDescriptorType descriptorType) const
{
    VkDescriptorBufferInfo bufferInfo;
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
