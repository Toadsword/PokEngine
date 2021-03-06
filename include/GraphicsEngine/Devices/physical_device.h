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
// Date : 08.12.19
//-----------------------------------------------------------------------------
#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace poke {
namespace graphics {

class Instance;

/**
 * \brief Encapsulate VkPhysicalDevice. It's a class to represent a GPU
 */
class PhysicalDevice {
public:
    /**
     * \brief 
     * \param instance 
     */
    PhysicalDevice(const Instance& instance);

    operator const VkPhysicalDevice &() const { return physicalDevice_; }

    const VkPhysicalDevice& GetPhysicalDevice() const;

    const VkPhysicalDeviceProperties& GetProperties() const;

    const VkPhysicalDeviceFeatures& GetFeatures() const { return features_; }

    const VkPhysicalDeviceMemoryProperties& GetMemoryProperties() const;

    const VkSampleCountFlagBits& GetMsaaSamples() const { return msaaSamples_; }
private:
    static VkPhysicalDevice ChoosePhysicalDevice(
        const std::vector<VkPhysicalDevice>& devices);

    static int32_t ScorePhysicalDevice(const VkPhysicalDevice& device);

    VkSampleCountFlagBits GetMaxUsableSampleCount() const;

    const Instance& kInstance_;

    VkPhysicalDevice physicalDevice_;
    VkPhysicalDeviceProperties properties_;
    VkPhysicalDeviceFeatures features_;
    VkPhysicalDeviceMemoryProperties memoryProperties_;
    VkSampleCountFlagBits msaaSamples_;
};
} //namespace graphics
} //namespace poke
