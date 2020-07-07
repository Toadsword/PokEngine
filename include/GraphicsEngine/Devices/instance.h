//-----------------------------------------------------------------------------
// Copyright (c) 2019-2020, POK Family. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of POK Family nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author : Nicolas Schneider
// Co-Author :
// Date : 07.12.19
//-----------------------------------------------------------------------------
#pragma once

#include <vector>

#include <vulkan/vulkan.h>
#include <SDL.h>

#ifdef NDEBUG
constexpr bool kEnableValidationLayers = true;
#else
constexpr bool kEnableValidationLayers = true;
#endif

namespace poke {
namespace graphics {
/**
 * \brief Encapsulate VkInstance. It's the instance representing Vulkan
 */
class Instance {
public:
    friend VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);

    static const std::vector<const char*> kValidationLayers;
    static const std::vector<const char*> kInstanceExtensions;
    static const std::vector<const char*> kDeviceExtensions;

    explicit Instance(SDL_Window& window);
    ~Instance();

    /**
     * \brief Create a Debug Messenger specialized for vulkan layer validation
     * \param instance 
     * \param pCreateInfo 
     * \param pAllocator 
     * \param pDebugMessenger 
     * \return 
     */
    static VkResult CreateDebugUtilsMessengerEXT(
        const VkInstance instance,
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDebugUtilsMessengerEXT* pDebugMessenger);

    /**
     * \brief Destroy the layer validation's messenger
     * \param instance 
     * \param debugMessenger 
     * \param pAllocator 
     */
    static void DestroyDebugUtilsMessengerEXT(
        VkInstance instance,
        VkDebugUtilsMessengerEXT debugMessenger,
        const VkAllocationCallbacks* pAllocator);

    /**
     * \brief Function to bind ressources to the instances from a descriptor set
     * \param device 
     * \param commandBuffer 
     * \param pipelineBindPoint 
     * \param layout 
     * \param set 
     * \param descriptorWriteCount 
     * \param pDescriptorSet 
     */
    static void CmdPushDescriptorSetKhr(
        VkDevice device,
        VkCommandBuffer commandBuffer,
        VkPipelineBindPoint pipelineBindPoint,
        VkPipelineLayout layout,
        uint32_t set,
        uint32_t descriptorWriteCount,
        const VkWriteDescriptorSet*
        pDescriptorSet);

    /**
     * \brief Return of the memory index for a certain type of memory
     * \param deviceMemoryProperties 
     * \param memoryRequirements 
     * \param requiredProperties 
     * \return 
     */
    static uint32_t FindMemoryTypeIndex(
        const VkPhysicalDeviceMemoryProperties& deviceMemoryProperties,
        const VkMemoryRequirements& memoryRequirements,
        VkMemoryPropertyFlags requiredProperties);

    operator const VkInstance &() const { return instance_; }

    const std::vector<const char*>& GetInstanceLayers() const;

    const std::vector<const char*>& GetInstanceExtensions() const;

    const std::vector<const char*>& GetDeviceExtensions() const;

    const VkInstance& GetInstance() const { return instance_; }

    static bool GetEnableValidationLayers() { return kEnableValidationLayers; }
private:
    void SetupLayers();
    void SetupExtensions(SDL_Window& window);
    void CreateInstance();
    void CreateDebugCallback();

    std::vector<const char*> instanceLayer_;
    std::vector<const char*> instanceExtensions_;
    std::vector<const char*> deviceExtensions_;

    VkDebugUtilsMessengerEXT debugMessenger_;
    VkInstance instance_;
};
} //namespace graphics
} //namespace poke
