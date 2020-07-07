#include <GraphicsEngine/Shaders/shader.h>

#include <iostream>

#include <GraphicsEngine/Images/image_2d.h>
#include <GraphicsEngine/Buffers/uniform_buffer.h>
#include <GraphicsEngine/Buffers/storage_buffer.h>
#include <GraphicsEngine/Images/image_cube.h>
#include <CoreEngine/cassert.h>
#include <Utility/file_system.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>
#include "GraphicsEngine/vulkan_error_handler.h"

namespace poke {
namespace graphics {

Shader::Shader() : lastDescriptorBinding_(0) {}

void Shader::ProcessShader()
{
    std::map<VkDescriptorType, uint32_t> descriptorPoolCounts;

    // Process to descriptors.
    for (const auto& uniformBlock : uniformBlocks_) {
        auto descriptorType = VK_DESCRIPTOR_TYPE_MAX_ENUM;

        switch (uniformBlock.second.GetType()) {
        case UniformBlock::Type::UNIFORM:
            descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorSetLayout_.emplace_back(
                UniformBuffer::
                GetDescriptorSetLayout(
                    static_cast<uint32_t>(uniformBlock.second.GetBinding()),
                    descriptorType,
                    uniformBlock.second.GetStageFlags()));
            break;
        case UniformBlock::Type::STORAGE:
            descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            descriptorSetLayout_.emplace_back(
                StorageBuffer::
                GetDescriptorSetLayout(
                    static_cast<uint32_t>(uniformBlock.second.GetBinding()),
                    descriptorType,
                    uniformBlock.second.GetStageFlags()));
            break;
        case UniformBlock::Type::PUSH:
            // Push constants are described in the pipeline.
            break;
        default:
            break;
        }

        IncrementDescriptorPool(descriptorPoolCounts, descriptorType);
        descriptorLocations_.emplace(
            uniformBlock.first,
            uniformBlock.second.GetBinding());
        descriptorSizes_.emplace(
            uniformBlock.first,
            uniformBlock.second.GetSize());
    }

    for (const auto& uniform : uniform_) {
        auto descriptorType = VK_DESCRIPTOR_TYPE_MAX_ENUM;

        switch (uniform.second.GetGlType()) {
        case 0x8B5E: // GL_SAMPLER_2D
        case 0x904D: // GL_IMAGE_2D
        case 0x9108: // GL_SAMPLER_2D_MULTISAMPLE
        case 0x9055: // GL_IMAGE_2D_MULTISAMPLE
            descriptorType = uniform.second.IsWriteOnly() ?
                                 VK_DESCRIPTOR_TYPE_STORAGE_IMAGE :
                                 VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorSetLayout_.emplace_back(
                Image2d::GetDescriptorSetLayout(
                    static_cast<uint32_t>(uniform.second.GetBinding()),
                    descriptorType,
                    uniform.second.GetStageFlags()));
            break;
        case 0x8B60: // GL_SAMPLER_CUBE
        case 0x9050: // GL_IMAGE_CUBE
            descriptorType = uniform.second.IsWriteOnly() ?
                                 VK_DESCRIPTOR_TYPE_STORAGE_IMAGE :
                                 VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorSetLayout_.emplace_back(
                ImageCube::GetDescriptorSetLayout(
                    static_cast<uint32_t>(uniform.second.GetBinding()),
                    descriptorType,
                    uniform.second.GetStageFlags()));
            break;
        default:
            break;
        }

        IncrementDescriptorPool(descriptorPoolCounts, descriptorType);
        descriptorLocations_.
            emplace(uniform.first, uniform.second.GetBinding());
        descriptorSizes_.emplace(uniform.first, uniform.second.GetSize());
    }

    for (const auto& descriptor : descriptorPoolCounts) {
        VkDescriptorPoolSize descriptorPoolSize = {};
        descriptorPoolSize.type = descriptor.first;
        descriptorPoolSize.descriptorCount = descriptor.second;
        descriptorPools_.emplace_back(descriptorPoolSize);
    }

    // FIXME: This is a AMD workaround that works on Nvidia too
    descriptorPools_ = std::vector<VkDescriptorPoolSize>(6);
    descriptorPools_[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorPools_[0].descriptorCount = 4096;
    descriptorPools_[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorPools_[1].descriptorCount = 2048;
    descriptorPools_[2].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    descriptorPools_[2].descriptorCount = 2048;
    descriptorPools_[3].type = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
    descriptorPools_[3].descriptorCount = 2048;
    descriptorPools_[4].type = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
    descriptorPools_[4].descriptorCount = 2048;
    descriptorPools_[5].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptorPools_[5].descriptorCount = 2048;

    // Sort descriptors by binding.
    std::sort(
        descriptorSetLayout_.begin(),
        descriptorSetLayout_.end(),
        [](
        const VkDescriptorSetLayoutBinding& l,
        const VkDescriptorSetLayoutBinding& r) {
            return l.binding < r.binding;
        });

    // Gets the last descriptors binding.
    if (!descriptorSetLayout_.empty()) {
        lastDescriptorBinding_ = descriptorSetLayout_.back().binding;
    }

    // Gets the descriptor type for each descriptor.
    for (const auto& descriptor : descriptorSetLayout_) {
        descriptorTypes_.emplace(
            descriptor.binding,
            descriptor.descriptorType);
    }

    // Process attribute descriptions.
    uint32_t currentOffset = 4;

    for (const auto& attribute : attribute_) {
        VkVertexInputAttributeDescription attributeDescription = {};
        attributeDescription.location = static_cast<uint32_t>(attribute
                                                              .second.
                                                              GetLocation());
        attributeDescription.binding = 0;
        attributeDescription.format = GlTypeToVk(attribute.second.GetGlType());
        attributeDescription.offset = currentOffset;
        attributeDescriptions_.emplace_back(attributeDescription);
        currentOffset += attribute.second.GetSize();
    }
}

VkFormat Shader::GlTypeToVk(const int32_t type)
{
    switch (type) {
    case 0x1406: // GL_FLOAT
        return VK_FORMAT_R32_SFLOAT;
    case 0x8B50: // GL_FLOAT_VEC2
        return VK_FORMAT_R32G32_SFLOAT;
    case 0x8B51: // GL_FLOAT_VEC3
        return VK_FORMAT_R32G32B32_SFLOAT;
    case 0x8B52: // GL_FLOAT_VEC4
        return VK_FORMAT_R32G32B32A32_SFLOAT;
    case 0x1404: // GL_INT
        return VK_FORMAT_R32_SINT;
    case 0x8B53: // GL_INT_VEC2
        return VK_FORMAT_R32G32_SINT;
    case 0x8B54: // GL_INT_VEC3
        return VK_FORMAT_R32G32B32_SINT;
    case 0x8B55: // GL_INT_VEC4
        return VK_FORMAT_R32G32B32A32_SINT;
    case 0x1405: // GL_UNSIGNED_INT
        return VK_FORMAT_R32_SINT;
    case 0x8DC6: // GL_UNSIGNED_INT_VEC2
        return VK_FORMAT_R32G32_SINT;
    case 0x8DC7: // GL_UNSIGNED_INT_VEC3
        return VK_FORMAT_R32G32B32_SINT;
    case 0x8DC8: // GL_UNSIGNED_INT_VEC4
        return VK_FORMAT_R32G32B32A32_SINT;
    default:
        return VK_FORMAT_UNDEFINED;
    }
}

uint32_t Shader::GetDescriptorLocation(const nonstd::string_view& name) const
{
    const auto hash = math::HashString(name);
    const auto it = descriptorLocations_.find(hash);

    cassert(it != descriptorLocations_.end(), "Descriptor doesn't exist");

    return descriptorLocations_.at(hash);
}

uint32_t Shader::GetDescriptorLocation(const XXH64_hash_t& descriptorHash) const
{
	const auto it = descriptorLocations_.find(descriptorHash);

    if(it == descriptorLocations_.end()) {
		std::cout << "Shader::GetDescriptorLocation\n";
    }

    //TODO(@Nico) This a fix made for forcing an update of a materials when a descriptor(texture) is added to a material that previously hadn't this descriptor.
	if (it == descriptorLocations_.end()) { return -1; }
	cassert(it != descriptorLocations_.end(), "Descriptor doesn't exist");

	return descriptorLocations_.at(descriptorHash);
}

uint32_t Shader::GetDescriptorSize(const nonstd::string_view& name) const
{
    const auto hash = math::HashString(name);
    const auto it = descriptorSizes_.find(hash);

    cassert(it != descriptorSizes_.end(), "Descriptor doesn't exist");

    return descriptorSizes_.at(hash);
}

const Uniform& Shader::GetUniform(const nonstd::string_view& name) const
{
    const auto hash = math::HashString(name);
    const auto it = uniform_.find(hash);

    cassert(it != uniform_.end(), "Uniform doesn't exist");

    return uniform_.at(hash);
}

const UniformBlock& Shader::GetUniformBlock(
    const nonstd::string_view& name) const
{
    const auto hash = math::HashString(name);
    const auto it = uniformBlocks_.find(hash);

    cassert(it != uniformBlocks_.end(), "Uniform block doesn't exist");

    return uniformBlocks_.at(hash);
}

const UniformBlock& Shader::GetUniformBlock(const XXH64_hash_t descriptorHash) const
{
	const auto it = uniformBlocks_.find(descriptorHash);

	cassert(it != uniformBlocks_.end(), "Uniform block doesn't exist");

	return uniformBlocks_.at(descriptorHash);
}

const Attribute& Shader::GetAttribute(const nonstd::string_view& name) const
{
    const auto hash = math::HashString(name);
    const auto it = attribute_.find(hash);

    cassert(it != attribute_.end(), "Attribute doesn't exist");

    return attribute_.at(hash);
}

std::vector<VkPushConstantRange> Shader::GetPushConstantRanges() const
{
    std::vector<VkPushConstantRange> pushConstantRanges;
    uint32_t currentOffset = 0;

    for (const auto& uniformBlock : uniformBlocks_) {
        if (uniformBlock.second.GetType() != UniformBlock::Type::PUSH) {
            continue;
        }

        VkPushConstantRange pushConstantRange = {};
        pushConstantRange.stageFlags = uniformBlock.second.GetStageFlags();
        pushConstantRange.offset = currentOffset;
        pushConstantRange.size = static_cast<uint32_t>(uniformBlock
                                                       .second.GetSize());
        pushConstantRanges.emplace_back(pushConstantRange);
        currentOffset += pushConstantRange.size;
    }

    return pushConstantRanges;
}

const std::map<XXH64_hash_t, Uniform>& Shader::GetUniforms() const
{
    return uniform_;
}

const std::map<XXH64_hash_t, UniformBlock>& Shader::GetUniformBlocks() const
{
    return uniformBlocks_;
}

const std::map<XXH64_hash_t, Attribute>& Shader::GetAttributes() const
{
    return attribute_;
}

const std::array<std::experimental::optional<uint32_t>, Shader::
                 kLocalSizeDimension>& Shader::GetLocalSizes() const
{
    return localSizes_;
}

const std::vector<VkDescriptorSetLayoutBinding>& Shader::
GetDescriptorSetLayouts() const { return descriptorSetLayout_; }

const std::vector<VkDescriptorPoolSize>& Shader::GetDescriptorPools() const
{
    return descriptorPools_;
}

VkDescriptorType Shader::GetDescriptorType(const uint32_t location) const
{
    const auto it = descriptorTypes_.find(location);

    cassert(it != descriptorTypes_.end(), "Descriptor doesn't exist");

    return descriptorTypes_.at(location);
}

const std::vector<VkVertexInputAttributeDescription>& Shader::
GetAttributeDescriptions() const { return attributeDescriptions_; }

std::string Lowercase(std::string str)
{
    std::transform(str.begin(), str.end(), str.begin(), tolower);
    return str;
}

std::string FileSuffix(const std::string& path)
{
    const auto start = path.find_last_of('/');
    auto suffix = path.substr(start + 1);
    const auto end = suffix.find_last_of('.');
    suffix = suffix.substr(end);

    return suffix;
}

VkShaderStageFlagBits Shader::GetShaderStage(
    const nonstd::string_view& filename)
{
    const auto fileExt = Lowercase(
        FileSuffix(static_cast<std::string>(filename)));

    if (fileExt == ".comp") { return VK_SHADER_STAGE_COMPUTE_BIT; }

    if (fileExt == ".vert") { return VK_SHADER_STAGE_VERTEX_BIT; }

    if (fileExt == ".frag") { return VK_SHADER_STAGE_FRAGMENT_BIT; }

    if (fileExt == ".tese") {
        return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
    }

    if (fileExt == ".tesc") { return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT; }

    if (fileExt == ".geom") { return VK_SHADER_STAGE_GEOMETRY_BIT; }

    std::cout << "no shader extension for " << fileExt << "\n";
    return VK_SHADER_STAGE_ALL;
}

FileType Shader::ShaderStageToFileType(const VkShaderStageFlags stageFlag)
{
    if (stageFlag == VK_SHADER_STAGE_COMPUTE_BIT) { return FileType::COMP; }

    if (stageFlag == VK_SHADER_STAGE_VERTEX_BIT) { return FileType::VERT; }

    if (stageFlag == VK_SHADER_STAGE_FRAGMENT_BIT) { return FileType::FRAG; }

    if (stageFlag == VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT) {
        cassert(false, "not implemented");
    }

    if (stageFlag == VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT) {
        cassert(false, "not implemented");
    }

    if (stageFlag == VK_SHADER_STAGE_GEOMETRY_BIT) {
        cassert(false, "not implemented");
    }

    cassert(false, "not implemented");
}

VkShaderModule Shader::ProcessShader(
    const json& jsonShader,
    const nonstd::string_view& filename,
    const VkShaderStageFlags stageFlag)
{
    if (CheckJsonExists(jsonShader, "local_size_x")) {
        localSizes_[0].emplace(jsonShader["local_size_x"]);
    }

    if (CheckJsonExists(jsonShader, "local_size_y")) {
        localSizes_[1].emplace(jsonShader["local_size_y"]);
    }

    if (CheckJsonExists(jsonShader, "local_size_z")) {
        localSizes_[2].emplace(jsonShader["local_size_z"]);
    }

    //load uniform block
    if (CheckJsonExists(jsonShader, "uniforms_blocks")) {
        for (const auto& uniformBlockJson : jsonShader["uniforms_blocks"]) {
            LoadUniformBlock(uniformBlockJson);
        }
    }

    //load uniforms
    if (CheckJsonExists(jsonShader, "uniforms")) {
        for (const auto& uniformJson : jsonShader["uniforms"]) {
            LoadUniform(uniformJson);
        }
    }

    //load attributes
    if (CheckJsonExists(jsonShader, "attributes")) {
        for (const auto& uniformJson : jsonShader["attributes"]) {
            LoadVertexAttribute(uniformJson);
        }
    }

    std::string file = PokFileSystem::ReadFile(
        static_cast<std::string>(filename),
        ShaderStageToFileType(stageFlag));

    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = file.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(file.data());

    VkShaderModule shaderModule = nullptr;
    CheckVk(vkCreateShaderModule(GraphicsEngineLocator::Get().GetLogicalDevice(), &createInfo, nullptr, &shaderModule));
    return shaderModule;
}

std::string Shader::ToString() const
{
    std::stringstream stream;

    if (!attribute_.empty()) {
        stream << "Vertex Attributes: \n";

        for (const auto& attribute : attribute_) {
            stream << "  - " << attribute.first << ": " << attribute
                                                           .second.ToString() <<
                "\n";
        }
    }

    if (!uniform_.empty()) {
        stream << "Uniforms: \n";

        for (const auto& uniform : uniform_) {
            stream << "  - " << uniform.first << ": " << uniform
                                                         .second.ToString() <<
                "\n";
        }
    }

    if (!uniformBlocks_.empty()) {
        stream << "Uniform Blocks: \n";

        for (const auto& uniformBlock : uniformBlocks_) {
            stream << "  - " << uniformBlock.first << ": " << uniformBlock
                                                              .second.
                                                              ToString()
                << " \n";

            for (const auto& uniform : uniformBlock.second.GetUniforms()) {
                stream << "	- " << uniform.first << ": " << uniform
                                                            .second.ToString()
                    << " \n";
            }
        }
    }

    for (uint32_t dim = 0; dim < localSizes_.size(); dim++) {
        if (localSizes_[dim]) {
            stream << "Local size " << kAxes[dim] << ": " << *localSizes_[dim]
                <<
                " \n";
        }
    }

    return stream.str();
}

void Shader::IncrementDescriptorPool(
    std::map<VkDescriptorType, uint32_t>& descriptorPoolCounts,
    VkDescriptorType type
)
{
    if (type == VK_DESCRIPTOR_TYPE_MAX_ENUM) { return; }

    const auto it = descriptorPoolCounts.find(type);

    if (it != descriptorPoolCounts.end()) { it->second++; } else {
        descriptorPoolCounts.emplace(type, 1);
    }
}

void Shader::LoadUniformBlock(json uniformBlockJson)
{
    const auto name = uniformBlockJson["name"].get<std::string>();
    uniformBlocks_.emplace(
		math::HashString(name),
        UniformBlock::CreateFromJson(uniformBlockJson));
}

void Shader::LoadUniform(json uniformJson)
{
    const auto name = uniformJson["name"].get<std::string>();
    uniform_.emplace(math::HashString(name), Uniform::CreateFromJson(uniformJson));
}

void Shader::LoadVertexAttribute(json uniformJson)
{
    const auto name = uniformJson["name"].get<std::string>();
    attribute_.emplace(math::HashString(name), Attribute::CreateFromJson(uniformJson));
}
} //namespace graphics
} //namespace poke
