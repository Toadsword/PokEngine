#include <GraphicsEngine/Devices/physical_device.h>

#include <map>
#include <algorithm>

#include <GraphicsEngine/Devices/instance.h>
#include <CoreEngine/cassert.h>

namespace poke {
namespace graphics {
static const std::vector<VkSampleCountFlagBits> kStageFlagBits = {
    VK_SAMPLE_COUNT_64_BIT,
    VK_SAMPLE_COUNT_32_BIT,
    VK_SAMPLE_COUNT_16_BIT,
    VK_SAMPLE_COUNT_8_BIT,
    VK_SAMPLE_COUNT_4_BIT,
    VK_SAMPLE_COUNT_2_BIT
};

PhysicalDevice::PhysicalDevice(const Instance& instance)
    : kInstance_(instance)
    , physicalDevice_(VK_NULL_HANDLE)
    , properties_({})
    , features_({})
    , memoryProperties_({})
    , msaaSamples_(VK_SAMPLE_COUNT_1_BIT) {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(kInstance_, &deviceCount, nullptr);

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(kInstance_, &deviceCount, devices.data());

    physicalDevice_ = ChoosePhysicalDevice(devices);

    vkGetPhysicalDeviceProperties(physicalDevice_, &properties_);
    vkGetPhysicalDeviceFeatures(physicalDevice_, &features_);
    vkGetPhysicalDeviceMemoryProperties(physicalDevice_, &memoryProperties_);
    msaaSamples_ = GetMaxUsableSampleCount();
}

const VkPhysicalDeviceProperties& PhysicalDevice::GetProperties() const {
    return properties_;
}

const VkPhysicalDeviceMemoryProperties& PhysicalDevice::
GetMemoryProperties() const { return memoryProperties_; }

const VkPhysicalDevice& PhysicalDevice::GetPhysicalDevice() const {
    return physicalDevice_;
}

VkPhysicalDevice PhysicalDevice::ChoosePhysicalDevice(
    const std::vector<VkPhysicalDevice>& devices) {
    std::multimap<int32_t, VkPhysicalDevice> rankedDevices;

    for (const auto& kDevice : devices) {
        auto score = ScorePhysicalDevice(kDevice);
        rankedDevices.emplace(score, kDevice);
    }

    if (rankedDevices.rbegin()->first > 0) {
        return rankedDevices.rbegin()->second;
    }

    return nullptr;
}

int32_t PhysicalDevice::ScorePhysicalDevice(const VkPhysicalDevice& device) {
    auto score = 0;

    uint32_t extensionPropertyCount;
    vkEnumerateDeviceExtensionProperties(device,
                                         nullptr,
                                         &extensionPropertyCount,
                                         nullptr);
    std::vector<VkExtensionProperties>
        extensionProperties(extensionPropertyCount);
    vkEnumerateDeviceExtensionProperties(device,
                                         nullptr,
                                         &extensionPropertyCount,
                                         extensionProperties.data());

    for (auto currentExtension : Instance::kDeviceExtensions) {
        auto extensionFound = false;

        for (const auto& kExtension : extensionProperties) {
            if (strcmp(currentExtension, kExtension.extensionName) == 0) {
                extensionFound = true;
                break;
            }
        }

        if (!extensionFound) { return 0; }
    }

    VkPhysicalDeviceProperties physicalDeviceProperties;
    VkPhysicalDeviceFeatures physicalDeviceFeatures;
    vkGetPhysicalDeviceProperties(device, &physicalDeviceProperties);
    vkGetPhysicalDeviceFeatures(device, &physicalDeviceFeatures);

    if (physicalDeviceProperties.deviceType ==
        VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) { score += 1000; }

    score += physicalDeviceProperties.limits.maxImageDimension2D;

    return score;
}

VkSampleCountFlagBits PhysicalDevice::GetMaxUsableSampleCount() const {
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice_, &physicalDeviceProperties);

    const auto kCounts = std::min(physicalDeviceProperties
                                 .limits.framebufferColorSampleCounts,
                                 physicalDeviceProperties
                                 .limits.framebufferDepthSampleCounts);

    for (const auto& kSampleFlag : kStageFlagBits) {
        if (kCounts & kSampleFlag) { return kSampleFlag; }
    }
    return VK_SAMPLE_COUNT_1_BIT;
}
} //namespace graphics
} //namespace poke
