#include <GraphicsEngine/Commands/command_buffer.h>

#include <GraphicsEngine/vulkan_error_handler.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>

namespace poke {
namespace graphics {
CommandBuffer::CommandBuffer(
    const bool begin,
    const VkQueueFlagBits queueType,
    const VkCommandBufferLevel bufferLevel)
    : queueType_(queueType)
{
	auto& commandPool = GraphicsEngineLocator::Get().GetCommandPool();

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = bufferLevel;
    allocInfo.commandBufferCount = 1;

	threadId_ = commandPool.GetThreadId();

    CheckVk(
        vkAllocateCommandBuffers(
            GraphicsEngineLocator::Get().GetLogicalDevice(),
            &allocInfo,
            &commandBuffer_));

    if (begin) { Begin(); }
}

CommandBuffer::~CommandBuffer()
{
    vkFreeCommandBuffers(
        GraphicsEngineLocator::Get().GetLogicalDevice(),
        GraphicsEngineLocator::Get().GetCommandPool(threadId_),
        1,
        &commandBuffer_);
}

void CommandBuffer::Begin(const VkCommandBufferUsageFlags usage)
{
    if (running_) { return; }
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = usage;

    CheckVk(vkBeginCommandBuffer(commandBuffer_, &beginInfo));

    running_ = true;
}

void CommandBuffer::End()
{
    if (!running_) { return; }

    CheckVk(vkEndCommandBuffer(commandBuffer_));

    running_ = false;
}

void CommandBuffer::SubmitIdle()
{
    const auto queueSelected = GetQueue();

    if (running_) { End(); }

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer_;


    CheckVk(vkQueueSubmit(queueSelected, 1, &submitInfo, VK_NULL_HANDLE));

	CheckVk(vkQueueWaitIdle(queueSelected));
}

void CommandBuffer::Submit(
    const VkSemaphore& waitSemaphore,
    const VkSemaphore& signalSemaphore,
    VkFence fence)
{
    const auto queueSelected = GetQueue();

    if (running_) { End(); }

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer_;

    if (waitSemaphore != VK_NULL_HANDLE) {
        // Pipeline stages used to wait at for graphics queue submissions.
        static VkPipelineStageFlags submitPipelineStages =
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        submitInfo.pWaitDstStageMask = &submitPipelineStages;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &waitSemaphore;
    }

    if (signalSemaphore != VK_NULL_HANDLE) {
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &signalSemaphore;
    }

    if (fence != VK_NULL_HANDLE) {
        const auto& logicalDevice = GraphicsEngineLocator::Get().GetLogicalDevice();
        CheckVk(
            vkWaitForFences(
                logicalDevice,
                1,
                &fence,
                VK_TRUE,
                std::numeric_limits<uint64_t>::max()));

        CheckVk(vkResetFences(logicalDevice, 1, &fence));
    }

    CheckVk(vkQueueSubmit(queueSelected, 1, &submitInfo, fence));
}

VkQueue CommandBuffer::GetQueue() const
{
    const auto& logicalDevice = GraphicsEngineLocator::Get().GetLogicalDevice();
    switch (queueType_) {
    case VK_QUEUE_GRAPHICS_BIT:
        return logicalDevice.GetGraphicsQueue();
    case VK_QUEUE_COMPUTE_BIT:
        return logicalDevice.GetComputeQueue();
    default:
        return nullptr;
    }
}
} //namespace graphics
} //namespace poke
