#include <GraphicsEngine/Commands/command_pool.h>

#include <GraphicsEngine/vulkan_error_handler.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>

namespace poke {
namespace graphics {
CommandPool::CommandPool(const LogicalDevice& logicalDevice, const std::thread::id &threadId) :
    threadId_(threadId)
{
    const auto graphicsFamily = logicalDevice.GetGraphicsFamily();

    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT |
                     VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = graphicsFamily;

    CheckVk(
        vkCreateCommandPool(
            logicalDevice,
            &poolInfo,
            nullptr,
            &commandPool_));
}

CommandPool::~CommandPool()
{
	std::cout << "\~CommandPool()\n";

    vkDestroyCommandPool(
        GraphicsEngineLocator::Get().GetLogicalDevice(),
        commandPool_,
        nullptr);
}
} //namespace graphics
} //namespace poke
