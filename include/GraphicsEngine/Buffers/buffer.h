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
// Date : 28.01.20
//-----------------------------------------------------------------------------
#pragma once

#include <vector>

#include <vulkan/vulkan.h>

namespace poke {
namespace graphics {
/**
 * \brief Wrapper for VkBuffer, can be bind to graphics or compute pipeline via a descriptor set or certain commands
 */
class Buffer {
public:
    /**
     * \brief represent the status of the buffer, it's used to know if it can be used or not
     */
    enum class Status : uint8_t {
        RESET = 0,
        CHANGED,
        NORMAL
    };

	Buffer() = default;

    /**
     * \brief 
     * \param size 
     * \param usage 
     * \param properties 
     * \param data 
     */
    Buffer(
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties,
		const void *data = nullptr
    );

    virtual ~Buffer();

    /**
     * \brief Map dataptr to the logical device memory
     * \param dataPtr
     */
    void MapMemory(char** dataPtr) const;

    /**
     * \brief Unmap data from the logical device's memory
     */
    void UnmapMemory() const;

    const VkDeviceSize& GetSize() const { return size_; }

    const VkBuffer& GetBuffer() const { return buffer_; }

    const VkDeviceMemory& GetBufferMemory() const { return bufferMemory_; }

    /**
     * \brief Check if the needed memory property exist in the physical device
     * \param typeFilter
     * \param requiredProperties
     * \return
     */
    static uint32_t FindMemoryType(
        uint32_t typeFilter,
        VkMemoryPropertyFlags requiredProperties
    );

protected:
    VkDeviceSize size_ = 0;
    VkBuffer buffer_ = VK_NULL_HANDLE;
    VkDeviceMemory bufferMemory_ = VK_NULL_HANDLE;
};
} //namespace graphics
} //namespace poke
