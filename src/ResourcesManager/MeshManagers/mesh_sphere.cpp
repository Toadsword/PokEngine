#include <ResourcesManager/MeshManagers/mesh_sphere.h>

namespace poke {
std::unique_ptr<MeshSphere> MeshSphere::Create(
    const float radius,
    int latitudeBands,
    int longitudeBands)
{
    return std::make_unique<MeshSphere>(
        true,
        radius,
        latitudeBands,
        longitudeBands);
}

MeshSphere::MeshSphere(
    const bool load,
    const float radius,
    const int latitudeBands,
    const int longitudeBands)
    : radius_(radius),
      latitudeBands_(latitudeBands),
      longitudeBands_(longitudeBands) { if (load) { MeshSphere::Load(); } }

void MeshSphere::Load()
{
    if (radius_ <= 0) { return; }
    std::vector<graphics::VertexMesh> vertices;
    std::vector<uint32_t> indices;
    vertices.reserve((longitudeBands_ + 1) * (latitudeBands_ + 1));
    indices.reserve(longitudeBands_ * latitudeBands_ * 6);
    for (uint32_t i = 0; i < longitudeBands_ + 1; i++) {
        const auto iDivLong = static_cast<float>(i) / static_cast<float>(
                                  longitudeBands_);

        const auto theta = (i == 0 || i == longitudeBands_) ?
                               0.0f :
                               iDivLong * 2.0f * math::kPi;

        for (uint32_t j = 0; j < latitudeBands_ + 1; j++) {
            const auto jDivLat =
                static_cast<float>(j) / static_cast<float>(latitudeBands_);

            const auto phi = jDivLat * 2.0f * math::kPi;
            auto normal = math::Vec3(
                std::cos(phi) * std::sin(theta),
                std::cos(theta),
                std::sin(phi) * std::sin(theta));

            const auto position = normal * radius_;
            const auto uvs = math::Vec2(1.0f - jDivLat, 1.0f - iDivLong);
            vertices.emplace_back(graphics::VertexMesh(position, uvs, normal));
        }
    }
    for (uint32_t i = 0; i < longitudeBands_; i++) {
        for (uint32_t j = 0; j < latitudeBands_; j++) {
            auto first = j + ((latitudeBands_ + 1) * i);
            auto second = j + ((latitudeBands_ + 1) * (i + 1));
            indices.emplace_back(second + 1);
            indices.emplace_back(first + 1);
            indices.emplace_back(first);
            indices.emplace_back(second);
            indices.emplace_back(second + 1);
            indices.emplace_back(first);
        }
    }
    Initialize(vertices, indices);
}
} //namespace poke 
