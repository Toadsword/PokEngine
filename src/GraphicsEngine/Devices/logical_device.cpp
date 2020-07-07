#include <GraphicsEngine/Devices/logical_device.h>

#include <GraphicsEngine/Devices/instance.h>
#include <GraphicsEngine/Devices/physical_device.h>
#include <GraphicsEngine/Devices/surface.h>
#include <GraphicsEngine/vulkan_error_handler.h>
#include <CoreEngine/cassert.h>
#include <Utility/log.h>

namespace poke {
namespace graphics {
LogicalDevice::LogicalDevice(
    const Instance& instance,
    const PhysicalDevice& physicalDevice,
    const Surface& surface)
    : kInstance_(instance),
      kPhysicalDevice_(physicalDevice),
      kSurface_(surface),
      logicalDevice_(VK_NULL_HANDLE),
      supportedQueues_(0),
      graphicsFamily_(0),
      presentFamily_(0),
      computeFamily_(0),
      transferFamily_(0),
      graphicsQueue_(VK_NULL_HANDLE),
      presentQueue_(VK_NULL_HANDLE),
      computeQueue_(VK_NULL_HANDLE),
      transferQueue_(VK_NULL_HANDLE)
{
    CreateQueueIndices();
    CreateLogicalDevice();
}

LogicalDevice::~LogicalDevice()
{
	std::cout << "\~LogicalDevice()\n";
    vkDeviceWaitIdle(logicalDevice_);

    vkDestroyDevice(logicalDevice_, nullptr);
}

const VkPhysicalDeviceFeatures& LogicalDevice::GetEnabledFeatures() const
{
    return enabledFeatures_;
}

void LogicalDevice::CreateQueueIndices()
{
    uint32_t deviceQueueFamilyPropertyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(
        kPhysicalDevice_,
        &deviceQueueFamilyPropertyCount,
        nullptr);

    std::vector<VkQueueFamilyProperties>
        deviceQueueFamilyProperties(deviceQueueFamilyPropertyCount);

    vkGetPhysicalDeviceQueueFamilyProperties(
        kPhysicalDevice_,
        &deviceQueueFamilyPropertyCount,
        deviceQueueFamilyProperties.
        data());

    int32_t graphicsFamily = -1;
    int32_t presentFamily = -1;
    int32_t computeFamily = -1;
    int32_t transferFamily = -1;

    for (uint32_t i = 0; i < deviceQueueFamilyPropertyCount; i++) {
        // Check for graphics support.
        if (deviceQueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            graphicsFamily = i;
            graphicsFamily_ = i;
            supportedQueues_ |= VK_QUEUE_GRAPHICS_BIT;
        }

        // Check for presentation support.
        VkBool32 presentSupport = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(
            kPhysicalDevice_,
            i,
            kSurface_,
            &presentSupport);

        if (deviceQueueFamilyProperties[i].queueCount > 0 && presentSupport) {                     
            presentFamily = i;
            presentFamily_ = i;
        }

        // Check for compute support.
        if (deviceQueueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
            computeFamily = i;
            computeFamily_ = i;
            supportedQueues_ |= VK_QUEUE_COMPUTE_BIT;
        }

        // Check for transfer support.
        if (deviceQueueFamilyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
            transferFamily = i;
            transferFamily_ = i;
            supportedQueues_ |= VK_QUEUE_TRANSFER_BIT;
        }

        if (graphicsFamily != -1 && presentFamily != -1 && computeFamily != -1
            && transferFamily != -1) { break; }
    }

    cassert(
        graphicsFamily != -1,
        "Failed to find queue family supporting VK_QUEUE_GRAPHICS_BIT");
}

void LogicalDevice::CreateLogicalDevice()
{
    auto physicalDeviceFeatures = kPhysicalDevice_.GetFeatures();

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    float queuePriorities[1] = {0.0f};

    if (supportedQueues_ & VK_QUEUE_GRAPHICS_BIT) {
        VkDeviceQueueCreateInfo graphicsQueueCreateInfo = {};
        graphicsQueueCreateInfo.sType =
            VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        graphicsQueueCreateInfo.queueFamilyIndex = graphicsFamily_;
        graphicsQueueCreateInfo.queueCount = 1;
        graphicsQueueCreateInfo.pQueuePriorities = queuePriorities;
        queueCreateInfos.emplace_back(graphicsQueueCreateInfo);
    } else { graphicsFamily_ = VK_NULL_HANDLE; }

    if (supportedQueues_ & VK_QUEUE_COMPUTE_BIT && computeFamily_ !=
        graphicsFamily_) {
        VkDeviceQueueCreateInfo computeQueueCreateInfo = {};
        computeQueueCreateInfo.sType =
            VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        computeQueueCreateInfo.queueFamilyIndex = computeFamily_;
        computeQueueCreateInfo.queueCount = 1;
        computeQueueCreateInfo.pQueuePriorities = queuePriorities;
        queueCreateInfos.emplace_back(computeQueueCreateInfo);
    } else { computeFamily_ = graphicsFamily_; }

    if (supportedQueues_ & VK_QUEUE_TRANSFER_BIT && transferFamily_ !=
        graphicsFamily_ && transferFamily_ !=
        computeFamily_) {
        VkDeviceQueueCreateInfo transferQueueCreateInfo = {};
        transferQueueCreateInfo.sType =
            VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        transferQueueCreateInfo.queueFamilyIndex = transferFamily_;
        transferQueueCreateInfo.queueCount = 1;
        transferQueueCreateInfo.pQueuePriorities = queuePriorities;
        queueCreateInfos.emplace_back(transferQueueCreateInfo);
    } else { transferFamily_ = graphicsFamily_; }

    VkPhysicalDeviceFeatures enabledFeatures = {};

    // Enable sample rate shading filtering if supported.
    if (physicalDeviceFeatures.sampleRateShading) {
        enabledFeatures.sampleRateShading = VK_TRUE;
    }

    // Fill mode non solid is required for wireframe display.
    if (physicalDeviceFeatures.fillModeNonSolid) {
        enabledFeatures.fillModeNonSolid = VK_TRUE;

        // Wide lines must be present for line width > 1.0f.
        if (physicalDeviceFeatures.wideLines) {
            enabledFeatures.wideLines = VK_TRUE;
        }
    } else {
        std::cout << "Selected GPU does not support wireframe pipelines!\n";
    }

    if (physicalDeviceFeatures.samplerAnisotropy) {
        enabledFeatures.samplerAnisotropy = VK_TRUE;
    } else {
        std::cout << "Selected GPU does not support sampler anisotropy!\n";
    }

    if (physicalDeviceFeatures.textureCompressionBC) {
        enabledFeatures.textureCompressionBC = VK_TRUE;
    } else if (
        physicalDeviceFeatures.textureCompressionASTC_LDR) {
        enabledFeatures.textureCompressionASTC_LDR = VK_TRUE;
    } else if (physicalDeviceFeatures.textureCompressionETC2) {
        enabledFeatures.textureCompressionETC2 = VK_TRUE;
    }

    if (physicalDeviceFeatures.vertexPipelineStoresAndAtomics) {
        enabledFeatures.vertexPipelineStoresAndAtomics = VK_TRUE;
    } else {
        LogWarning(
            "Selected GPU does not support vertex pipeline stores and atomics!");
    }

    if (physicalDeviceFeatures.fragmentStoresAndAtomics) {
        enabledFeatures.fragmentStoresAndAtomics = VK_TRUE;
    } else {
        LogWarning(
            "Selected GPU does not support fragment stores and atomics!");
    }

    if (physicalDeviceFeatures.shaderStorageImageExtendedFormats) {
        enabledFeatures.shaderStorageImageExtendedFormats = VK_TRUE;
    } else {
        LogWarning(
            "Selected GPU does not support shader storage extended formats!");
    }

    if (physicalDeviceFeatures.shaderStorageImageWriteWithoutFormat) {
        enabledFeatures.shaderStorageImageWriteWithoutFormat = VK_TRUE;
    } else {
        LogWarning(
            "Selected GPU does not support shader storage write without format!");
    }

    if (physicalDeviceFeatures.geometryShader) {
        enabledFeatures.geometryShader = VK_TRUE;
    } else { LogWarning("Selected GPU does not support geometry shaders!"); }

    if (physicalDeviceFeatures.tessellationShader) {
        enabledFeatures.tessellationShader = VK_TRUE;
    } else {
        LogWarning("Selected GPU does not support tessellation shaders!");
    }

    if (physicalDeviceFeatures.multiViewport) {
        enabledFeatures.multiViewport = VK_TRUE;
    } else { LogWarning("Selected GPU does not support multi viewports!"); }

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(
        queueCreateInfos.size());
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
    if (Instance::GetEnableValidationLayers()) {
        deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(
            kInstance_
            .GetInstanceLayers()
            .size());
        deviceCreateInfo.ppEnabledLayerNames = kInstance_
                                               .GetInstanceLayers().data();
    }
    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(
        kInstance_
        .GetDeviceExtensions()
        .size());
    deviceCreateInfo.ppEnabledExtensionNames = kInstance_
                                               .GetDeviceExtensions().
                                               data();
    deviceCreateInfo.pEnabledFeatures = &enabledFeatures;
    CheckVk(
        vkCreateDevice(
            kPhysicalDevice_,
            &deviceCreateInfo,
            nullptr,
            &logicalDevice_));

    vkGetDeviceQueue(logicalDevice_, graphicsFamily_, 0, &graphicsQueue_);
    vkGetDeviceQueue(logicalDevice_, presentFamily_, 0, &presentQueue_);
    vkGetDeviceQueue(logicalDevice_, computeFamily_, 0, &computeQueue_);
    vkGetDeviceQueue(logicalDevice_, transferFamily_, 0, &transferQueue_);

    enabledFeatures_ = enabledFeatures;
}
} //namespace graphics
} //namespace poke
