#include <GraphicsEngine/Shaders/shader_uniform_block.h>
#include <CoreEngine/cassert.h>
#include <Math/math.h>
#include "math/hash.h"

namespace poke {
namespace graphics {
UniformBlock::UniformBlock(
    const int32_t binding,
    const int32_t size,
    const VkShaderStageFlags stageFlags,
    const Type type)
    : binding_(binding),
      size_(size),
      stageFlags_(stageFlags),
      type_(type) {}

const std::map<XXH64_hash_t, Uniform>& UniformBlock::GetUniforms() const
{
    return uniforms_;
}

const Uniform& UniformBlock::GetUniform(const XXH64_hash_t uniformHash) const
{
    const auto it = uniforms_.find(uniformHash);

    cassert(it != uniforms_.end(), "Uniform doesn't exist " + std::to_string(uniformHash) + "\n" + ToString());

    return uniforms_.at(uniformHash);
}

void UniformBlock::AddUniform(const XXH64_hash_t uniformHash, Uniform uniform)
{
    uniforms_.emplace(std::pair<XXH64_hash_t, Uniform>(uniformHash, uniform));
}

bool UniformBlock::operator==(const UniformBlock& other) const
{
    return binding_ == other.binding_ && size_ == other.size_ && stageFlags_ ==
           other.stageFlags_ && type_ ==
           other.type_ && uniforms_ == other.uniforms_;
}

bool UniformBlock::operator!=(const UniformBlock& other) const
{
    return !(*this == other);
}

std::string UniformBlock::ToString() const
{
    std::stringstream stream;
    stream << "UniformBlock(binding " << binding_ << ", size " << size_ <<
        ", type " << static_cast<uint32_t>(
            type_) << ")";
    return stream.str();
}

UniformBlock UniformBlock::CreateFromJson(json jsonUniformBlock)
{
    int32_t binding = -1;
    if (CheckJsonNumber(jsonUniformBlock, "binding")) {
        binding = jsonUniformBlock["binding"];
    }

    int32_t size = -1;
    if (CheckJsonNumber(jsonUniformBlock, "size")) {
        size = jsonUniformBlock["size"];
    }

    VkShaderStageFlags stageFlags = -1;
    if (CheckJsonNumber(jsonUniformBlock, "stageFlags")) {
        stageFlags = static_cast<VkShaderStageFlags>(jsonUniformBlock[
            "stageFlags"]);
    }

    Type type = Type::UNIFORM;
    if (CheckJsonNumber(jsonUniformBlock, "type")) {
        type = static_cast<Type>(jsonUniformBlock["type"]);
    }

    UniformBlock uniformBlock(binding, size, stageFlags, type);

    for (auto jsonUniform : jsonUniformBlock["uniforms"]) {
		std::string name = jsonUniform["name"].get<std::string>();
		auto hash = math::HashString(name);
        uniformBlock.AddUniform(
            hash,
            Uniform::CreateFromJson(jsonUniform));
    }

    return uniformBlock;
}
} //namespace graphics
} //namespace poke
