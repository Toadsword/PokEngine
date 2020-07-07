#include <ResourcesManager/MeshManagers/mesh_quad.h>

namespace poke {
std::unique_ptr<MeshQuad> MeshQuad::Create(
    math::Vec2 extent) { return std::make_unique<MeshQuad>(true, extent); }

MeshQuad::MeshQuad(const bool load, const math::Vec2 extent)
    : Mesh(),
      extent_(extent) { if (load) { MeshQuad::Load(); } }

void MeshQuad::Load()
{
    const std::vector<graphics::VertexMesh> vertices = {
        graphics::VertexMesh(
            math::Vec3(-extent_.x, -extent_.y, 0.0f),
            math::Vec2(0.0f, 0.0f),
            math::Vec3(0.0f, 0.0f, 0.0f)),
        graphics::VertexMesh(
            math::Vec3(extent_.x, -extent_.y, 0.0f),
            math::Vec2(1.0f, 0.0f),
            math::Vec3(0.0f, 0.0f, 0.0f)),
        graphics::VertexMesh(
            math::Vec3(extent_.x, extent_.y, 0.0f),
            math::Vec2(1.0f, 1.0f),
            math::Vec3(0.0f, 0.0f, 0.0f)),
        graphics::VertexMesh(
            math::Vec3(-extent_.x, extent_.y, 0.0f),
            math::Vec2(0.0f, 1.0f),
            math::Vec3(0.0f, 0.0f, 0.0f)),
    };

    const std::vector<uint32_t> indices = {0, 1, 2, 2, 3, 0};

    Initialize(vertices, indices);
}
} //namespace poke
