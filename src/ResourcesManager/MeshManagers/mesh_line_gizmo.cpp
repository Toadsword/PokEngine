#include <ResourcesManager/MeshManagers/mesh_line_gizmo.h>

namespace poke {
std::unique_ptr<MeshLineGizmo> MeshLineGizmo::Create(
    math::Vec3 point1,
    math::Vec3 point2
) { return std::make_unique<MeshLineGizmo>(true, point1, point2); }

MeshLineGizmo::MeshLineGizmo(
    const bool load,
    const math::Vec3 point1,
    const math::Vec3 point2
)
    : point1_(point1),
      point2_(point2) { if (load) { MeshLineGizmo::Load(); } }

void MeshLineGizmo::Load()
{
    std::vector<graphics::VertexMesh> vertices;
    vertices.reserve(2);

    vertices.emplace_back(
        point1_,
        math::Vec2(0.0f, 0.0f),
        math::Vec3(0.0f, 0.0f, 0.0f));

    vertices.emplace_back(
        point2_,
        math::Vec2(0.0f, 0.0f),
        math::Vec3(0.0f, 0.0f, 0.0f));

    const std::vector<uint32_t> indices = {0, 1};

    Initialize(vertices, indices);
}
} //namespace poke
