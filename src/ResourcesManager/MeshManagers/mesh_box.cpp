#include <ResourcesManager/MeshManagers/mesh_box.h>

namespace poke {
std::unique_ptr<MeshBox> MeshBox::
Create(math::Vec3 extents) { return std::make_unique<MeshBox>(true, extents); }

MeshBox::MeshBox(
    const bool load,
    const math::Vec3 extents
) : extents_(extents) { if (load) { MeshBox::Load(); } }

void MeshBox::Load()
{
    if (extents_.x <= 0.0f || extents_.y <= 0.0f || extents_.z <= 0.0f) {
        return;
    }

    auto vertices = kVertices;
    for (auto& vertex : vertices) {
        vertex.position.x *= extents_.x;
        vertex.position.y *= extents_.y;
        vertex.position.z *= extents_.z;
    }

    Initialize(kVertices, kIndices);
}
} //namespace poke
