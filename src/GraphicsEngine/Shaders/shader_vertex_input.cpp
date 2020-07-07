#include <GraphicsEngine/Shaders/shader_vertex_input.h>

namespace poke {
namespace graphics {
VertexInput::VertexInput(
    const uint32_t binding,
    const std::vector<VkVertexInputBindingDescription> bindingDescriptions,
    const std::vector<VkVertexInputAttributeDescription> attributeDescriptions)
    : binding_(binding),
      bindingDescriptions_(bindingDescriptions),
      attributeDescription_(attributeDescriptions) { }

bool VertexInput::operator<(const VertexInput& other) const
{
    return binding_ < other.binding_;
}

} //namespace graphics
} //namespace poke
