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

#include <GraphicsEngine/Shaders/shader_uniform.h>

#include <xxhash.h>

namespace poke {
namespace graphics {
/**
 * \brief Interface between uniform block in a shader and the code
 */
class UniformBlock {
public:
    enum class Type : uint8_t {
        UNIFORM = 0,
        STORAGE,
        PUSH
    };

    /**
     * \brief 
     * \param binding 
     * \param size 
     * \param stageFlags 
     * \param type 
     */
    UniformBlock(
        int32_t binding = -1,
        int32_t size = -1,
        VkShaderStageFlags stageFlags = 0,
        Type type = Type::UNIFORM);

    int32_t GetBinding() const { return binding_; }

    int32_t GetSize() const { return size_; }

    VkShaderStageFlags GetStageFlags() const { return stageFlags_; }

    Type GetType() const { return type_; }

    const std::map<XXH64_hash_t, Uniform>& GetUniforms() const;

    const Uniform& GetUniform(XXH64_hash_t uniformHash) const;

    void AddUniform(XXH64_hash_t uniformHash, Uniform uniform);

    bool operator==(const UniformBlock& other) const;

    bool operator!=(const UniformBlock& other) const;

    std::string ToString() const;

    /**
     * \brief Create an uniform block from a json object.
     * \param jsonUniformBlock 
     * \return 
     */
    static UniformBlock CreateFromJson(json jsonUniformBlock);

private:
    int32_t binding_;
    int32_t size_;
    VkShaderStageFlags stageFlags_;
    Type type_;
    std::map<XXH64_hash_t, Uniform> uniforms_;
};
} //namespace graphics
} //namespace poke
