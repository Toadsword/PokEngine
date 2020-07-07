#include <ResourcesManager/MeshManagers/mesh_plane.h>

namespace poke {
std::unique_ptr<MeshPlane> MeshPlane::Create(
    math::Vec2 extent,
    int patchSize)
{
    return std::make_unique<MeshPlane>(true, extent, patchSize);
}

MeshPlane::MeshPlane(
    const bool load,
    const math::Vec2 extent,
    const int patchSize
)
    : extent_(extent),
      patchSize_(patchSize) { if (load) { MeshPlane::Load(); } }

void MeshPlane::Load()
{
    std::vector<graphics::VertexMesh> vertices;
    std::vector<uint32_t> indices;

    for (auto x = 0; x <= patchSize_; x++) {
        for (auto y = 0; y <= patchSize_; y++) {
            vertices.emplace_back(
                math::Vec3(
                    static_cast<float>(x) / static_cast<float>(patchSize_) *
                    extent_.x - extent_.x / 2.0f,
                    0.0,
                    static_cast<float>(y) / static_cast<float>(patchSize_) *
                    extent_.y - extent_.y / 2.0f),
                math::Vec2(
                    static_cast<float>(x) / static_cast<float>(patchSize_),
                    static_cast<float>(y) / static_cast<float>(patchSize_)),
                math::Vec3(0.0f, 1.0f, 0.0f));
        }
    }


    auto vert = 0;
    auto tris = 0;
    indices.resize(patchSize_ * patchSize_ * 6);
    for (auto x = 0; x < patchSize_; x++) {
        for (auto y = 0; y < patchSize_; y++) {
            indices[tris] = vert;
            indices[tris + 2] = vert + patchSize_ + 1;
            indices[tris + 1] = vert + 1;
            indices[tris + 4] = vert + 1;
            indices[tris + 3] = vert + patchSize_ + 1;
            indices[tris + 5] = vert + patchSize_ + 2;
            vert++;
            tris += 6;
        }
        vert++;
    }
    Initialize(vertices, indices);
}
} //namespace poke 
