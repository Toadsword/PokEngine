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
#include <string>

#include <optional_custom.h>

#include <GraphicsEngine/Shaders/shader_uniform_block.h>

namespace poke {
namespace graphics {
class Pipeline;
class CommandBuffer;

/**
 * \brief Helper to use push buffer
 */
class PushHandle {
public:
    /**
     * \brief 
     */
    explicit PushHandle() = default;

    /**
     * \brief 
     * \param uniformBlock 
     */
    explicit PushHandle(const UniformBlock& uniformBlock);

    /**
     * \brief function to push object inside the data that will be drawn
     * \tparam T 
     * \param object to push inside the buffer
     * \param offset of the object (must be the same as the one in shader)
     * \param size of the object (must be the same as the one in shader)
     */
    template <typename T>
    void Push(
        const T& object,
        const std::size_t offset,
        const std::size_t size)
    {
        std::memcpy(&data_[0] + offset, &object, size);
    }

    /**
     * \brief function to push object inside the data that will be drawn using its uniform name
     * \tparam T 
     * \param uniformHash of the uniform to link to
     * \param object to push to this uniform
     * \param size of the object (if == 0 => the size is compute in the function)
     */
    template <typename T>
    void Push(
        const XXH64_hash_t uniformHash,
        const T& object,
        const std::size_t size = 0)
    {
        if (!uniformBlock_) { return; }

        const auto& uniform = uniformBlock_->GetUniform(uniformHash);

        auto realSize = size;

        if (realSize == 0) {
            realSize = std::min(
                sizeof(object),
                static_cast<std::size_t>(uniform.GetSize()));
        }

        Push(object, static_cast<std::size_t>(uniform.GetOffset()), realSize);
    }

    /**
     * \brief Update a uniform block using this push buffer
     * \param uniformBlock
     * \return true if the update succeed (if the uniformHandle has changed it will return false)
     */
    bool Update(const UniformBlock& uniformBlock);

    /**
     * \brief Bind push buffer to the command buffer using specified pipeline
     * \param commandBuffer
     * \param pipeline
     */
    void BindPush(
        const CommandBuffer& commandBuffer,
        const Pipeline& pipeline) const;
private:
    std::experimental::optional<const UniformBlock&> uniformBlock_ = std::
        experimental::nullopt;
    std::vector<char> data_;
};
} //namespace graphics
} //namespace poke
