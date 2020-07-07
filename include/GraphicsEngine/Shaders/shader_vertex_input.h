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

#include <vector>
#include <vulkan/vulkan.h>

namespace poke {
namespace graphics {
/**
 * \brief Represents the vertex input in a shader
 */
class VertexInput {
public:
    /**
     * \brief 
     * \param binding 
     * \param bindingDescriptions 
     * \param attributeDescriptions 
     */
    explicit VertexInput(
        uint32_t binding = 0,
        std::vector<VkVertexInputBindingDescription>
        bindingDescriptions = {},
        std::vector<VkVertexInputAttributeDescription>
        attributeDescriptions = {});

    uint32_t GetBinding() const { return binding_; }

    const std::vector<VkVertexInputBindingDescription>&
    GetBindingDescriptions() const { return bindingDescriptions_; }

    const std::vector<VkVertexInputAttributeDescription>&
    GetAttributeDescriptions() const { return attributeDescription_; }

    bool operator<(const VertexInput& other) const;

private:
    uint32_t binding_;
    std::vector<VkVertexInputBindingDescription> bindingDescriptions_;
    std::vector<VkVertexInputAttributeDescription> attributeDescription_;
};
} //namespace graphics
} //namespace poke
