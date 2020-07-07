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

#include <Utility/json_utility.h>

namespace poke {
namespace graphics {
/**
 * \brief Interface between uniform in a shader and the code
 */
class Uniform {
public:
    /**
     * \brief 
     * \param binding 
     * \param offset 
     * \param size 
     * \param glType 
     * \param readOnly 
     * \param writeOnly 
     * \param stageFlags 
     */
    explicit Uniform(
        int32_t binding = -1,
        int32_t offset = -1,
        int32_t size = -1,
        int32_t glType = -1,
        bool readOnly = false,
        bool writeOnly = false,
        VkShaderStageFlags stageFlags = 0);

    int32_t GetBinding() const { return binding_; }

    int32_t GetOffset() const { return offset_; }

    int32_t GetSize() const { return size_; }

    int32_t GetGlType() const { return glType_; }

    bool IsReadOnly() const { return readOnly_; }

    bool IsWriteOnly() const { return writeOnly_; }

    VkShaderStageFlags GetStageFlags() const { return stageFlags_; }

    bool operator==(const Uniform& other) const;

    bool operator!=(const Uniform& other) const;

    std::string ToString() const;

    /**
     * \brief Create an Uniform from a json object.
     * \param jsonAttribute 
     * \return 
     */
    static Uniform CreateFromJson(json jsonAttribute);

private:
    int32_t binding_;
    int32_t offset_;
    int32_t size_;
    int32_t glType_;
    bool readOnly_;
    bool writeOnly_;
    VkShaderStageFlags stageFlags_;
};
} //namespace graphics
} //namespace poke
