#include <PhysicsEngine/Shapes/box_shape.h>

#include <Math/tranform.h>

namespace poke::physics {

BoxShape::BoxShape(
    const math::Vec3 positionOffset,
    const math::Vec3 extent)
    : positionOffset_(positionOffset),
      extent_(extent) { }

bool BoxShape::operator==(const BoxShape& other) const
{
    return positionOffset_ == other.positionOffset_
           && extent_ == other.extent_;
}

bool BoxShape::operator!=(const BoxShape& other) const
{
	return !(*this == other);
}

AABB BoxShape::ComputeAABB(
    const math::Vec3 position,
    const math::Vec3 scale,
    const math::Vec3 rotation) const
{
    math::Vec3 aabbExtent;

    const math::Matrix3 rotationMatrix = GetRotationMatrix(rotation);

    for (const auto& baseVertex : kVertices) {
        math::Vec3 vertex = rotationMatrix * baseVertex;
		vertex = math::Vec3{
			vertex.x * extent_.x * scale.x,
			vertex.y * extent_.y * scale.y,
			vertex.z * extent_.z * scale.z
		};

        //test
		aabbExtent.x = std::max(aabbExtent.x, abs(vertex.x));
		aabbExtent.y = std::max(aabbExtent.y, abs(vertex.y));
		aabbExtent.z = std::max(aabbExtent.z, abs(vertex.z));
    }

    return AABB{positionOffset_ + position, aabbExtent};
}

AABB BoxShape::ComputeAABB(const math::Transform& transform) const
{
	return ComputeAABB(transform.GetLocalPosition(), transform.GetLocalScale(), transform.GetLocalRotation());
}

ShapeType BoxShape::GetType() const { return ShapeType::BOX; }

std::unique_ptr<IShape> BoxShape::Clone()
{
    return std::make_unique<BoxShape>(*this);
}

math::Vec3 BoxShape::GetCenter() const { return positionOffset_; }

void BoxShape::SetCenter(const math::Vec3 center) { positionOffset_ = center; }

math::Vec3 BoxShape::GetExtent() const { return extent_; }

void BoxShape::SetExtent(const math::Vec3 extent) { extent_ = extent; }

void BoxShape::SetFromJson(json shapeJson)
{
    positionOffset_.SetFromJson(shapeJson["positionOffset"]);
    extent_.SetFromJson(shapeJson["extent"]);
}

json BoxShape::ToJson() const
{
    json shapeJson;
    shapeJson["positionOffset"] = positionOffset_.ToJson();
    shapeJson["extent"] = extent_.ToJson();
    return shapeJson;
}

math::Matrix3 BoxShape::GetRotationMatrix(math::Vec3 theta) const
{
    // Calculate rotation about x axis
    const math::Matrix3 rotationX = {
        {1, 0, 0},
        {0, cos(theta[0]), -sin(theta[0])},
        {0, sin(theta[0]), cos(theta[0])}
    };

    // Calculate rotation about y axis
    math::Matrix3 rotationY = {
        {cos(theta[1]), 0, sin(theta[1])},
        {0, 1, 0},
        {-sin(theta[1]), 0, cos(theta[1])}
    };

    // Calculate rotation about z axis
    const math::Matrix3 rotationZ = {
        {cos(theta[2]), -sin(theta[2]), 0},
        {sin(theta[2]), cos(theta[2]), 0},
        {0, 0, 1}
    };


    // Combined rotation matrix
    return rotationZ * rotationY * rotationX;
}
} //namespace poke::physics
