#include <GraphicsEngine/Shaders/shader_uniform.h>

namespace poke {
namespace graphics {
Uniform::Uniform(
    const int32_t binding,
    const int32_t offset,
    const int32_t size,
    const int32_t glType,
    const bool readOnly,
    const bool writeOnly,
    const VkShaderStageFlags stageFlags)
    : binding_(binding),
      offset_(offset),
      size_(size),
      glType_(glType),
      readOnly_(readOnly),
      writeOnly_(writeOnly),
      stageFlags_(stageFlags) {}

bool Uniform::operator==(const Uniform& other) const
{
    return binding_ == other.binding_ && offset_ == other.offset_ && size_ ==
           other.size_ && glType_ == other.
           glType_ && readOnly_ == other.readOnly_
           && writeOnly_ == other.writeOnly_ && stageFlags_ == other.
           stageFlags_;
}

bool Uniform::operator!=(const Uniform& other) const
{
    return !(*this == other);
}

std::string Uniform::ToString() const
{
    std::stringstream stream;
    stream << "Uniform(binding " << binding_ << ", offset " << offset_ <<
        ", size " << size_ << ", glType " <<
        glType_ << ")";
    return stream.str();
}

Uniform Uniform::CreateFromJson(nlohmann::json jsonAttribute)
{
    int32_t binding = -1;
    if (CheckJsonNumber(jsonAttribute, "binding")) {
        binding = jsonAttribute["binding"];
    }

    int32_t offset = -1;
    if (CheckJsonNumber(jsonAttribute, "offset")) {
        offset = jsonAttribute["offset"];
    }

    int32_t size = -1;
    if (CheckJsonNumber(jsonAttribute, "size")) {
        size = jsonAttribute["size"];
    }

    int32_t glType = -1;
    if (CheckJsonNumber(jsonAttribute, "glType")) {
        glType = jsonAttribute["glType"];
    }

    bool readOnly = false;
    if (CheckJsonExists(jsonAttribute, "readOnly")) {
        readOnly = jsonAttribute["readOnly"].get<bool>();
    }

    bool writeOnly = false;
    if (CheckJsonExists(jsonAttribute, "writeOnly")) {
        writeOnly = jsonAttribute["writeOnly"].get<bool>();
    }

    VkShaderStageFlags stageFlags = -1;
    if (CheckJsonNumber(jsonAttribute, "stageFlags")) {
        stageFlags = jsonAttribute["stageFlags"];
    }

    return Uniform(
        binding,
        offset,
        size,
        glType,
        readOnly,
        writeOnly,
        stageFlags);
}
} //namespace graphics
} //namespace poke
