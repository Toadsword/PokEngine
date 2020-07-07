#include <ResourcesManager/MeshManagers/mesh_box_gizmo.h>

namespace poke {
std::unique_ptr<MeshBoxGizmo> MeshBoxGizmo::Create()
{
    return std::make_unique<MeshBoxGizmo>(true);
}

MeshBoxGizmo::MeshBoxGizmo(const bool load)
{
    if (load) { MeshBoxGizmo::Load(); }
}

void MeshBoxGizmo::Load() { Initialize(kVertices, kIndices); }
} //namespace poke
