#include <GraphicsEngine/Shaders/shader_attribute.h>

namespace poke {
namespace graphics {
Attribute::Attribute(
    const int32_t set,
    const int32_t location,
    const int32_t size,
    const int32_t glType)
    : set_(set),
      location_(location),
      size_(size),
      glType_(glType) {}

bool Attribute::operator==(const Attribute& other) const
{
    return set_ == other.set_ && location_ == other.location_ && size_ == other.
           size_ && glType_ == other.
           glType_;
}

bool Attribute::operator!=(const Attribute& other) const
{
    return !(*this == other);
}

std::string Attribute::ToString() const
{
    std::stringstream stream;
    stream << "VertexAttribute(set " << set_ << "', location " << location_ <<
        ", size " << size_ << ", glType " << glType_ << ")";
    return stream.str();
}

Attribute Attribute::CreateFromJson(json jsonAttribute)
{
    int32_t set = -1;
    if (CheckJsonNumber(jsonAttribute, "set")) { set = jsonAttribute["set"]; }

    int32_t location = -1;
    if (CheckJsonNumber(jsonAttribute, "location")) {
        location = jsonAttribute["location"];
    }

    int32_t size = -1;
    if (CheckJsonNumber(jsonAttribute, "size")) {
        size = jsonAttribute["size"];
    }

    int32_t glType = -1;
    if (CheckJsonNumber(jsonAttribute, "glType")) {
        glType = jsonAttribute["glType"];
    }

    return Attribute(set, location, size, glType);
}
} //namespace graphics
} //namespace poke
