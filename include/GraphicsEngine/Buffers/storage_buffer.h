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

#include <GraphicsEngine/Descriptors/interface_descriptor.h>
#include <GraphicsEngine/Buffers/buffer.h>

namespace poke {
namespace graphics {
/**
* \brief Buffer to store data, mainly used for layout in shader. Its size can be up to 16KB, it must be used to store arrays of data
*/
class StorageBuffer : public IDescriptor, public Buffer {
public:
    /**
     * \brief 
     * \param size 
     * \param data 
     */
    explicit StorageBuffer(
        const VkDeviceSize& size,
        const std::vector<char>& data = {});

    /**
     * \brief Change data inside the buffer
     * \param newStorageData
     */
    void Update(const std::vector<char>& newStorageData) const;

    /**
     * \brief Get a descriptor set layout using all params
     * \param binding
     * \param descriptorType
     * \param stage
     * \return
     */
    static VkDescriptorSetLayoutBinding GetDescriptorSetLayout(
        uint32_t binding,
        VkDescriptorType descriptorType,
        VkShaderStageFlags stage);

    /**
     * \brief Get the write descriptor for a specific binding
     * \param binding
     * \param descriptorType
     * \return
     */
    WriteDescriptorSet GetWriteDescriptor(
        uint32_t binding,
        VkDescriptorType descriptorType) const override;
};
} //namespace graphics
} //namespace poke
