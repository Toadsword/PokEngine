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
// Date : 13.01.20
//-----------------------------------------------------------------------------
#pragma once

#include <optional_custom.h>

#include <GraphicsEngine/Buffers/storage_buffer.h>
#include <GraphicsEngine/Shaders/shader_uniform_block.h>

namespace poke {
namespace graphics {

/**
 * \brief Helper to use storage buffer
 */
class StorageHandle {
public:
    /**
     * \brief 
     * \param multiPipeline if true can be used in multiple pipeline
     */
    explicit StorageHandle(
        bool multiPipeline = false);

    /**
     * \brief 
     * \param uniformBlock if a uniform block is given, then the handler will handle directly the uniform block
     * \param multiPipeline if true can be used in multiple pipeline
     */
    explicit StorageHandle(
        const UniformBlock& uniformBlock,
        bool multiPipeline = false);

    /**
     * \brief Push the data inside the buffer
     * \param data 
     * \param size 
     */
    void Push(const void* data, std::size_t size);

    /**
     * \brief Push the object inside the buffer
     * \tparam T 
     * \param object to push inside the buffer
     * \param offset indicates the offset of the object (must be the same as the one in shader)
     * \param size indicates the size of the object (must be the same as the one in shader)
     */
    template <typename T>
    void Push(const T& object, const std::size_t offset, const std::size_t size)
    {
        if (!uniformBlock_) { return; }

        if (std::memcmp(&arbitraryStorageData_[0] + offset, &object, size) != 0
        ) {
            std::memcpy(&arbitraryStorageData_[0] + offset, &object, size);
            handleStatus_ = Buffer::Status::CHANGED;
        }
    }

    /**
     * \brief Push the object at the given uniform 
     * \tparam T 
     * \param uniformHash is the uniform where the object want to be pushed
     * \param object to push inside the buffer
     * \param size of the object (if == 0 then it will be compute inside the function)
     */
    template <typename T>
    void Push(
        const XXH64_hash_t& uniformHash,
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
     * \brief Update the linked uniform block if not linked already
     * \param uniformBlock 
     * \return true if the update succeed (if the uniformHandle has changed it will return false)
     */
    bool Update(const UniformBlock& uniformBlock);

    const StorageBuffer& GetUniformBuffer() const { return *storageBuffer_; }
private:
    bool multiPipeline_;
    std::experimental::optional<const UniformBlock&> uniformBlock_ = std::
        experimental::nullopt;
    uint32_t size_ = 0;
    std::vector<char> arbitraryStorageData_;

    std::unique_ptr<StorageBuffer> storageBuffer_ = nullptr;

    Buffer::Status handleStatus_ = Buffer::Status::NORMAL;
};
} //namespace graphics
} //namespace poke
