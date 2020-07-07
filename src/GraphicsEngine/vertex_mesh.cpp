#include <GraphicsEngine/vertex_mesh.h>

namespace poke {
namespace graphics {
VertexMesh::VertexMesh(
    const math::Vec3 pos,
    const math::Vec2 uv,
    const math::Vec3 normal)
    : position(pos),
      uv(uv),
      normal(normal) { }

VertexInput VertexMesh::GetVertexInput(const uint32_t binding)
{
    std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);

    //vertex input
    bindingDescriptions[0].binding = binding;
    bindingDescriptions[0].stride = sizeof(VertexMesh);
    bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    std::vector<VkVertexInputAttributeDescription> attributeDescriptions(3);

    //position
    attributeDescriptions[0].binding = binding;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(VertexMesh, position);

    //uv
    attributeDescriptions[1].binding = binding;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(VertexMesh, uv);

    //normal
    attributeDescriptions[2].binding = binding;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(VertexMesh, normal);

    return VertexInput(binding, bindingDescriptions, attributeDescriptions);
}

bool VertexMesh::operator==(const VertexMesh& other) const
{
    return position == other.position && uv == other.uv && normal == other.
           normal;
}
} //namespace graphics
} //namespace poke
