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
// Date : 28.12.19
//-----------------------------------------------------------------------------
#pragma once

#include <vulkan/vulkan.h>
#include <thread>

namespace poke {
namespace graphics {

/**
 * \brief This buffer is used to store every vulkan command. Encapsulate VkCommandBuffer
 */
class CommandBuffer {
public:
    explicit CommandBuffer(
        bool begin = true,
        VkQueueFlagBits queueType = VK_QUEUE_GRAPHICS_BIT,
        VkCommandBufferLevel bufferLevel = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    ~CommandBuffer();

    /**
     * \brief Start to write inside the command buffer
     * \param usage by default the command buffer is used only one time
     */
    void Begin(
        VkCommandBufferUsageFlags usage =
            VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    /**
     * \brief Close the command buffer, nothing can be written inside anymore
     */
    void End();

    /**
     * \brief Submit to the queue of thing to draw and will pause the thread until it's finished
     */
    void SubmitIdle();

    /**
     * \brief Submit the command buffer
     * \param waitSemaphore 
     * \param signalSemaphore 
     * \param fence 
     */
    void Submit(
        const VkSemaphore& waitSemaphore = VK_NULL_HANDLE,
        const VkSemaphore& signalSemaphore = VK_NULL_HANDLE,
        VkFence fence = VK_NULL_HANDLE);

    bool IsRunning() const { return running_; }

    operator const VkCommandBuffer &() const { return commandBuffer_; }

    const VkCommandBuffer& GetCommandBuffer() const { return commandBuffer_; }

private:
    VkQueueFlagBits queueType_;
    VkCommandBuffer commandBuffer_ = VK_NULL_HANDLE;

    bool running_ = false;

    VkQueue GetQueue() const;

	std::thread::id threadId_;
};
} //namespace graphics
} //namespace poke
