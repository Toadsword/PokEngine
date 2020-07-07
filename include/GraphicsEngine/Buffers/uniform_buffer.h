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
 * \brief Buffer to represent a shader's uniform data
 */
class UniformBuffer : public IDescriptor, public Buffer {
public:
    /**
     * \brief 
     * \param size of the buffer
     * \param uniformData to hold that will be sent to the gpu
     */
    explicit UniformBuffer(
        VkDeviceSize size,
        const std::vector<char>& uniformData = {});

    /**
     * \brief Update the data inside the buffer
     * \param newUniformData 
     */
    void Update(const std::vector<char>& newUniformData) const;

    /**
     * \brief Get the descriptor set layout by the given following parameters
     * \param binding of the uniform (ref to the shader)
     * \param descriptorType of the uniform (ref to the shader)
     * \param stage of the uniform (ref to the shader)
     * \return 
     */
    static VkDescriptorSetLayoutBinding GetDescriptorSetLayout(
        uint32_t binding,
        VkDescriptorType
        descriptorType,
        VkShaderStageFlags
        stage);

    /**
     * \brief Get a write descriptor representing the data that will be sent to the gpu
     * \param binding of the uniform (ref to the shader)
     * \param descriptorType of the uniform (ref to the shader)
     * \return 
     */
    WriteDescriptorSet GetWriteDescriptor(
        uint32_t binding,
        VkDescriptorType descriptorType) const
    override;
};
} //namespace graphics
} //namespace poke
