#include <PhysicsEngine/Shapes/sphere_shape.h>

#include <Math/tranform.h>

namespace poke::physics {

SphereShape::SphereShape(const math::Vec3 positionOffset, const float radius)
    : positionOffset_(positionOffset)
    , radius_(radius) { }

bool SphereShape::operator==(const SphereShape& rhs) const
{
    return positionOffset_ == rhs.positionOffset_
           && radius_ == rhs.radius_;
}

bool SphereShape::operator!=(const SphereShape& rhs) const
{
    return !(*this == rhs);
}

AABB SphereShape::ComputeAABB(
    const math::Vec3 position,
    const math::Vec3 scale,
    const math::Vec3 rotation) const {
    const float maxExtends = std::max({scale.x, scale.y, scale.z });
    return {position + positionOffset_, maxExtends * radius_};
}

AABB SphereShape::ComputeAABB(const math::Transform& transform) const
{
	return ComputeAABB(transform.GetLocalPosition(), transform.GetLocalScale(), transform.GetLocalRotation());
}

ShapeType SphereShape::GetType() const { return ShapeType::SPHERE; }

std::unique_ptr<IShape> SphereShape::Clone() {
    return std::make_unique<SphereShape>(*this);
}

math::Vec3 SphereShape::GetCenter() const { return positionOffset_; }

void SphereShape::SetCenter(const math::Vec3 center) { positionOffset_ = center; }

float SphereShape::GetRadius() const { return radius_; }

void SphereShape::SetRadius(const float radius) { radius_ = radius; }
void SphereShape::SetFromJson(json shapeJson)
{
	positionOffset_.SetFromJson(shapeJson["positionOffset"]);
	radius_ = shapeJson["radius"];
}
json SphereShape::ToJson() const
{
	json shapeJson;
	shapeJson["positionOffset"] = positionOffset_.ToJson();
	shapeJson["radius"] = radius_;
	return shapeJson;
}
} //namespace poke::physics
