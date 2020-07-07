#include <ResourcesManager/MeshManagers/mesh_sphere_gizmo.h>

#include <Math/matrix.h>

namespace poke {
std::unique_ptr<MeshSphereGizmo> MeshSphereGizmo::Create(
    const float radius,
    int nbPoint)
{
    return std::make_unique<MeshSphereGizmo>(
        true,
        radius,
        nbPoint);
}

MeshSphereGizmo::MeshSphereGizmo(
    const bool load,
    const float radius,
    const int nbPoint)
    : radius_(radius),
      nbPoint_(nbPoint) { if (load) { MeshSphereGizmo::Load(); } }

void MeshSphereGizmo::Load()
{
    if (radius_ <= 0.0f) { return; }

    std::vector<graphics::VertexMesh> vertices;

    auto base = math::Vec3(0.5f, 0.0f, 0.0f);

    for (uint32_t j = 0; j < nbPoint_; j++) {
        const auto angle = 0.0f + j * (math::kPi * 2.0f / nbPoint_);
        auto rotation = math::Matrix3(
            math::Vec3(cos(angle), sin(angle), 0.0f),
            math::Vec3(-sin(angle), cos(angle), 0.0f),
            math::Vec3(0.0f, 0.0f, 1.0f));

        const auto axe = math::Vec3(
            rotation[0].x * base.x - rotation[0].y * base.y,
            rotation[0].y * base.x + rotation[1].y * base.y,
            base.z);

        vertices.emplace_back(
            graphics::VertexMesh(axe, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}));
    }

    base = math::Vec3(0.5f, 0.0f, 0.0f);

    for (uint32_t j = 0; j < nbPoint_; j++) {
        const auto angle = 0.0f + j * (math::kPi * 2.0f / nbPoint_);
        auto rotation = math::Matrix3(
            math::Vec3(cos(angle), 0.0f, -sin(angle)),
            math::Vec3(0.0f, 1.0f, 0.0f),
            math::Vec3(sin(angle), 0, cos(angle)));

        const auto axe = math::Vec3(
            rotation[0].x * base.x + rotation[2].x * base.z,
            base.y,
            -rotation[0].z * base.x + rotation[2].z * base.z);

        vertices.emplace_back(
            graphics::VertexMesh(axe, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}));
    }

    base = math::Vec3(0.0f, 0.5f, 0.0f);

    for (uint32_t j = 0; j < nbPoint_; j++) {
        const auto angle = 0.0f + j * (math::kPi * 2.0f / nbPoint_);
        auto rotation = math::Matrix3(
            math::Vec3(1.0f, 0.0f, 0.0f),
            math::Vec3(0.0f, cos(angle), -sin(angle)),
            math::Vec3(0.0f, sin(angle), cos(angle)));

        const auto axe = math::Vec3(
            base.x,
            rotation[1].y * base.y - rotation[2].y * base.z,
            -rotation[1].z * base.y + rotation[2].z * base.z);

        vertices.emplace_back(
            graphics::VertexMesh(axe, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}));
    }

    std::vector<uint32_t> indices;
    //first loop
    indices.reserve(nbPoint_ * 3.0f);
    for (uint32_t i = 0; i < nbPoint_ + 1; i++) {
        indices.push_back(i % nbPoint_);
    }
    //second loop
    for (uint32_t i = 1; i < nbPoint_ + 1; i++) {
        indices.push_back(i % nbPoint_ + nbPoint_);
    }
    //Quarter of a circle
    for (auto i = 1; i < nbPoint_ / 4.0f; i++) { indices.push_back(i); }

    //third circle
    for (uint32_t i = 0; i <= nbPoint_; i++) {
        indices.push_back(i % nbPoint_ + nbPoint_ * 2.0f);
    }

    Initialize(vertices, indices);
}
} //namespace poke
