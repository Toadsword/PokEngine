#include <GraphicsEngine/Buffers/buffer.h>

#include <array>

#include <GraphicsEngine/vulkan_error_handler.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>

namespace poke {
namespace graphics {
Buffer::Buffer(
    const VkDeviceSize size,
    const VkBufferUsageFlags usage,
    const VkMemoryPropertyFlags properties, 
	const void *data)
    : size_(size)
{
    const auto& logicalDevice = GraphicsEngineLocator::Get().GetLogicalDevice();
    const auto graphicsFamily = logicalDevice.GetGraphicsFamily();
    const auto presentFamily = logicalDevice.GetPresentFamily();
    const auto computeFamily = logicalDevice.GetComputeFamily();

    std::array<uint32_t, 3> queueFamily = {
        graphicsFamily,
        presentFamily,
        computeFamily
    };

    // Create the buffer handle.
    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size = size;
    bufferCreateInfo.usage = usage;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferCreateInfo.queueFamilyIndexCount = static_cast<uint32_t>(queueFamily.
        size());
    bufferCreateInfo.pQueueFamilyIndices = queueFamily.data();
    CheckVk(
        vkCreateBuffer(
            logicalDevice,
            &bufferCreateInfo,
            nullptr,
            &buffer_));

    // Create the memory backing up the buffer handle.
    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(
        logicalDevice,
        buffer_,
        &memoryRequirements);
    VkMemoryAllocateInfo memoryAllocateInfo = {};
    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.allocationSize = memoryRequirements.size;
    memoryAllocateInfo.memoryTypeIndex =
        FindMemoryType(
            memoryRequirements.memoryTypeBits,
            properties);

    //TODO(@Nico) Stop allocating for every new buffer.
    CheckVk(
        vkAllocateMemory(
            logicalDevice,
            &memoryAllocateInfo,
            nullptr,
            &bufferMemory_));

    // If a pointer to the buffer data has been passed, map the buffer and copy over the data.
    if (data) {
        char* dataPtr;
        MapMemory(&dataPtr);
        std::memcpy(dataPtr, data, size);

        // If host coherency hasn't been requested, do a manual flush to make writes visible.
        if ((properties & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0) {
            VkMappedMemoryRange mappedMemoryRange = {};
            mappedMemoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
            mappedMemoryRange.memory = bufferMemory_;
            mappedMemoryRange.offset = 0;
            mappedMemoryRange.size = size;
            vkFlushMappedMemoryRanges(logicalDevice, 1, &mappedMemoryRange);
        }

        UnmapMemory();
    }

    // Attach the memory to the buffer object.
    CheckVk(vkBindBufferMemory(logicalDevice, buffer_, bufferMemory_, 0));
}

Buffer::~Buffer()
{
    const auto& logicalDevice = GraphicsEngineLocator::Get().GetLogicalDevice();
    vkDestroyBuffer(logicalDevice, buffer_, nullptr);
    vkFreeMemory(logicalDevice, bufferMemory_, nullptr);
}

void Buffer::MapMemory(char** dataPtr) const
{
    const auto& logicalDevice = GraphicsEngineLocator::Get().GetLogicalDevice();
    CheckVk(
        vkMapMemory(
            logicalDevice,
            GetBufferMemory(),
            0,
            size_,
            0,
            reinterpret_cast<void**>(dataPtr)));
}

void Buffer::UnmapMemory() const
{
    const auto& logicalDevice = GraphicsEngineLocator::Get().GetLogicalDevice();
    vkUnmapMemory(logicalDevice, GetBufferMemory());
}

uint32_t Buffer::FindMemoryType(
    const uint32_t typeFilter,
    const VkMemoryPropertyFlags requiredProperties)
{
    const auto& physicalDevice = GraphicsEngineLocator::Get().
        GetPhysicalDevice();
    const auto memoryProperties = physicalDevice.GetMemoryProperties();

    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
        const uint32_t memoryTypeBits = 1 << i;
        const bool isRequiredMemoryType = typeFilter & memoryTypeBits;

        const auto properties = memoryProperties.memoryTypes[i].propertyFlags;
        const auto hasRequiredProperties =
            (properties & requiredProperties) == requiredProperties;

        if (isRequiredMemoryType && hasRequiredProperties) { return i; }
    }

    cassert(false, "failed to find a valid memory type for buffer");
}
} //namespace graphics 
} //namespace poke
