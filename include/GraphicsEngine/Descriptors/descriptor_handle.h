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

#include <GraphicsEngine/Shaders/shader.h>
#include <GraphicsEngine/Descriptors/interface_descriptor.h>
#include <GraphicsEngine/Descriptors/descriptor_set.h>
#include <GraphicsEngine/Models/material.h>

namespace poke {
namespace graphics {

class CommandBuffer;
class PushHandle;
class StorageHandle;
class UniformHandle;
class Pipeline;

/**
 * \brief Handler for a descriptor
 */
class DescriptorHandle {
public:

	/**
	 * \brief
	 */
	DescriptorHandle() = default;

	/**
	 * \brief
	 * \param pipeline
	 */
	DescriptorHandle(const Pipeline& pipeline);

    DescriptorHandle(const DescriptorHandle& other);

    DescriptorHandle(DescriptorHandle&& other) noexcept;

    DescriptorHandle& operator=(const DescriptorHandle& other);

    DescriptorHandle& operator=(DescriptorHandle&& other) noexcept;

    /**
     * \brief Cast parameter to ptr
     * \tparam T 
     * \param obj 
     * \return 
     */
    template <typename T>
    static T* AsPtr(T& obj) { return &obj; }

    /**
     * \brief Cast parameter to ptr
     * \tparam T
     * \param obj
     * \return
     */
    template <typename T>
    static T* AsPtr(T* obj) { return obj; }

    /**
     * \brief Cast parameter to ptr
     * \tparam T
     * \param obj
     * \return
     */
    template <typename T>
    static T* AsPtr(const std::unique_ptr<T>& obj) { return obj.get(); }

    void PushDescriptorData(const Material::PushDataContainer& dataContainer);

    /**
     * \brief bind object to memory
     * \tparam T 
     * \param descriptorHash 
     * \param descriptor 
     */
    template <typename T>
    void Push(const XXH64_hash_t descriptorHash, const T& descriptor)
    {
        if (!shader_) { return; }

        // Finds the local value given to the descriptor name.
        auto it = descriptor_.find(descriptorHash);

        if (it != descriptor_.end()) {
            // If the descriptor
            if (it->second.descriptor == AsPtr(descriptor)) { return; }

            descriptor_.erase(it);
        }

        // Only non-null descriptors can be mapped.
        if (AsPtr(descriptor) == nullptr) { return; }

        // When adding the descriptor find the location in the shader.
        auto location = shader_->GetDescriptorLocation(descriptorHash);

        if(location == -1) {
            return;
        }

        auto descriptorType = shader_->GetDescriptorType(location);

        // Adds the new descriptor value.
        auto writeDescriptor = AsPtr(descriptor)->GetWriteDescriptor(
            location,
            descriptorType);
        descriptor_.emplace(
			descriptorHash,
            DescriptorValue{
                AsPtr(descriptor),
                std::move(writeDescriptor),
                location
            });
        changed_ = true;
    }

    /**
     * \brief Bind object to memory
     * \tparam T 
     * \param descriptorHash 
     * \param descriptor 
     * \param writeDescriptorSet 
     */
    template <typename T>
    void Push(
		const XXH64_hash_t descriptorHash,
        const T& descriptor,
        WriteDescriptorSet writeDescriptorSet)
    {
        if (!shader_) { return; }

        const auto it = descriptor_.find(descriptorHash);

        if (it != descriptor_.end()) { descriptor_.erase(it); }

        const auto& location = shader_->GetDescriptorLocation(descriptorHash);

        descriptor_.emplace(
			descriptorHash,
            DescriptorValue{
                AsPtr(descriptor),
                std::move(writeDescriptorSet),
                location
            });
        changed_ = true;
    }

    /**
     * \brief Bind uniform to memory
     * \param descriptorHash 
     * \param uniformHandle 
     */
    void Push(
		const XXH64_hash_t descriptorHash,
        UniformHandle& uniformHandle);

    /**
     * \brief Bind storage to memory
     * \param descriptorHash 
     * \param storageHandle 
     */
    void Push(
		const XXH64_hash_t descriptorHash,
        StorageHandle& storageHandle);

    /**
     * \brief Bind push to memory 
     * \param descriptorHash 
     * \param pushHandle 
     */
    void Push(
		const XXH64_hash_t descriptorHash,
        PushHandle& pushHandle);

    /**
     * \brief Update the descriptor to a specific pipeline
     * \param pipeline 
     * \return true if has succeed (false if the descriptor has changed)
     */
    bool Update(const Pipeline& pipeline);

    /**
     * \brief Bind the descriptor to the command buffer with a specific pipeline 
     * \param commandBuffer 
     * \param pipeline 
     */
    void BindDescriptor(
        const CommandBuffer& commandBuffer,
        const Pipeline& pipeline) const;

    const DescriptorSet& GetDescriptorSet() const { return *descriptorSet_; }
private:
    struct DescriptorValue {
        const IDescriptor* descriptor;
        WriteDescriptorSet writeDescriptor;
        uint32_t location;
    };

    std::experimental::optional<const Shader&> shader_;

    bool pushDescriptor_ = false;
    bool changed_ = false;
    //TODO(@Nico) Remove the unique_ptr
    std::unique_ptr<DescriptorSet> descriptorSet_ = nullptr;

    std::map<XXH64_hash_t, DescriptorValue> descriptor_;
    std::vector<VkWriteDescriptorSet> writeDescriptorSets_;
};
} //namespace graphics
} //namespace poke
