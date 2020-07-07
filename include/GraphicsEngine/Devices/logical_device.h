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

namespace poke {
namespace graphics {
class Instance;
class PhysicalDevice;
class Surface;

/**
 * \brief Encapsulate VkLogicalDevice. Each GPU (PhysicalDevice) need to be bind to a LogicalDevice
 */
class LogicalDevice {
public:
    /**
     * \brief 
     * \param instance 
     * \param physicalDevice 
     * \param surface 
     */
    LogicalDevice(
        const Instance& instance,
        const PhysicalDevice& physicalDevice,
        const Surface& surface);

    ~LogicalDevice();

    operator const VkDevice &() const { return logicalDevice_; }

    const VkDevice& GetLogicalDevice() const { return logicalDevice_; }

    const VkPhysicalDeviceFeatures& GetEnabledFeatures() const;

    const VkQueue& GetGraphicsQueue() const { return graphicsQueue_; }
    const VkQueue& GetPresentQueue() const { return presentQueue_; }
    const VkQueue& GetComputeQueue() const { return computeQueue_; }
    const VkQueue& GetTransferQueue() const { return transferQueue_; }
    uint32_t GetGraphicsFamily() const { return graphicsFamily_; }
    uint32_t GetPresentFamily() const { return presentFamily_; }
    uint32_t GetComputeFamily() const { return computeFamily_; }
    uint32_t GetTransferFamily() const { return transferFamily_; }
private:
    void CreateQueueIndices();
    void CreateLogicalDevice();

    const Instance& kInstance_;
    const PhysicalDevice& kPhysicalDevice_;
    const Surface& kSurface_;

    VkDevice logicalDevice_;
    VkPhysicalDeviceFeatures enabledFeatures_;

    VkQueueFlags supportedQueues_;
    uint32_t graphicsFamily_;
    uint32_t presentFamily_;
    uint32_t computeFamily_;
    uint32_t transferFamily_;

    VkQueue graphicsQueue_;
    VkQueue presentQueue_;
    VkQueue computeQueue_;
    VkQueue transferQueue_;
};
} //namespace graphics
} //namespace poke
