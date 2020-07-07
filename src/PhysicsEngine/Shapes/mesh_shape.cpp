#include <PhysicsEngine/Shapes/mesh_shape.h>

#include <Math/tranform.h>

namespace poke::physics {

math::Vec3 MeshShape::GetMeshExtent() const { return meshExtent_; }

void MeshShape::SetMeshExtent(const math::Vec3 meshExtent)
{
    meshExtent_ = meshExtent;
}

math::Vec3 MeshShape::GetOffset() const { return positionOffset_; }

void MeshShape::SetOffset(const math::Vec3 offset) { positionOffset_ = offset; }

void MeshShape::SetFromJson(json shapeJson)
{
    meshExtent_.SetFromJson(shapeJson["meshExtent"]);
    positionOffset_.SetFromJson(shapeJson["positionOffset"]);
}

json MeshShape::ToJson() const
{
    json shapeJson;
    shapeJson["meshExtent"] = meshExtent_.ToJson();
    shapeJson["positionOffset"] = positionOffset_.ToJson();
    return shapeJson;
}

MeshShape::MeshShape()
    : meshExtent_({0, 0, 0}),
      positionOffset_({0, 0, 0}) { }

MeshShape::MeshShape(graphics::Mesh& mesh)
    : meshExtent_(mesh.GetExtent()),
      positionOffset_(mesh.GetPositionOffset()) {}

bool MeshShape::operator==(const MeshShape& rhs) const
{
    return meshExtent_ == rhs.meshExtent_
           && positionOffset_ == rhs.positionOffset_;
}

bool MeshShape::operator!=(const MeshShape& rhs) const
{
    return !(*this == rhs);
}

AABB MeshShape::ComputeAABB(
    const math::Vec3 position,
    const math::Vec3 scale,
    math::Vec3 rotation) const
{
    const math::Vec3 currentExtent = {
        scale.x * meshExtent_.x,
        scale.y * meshExtent_.y,
        scale.z * meshExtent_.z
    };

    return {position - positionOffset_, currentExtent};
}

AABB MeshShape::ComputeAABB(const math::Transform& transform) const
{
	return ComputeAABB(transform.GetLocalPosition(), transform.GetLocalScale(), transform.GetLocalRotation());
}

ShapeType MeshShape::GetType() const { return ShapeType::MESH; }

std::unique_ptr<IShape> MeshShape::Clone()
{
    return std::make_unique<MeshShape>(*this);
}
} //namespace poke::physics
