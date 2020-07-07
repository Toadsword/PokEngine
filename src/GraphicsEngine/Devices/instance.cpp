#include <GraphicsEngine/Devices/instance.h>

#include <iostream>

#include <SDL_vulkan.h>

#include <CoreEngine/cassert.h>
#include <Utility/log.h>
#include <GraphicsEngine/vulkan_error_handler.h>

namespace poke {
namespace graphics {
const std::vector<const char *> Instance::kValidationLayers = {
    "VK_LAYER_LUNARG_standard_validation"
};
const std::vector<const char *> Instance::kInstanceExtensions = {
    VK_KHR_SURFACE_EXTENSION_NAME
};
const std::vector<const char *> Instance::kDeviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

VkBool32 DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT*
    pCallbackData,
    void* pUserData) {
	std::string war = "Vulkan";

    switch(messageSeverity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			war += "[War] : ";
        break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			war += "[Err] : ";
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT: break;
        default: ;
    }

    war += pCallbackData->pMessage;
    LogWarning(war, LogType::GRAPHICS_LOG);

    std::cout << war.c_str() << "\n";
    return VK_FALSE;
}

Instance::Instance(SDL_Window& window)
    : debugMessenger_(VK_NULL_HANDLE)
    , instance_(VK_NULL_HANDLE) {
    SetupLayers();
    SetupExtensions(window);
    CreateInstance();
    CreateDebugCallback();
}

Instance::~Instance() {
    DestroyDebugUtilsMessengerEXT(instance_, debugMessenger_, nullptr);
    vkDestroyInstance(instance_, nullptr);
}

VkResult Instance::CreateDebugUtilsMessengerEXT(
    const VkInstance instance,
    const
    VkDebugUtilsMessengerCreateInfoEXT
    * pCreateInfo,
    const VkAllocationCallbacks*
    pAllocator,
    VkDebugUtilsMessengerEXT*
    pDebugMessenger) {
    const auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(
                              instance,
                              "vkCreateDebugUtilsMessengerEXT"));
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }

    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void Instance::DestroyDebugUtilsMessengerEXT(
    const VkInstance instance,
    const VkDebugUtilsMessengerEXT
    debugMessenger,
    const VkAllocationCallbacks*
    pAllocator) {
    const auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(
                              instance,
                              "vkDestroyDebugUtilsMessengerEXT"));
    if (func != nullptr) { func(instance, debugMessenger, pAllocator); }
}

void Instance::CmdPushDescriptorSetKhr(
    const VkDevice device,
    const VkCommandBuffer commandBuffer,
    const VkPipelineBindPoint
    pipelineBindPoint,
    const VkPipelineLayout layout,
    const uint32_t set,
    const uint32_t descriptorWriteCount,
    const VkWriteDescriptorSet*
    pDescriptorSet) {
    const auto func = reinterpret_cast<PFN_vkCmdPushDescriptorSetKHR>(
        vkGetDeviceProcAddr(
                            device,
                            "vkCmdPushDescriptorSetKHR"));

    if (func != nullptr) {
        func(commandBuffer,
             pipelineBindPoint,
             layout,
             set,
             descriptorWriteCount,
             pDescriptorSet);
    }
}

uint32_t Instance::FindMemoryTypeIndex(
    const VkPhysicalDeviceMemoryProperties& deviceMemoryProperties,
    const VkMemoryRequirements& memoryRequirements,
    const VkMemoryPropertyFlags requiredProperties) {
    for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; ++i) {
        if (memoryRequirements.memoryTypeBits & (1 << i)) {
            if ((deviceMemoryProperties.memoryTypes[i].propertyFlags &
                 requiredProperties) == requiredProperties) { return i; }
        }
    }
    return -1;
}

const std::vector<const char*>& Instance::GetInstanceLayers() const {
    return instanceLayer_;
}

const std::vector<const char*>& Instance::GetInstanceExtensions() const {
    return instanceExtensions_;
}

const std::vector<const char*>& Instance::GetDeviceExtensions() const {
    return deviceExtensions_;
}

void Instance::SetupLayers() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (auto layerName : kValidationLayers) {
        auto layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        cassert(layerFound, "Vulkan validation layer not found: \n");

        instanceLayer_.emplace_back(layerName);
    }

    for (const auto& layerName : kDeviceExtensions) {
        deviceExtensions_.emplace_back(layerName);
    }
}

void Instance::SetupExtensions(SDL_Window& window) {
    uint32_t extensionCount = 0;

	if (!SDL_Vulkan_GetInstanceExtensions(&window, &extensionCount, nullptr)) {
		//std::cout << "Could not SDL Vulkan Extensions correctly";
	}

    std::vector<const char*> extensions;
	//If debug mod, add an extension to handle error
	if (kEnableValidationLayers) {
		extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}
	const size_t addExtension = extensions.size();
	extensions.resize(addExtension + extensionCount);

	if(!SDL_Vulkan_GetInstanceExtensions(&window, &extensionCount, extensions.data() + addExtension)) {
		//std::cout << "Could not SDL Vulkan Extensions correctly";
	}

	for (const auto& instanceExtension : extensions) {
		instanceExtensions_.push_back(instanceExtension);
	}
    for (const auto& instanceExtension : kInstanceExtensions) {
        instanceExtensions_.push_back(instanceExtension);
    }
}

void Instance::CreateInstance() {
    //Create application info, use by the driver to do some optimization
    VkApplicationInfo appInfo = {}; //pNext = nullptr
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan application";
    appInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
    appInfo.pEngineName = "POK Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_MAKE_VERSION(1, 1, 0);

    //Store data for creating an instance
    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    //Set the layer for the api to be use by the sdl window
    auto extensions = instanceExtensions_;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    if (kEnableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(instanceLayer_.size());
        createInfo.ppEnabledLayerNames = instanceLayer_.data();
    }

    CheckVk(vkCreateInstance(&createInfo, nullptr, &instance_));
}

void Instance::CreateDebugCallback() {
    if (!kEnableValidationLayers)
        return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = DebugCallback;

    CheckVk(CreateDebugUtilsMessengerEXT(instance_, &createInfo, nullptr, &debugMessenger_));
}
} //namespace graphics
} //namespace poke
