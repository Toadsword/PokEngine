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
// Date : 29.01.20
//-----------------------------------------------------------------------------
#pragma once

#include <map>
#include <vector>
#include <array>

#include <vulkan/vulkan.h>
#include <Utility/json_utility.h>
#include <GraphicsEngine/Shaders/shader_uniform.h>
#include <GraphicsEngine/Shaders/shader_uniform_block.h>
#include <GraphicsEngine/Shaders/shader_attribute.h>
#include <optional_custom.h>
#include <string_view.hpp>
#include <xxhash.h>

namespace poke {
namespace graphics {

/**
 * \brief Represents a shader with all its uniform, make the link between the code and the spir-v
 */
class Shader {
    static const int kLocalSizeDimension = 3;

public:
    /**
     * \brief 
     */
    explicit Shader();

    /**
     * \brief Create descriptors.
     */
    void ProcessShader();

    /**
     * \brief Generate shader module.
     * \param jsonShader 
     * \param filename 
     * \param stageFlag 
     * \return 
     */
    VkShaderModule ProcessShader(
        const json& jsonShader,
        const nonstd::string_view& filename,
        VkShaderStageFlags stageFlag
    );

    /**
     * \brief Get VkFormat from a glType flag.
     * \param type 
     * \return 
     */
    static VkFormat GlTypeToVk(int32_t type);

    /**
     * \brief Get a descriptor location with the given name.
     * \param name 
     * \return 
     */
    uint32_t GetDescriptorLocation(const nonstd::string_view& name) const;

	/**
	 * \brief Get a descriptor location with the given hash.
	 * \param descriptorHash
	 * \return
	 */
	uint32_t GetDescriptorLocation(const XXH64_hash_t& descriptorHash) const;

    /**
     * \brief Get a descriptor size with the given name.
     * \param name 
     * \return 
     */
    uint32_t GetDescriptorSize(const nonstd::string_view& name) const;

    /**
     * \brief Get a Uniform from a name.
     * \param name 
     * \return 
     */
    const Uniform& GetUniform(const nonstd::string_view& name) const;

    /**
     * \brief Get a UniformBlock from a name.
     * \param name 
     * \return 
     */
    const UniformBlock& GetUniformBlock(const nonstd::string_view& name) const;

	/**
	 * \brief Get a UniformBlock from a hash.
	 * \param descriptorHash
	 * \return
	 */
    const UniformBlock& GetUniformBlock(const XXH64_hash_t descriptorHash) const;

    /**
     * \brief Get an Attribute from a name.
     * \param name 
     * \return 
     */
    const Attribute& GetAttribute(const nonstd::string_view& name) const;

    /**
     * \brief Get ranges of push constant.
     * \return 
     */
    std::vector<VkPushConstantRange> GetPushConstantRanges() const;

    /**
     * \brief Get last descriptor location written on. 
     * \return 
     */
    uint32_t GetLastDescriptorBinding() const { return lastDescriptorBinding_; }

    /**
     * \brief Get all uniforms.
     * \return 
     */
    const std::map<XXH64_hash_t, Uniform>& GetUniforms() const;

    /**
     * \brief Get all uniforms blocks.
     * \return 
     */
    const std::map<XXH64_hash_t, UniformBlock>& GetUniformBlocks() const;

    /**
     * \brief Get all attributes.
     * \return 
     */
    const std::map<XXH64_hash_t, Attribute>& GetAttributes() const;

    /**
     * \brief Get local size. Only use for compute shader.
     * \return 
     */
    const std::array<std::experimental::optional<uint32_t>, kLocalSizeDimension>
    & GetLocalSizes() const;

    /**
     * \brief Get all descriptor set layout.
     * \return 
     */
    const std::vector<VkDescriptorSetLayoutBinding>&
    GetDescriptorSetLayouts() const;

    /**
     * \brief get all descriptors pools.
     * \return 
     */
    const std::vector<VkDescriptorPoolSize>& GetDescriptorPools() const;

    /**
     * \brief Get the descriptor type at the given location.
     * \param location 
     * \return 
     */
    VkDescriptorType GetDescriptorType(uint32_t location) const;

    /**
     * \brief Get all attribute description.
     * \return 
     */
    const std::vector<VkVertexInputAttributeDescription>&
    GetAttributeDescriptions() const;

    /**
     * \brief Get shader stage from a filename.
     * \param filename 
     * \return 
     */
    static VkShaderStageFlagBits GetShaderStage(
        const nonstd::string_view& filename);

    /**
     * \brief Get FileType from a shader stage.
     * \param stageFlag 
     * \return 
     */
    static FileType ShaderStageToFileType(VkShaderStageFlags stageFlag);

    /**
     * \brief Turn the shader into a readable string.
     * \return 
     */
    std::string ToString() const;

private:
    static void IncrementDescriptorPool(
        std::map<VkDescriptorType, uint32_t>& descriptorPoolCounts,
        VkDescriptorType type
    );

    void LoadUniformBlock(json uniformBlockJson);

    void LoadUniform(json uniformJson);

    void LoadVertexAttribute(json uniformJson);

    std::map<XXH64_hash_t, Uniform> uniform_;
    std::map<XXH64_hash_t, UniformBlock> uniformBlocks_;
    std::map<XXH64_hash_t, Attribute> attribute_;

    std::array<std::experimental::optional<uint32_t>, kLocalSizeDimension>
    localSizes_;

    std::map<XXH64_hash_t, uint32_t> descriptorLocations_;
    std::map<XXH64_hash_t, uint32_t> descriptorSizes_;

    std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayout_;
    uint32_t lastDescriptorBinding_;
    std::vector<VkDescriptorPoolSize> descriptorPools_;
    std::map<uint32_t, VkDescriptorType> descriptorTypes_;
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions_;

    inline static const char kAxes[] = {'X', 'Y', 'Z'};
};
} //namespace graphics
} //namespace poke
