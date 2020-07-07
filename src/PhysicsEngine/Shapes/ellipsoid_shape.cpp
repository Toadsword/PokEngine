#include <PhysicsEngine/Shapes/ellipsoid_shape.h>

#include <Math/tranform.h>

namespace poke::physics {

EllipsoidShape::EllipsoidShape(
    const math::Vec3 positionOffset,
    const float radius)
    : positionOffset_(positionOffset),
      radius_(radius) { }

bool EllipsoidShape::operator==(const EllipsoidShape& other) const
{
    return positionOffset_ == other.positionOffset_
           && radius_ == other.radius_;
}

bool EllipsoidShape::operator!=(const EllipsoidShape& other) const
{
    return !(*this == other);
}

AABB EllipsoidShape::ComputeAABB(
    const math::Vec3 position,
    const math::Vec3 scale,
    const math::Vec3 rotation) const
{
    const math::Matrix4 modelMatrix = math::Matrix4::GetWorldMatrix(
        position,
        rotation,
        scale);
    const math::Matrix4 modelMatrixTranspose = math::Matrix4::
        GetTranspose(modelMatrix);

    const math::Matrix4 sphereMatrix{
        {radius_, 0.0f, 0.0f, 0.0f},
        {0.0f, radius_, 0.0f, 0.0f},
        {0.0f, 0.0f, radius_, 0.0f},
        {0.0f, 0.0f, 0.0f, -1.0f}
    };

    const math::Matrix4 sphereMatrixInverse = math::Matrix4::
        GetInverse(sphereMatrix);

    const math::Matrix4 rotatedMatrix =
        modelMatrix * sphereMatrixInverse * modelMatrixTranspose;

    const float minX = (rotatedMatrix[0][3] - std::sqrt(
                            pow(rotatedMatrix[0][3], 2) - rotatedMatrix[3][3] *
                            rotatedMatrix[0][0])
                       ) / rotatedMatrix[3][3];

    const float maxX = (rotatedMatrix[0][3] + std::sqrt(
                            pow(rotatedMatrix[0][3], 2) - rotatedMatrix[3][3] *
                            rotatedMatrix[0][0])
		                ) / rotatedMatrix[3][3];

    const float minY = (rotatedMatrix[1][3] - std::sqrt(
                            pow(rotatedMatrix[1][3], 2) - rotatedMatrix[3][3] *
                            rotatedMatrix[1][1])
                       ) / rotatedMatrix[3][3];

    const float maxY = (rotatedMatrix[1][3] + std::sqrt(
                            pow(rotatedMatrix[1][3], 2) - rotatedMatrix[3][3] *
                            rotatedMatrix[1][1])
                       ) / rotatedMatrix[3][3];

    const float minZ = (rotatedMatrix[2][3] - std::sqrt(
                            pow(rotatedMatrix[2][3], 2) - rotatedMatrix[3][3] *
                            rotatedMatrix[2][2])) /
                       rotatedMatrix[3][3];

    const float maxZ = (rotatedMatrix[2][3] + std::sqrt(
                            pow(rotatedMatrix[2][3], 2) - rotatedMatrix[3][3] *
                            rotatedMatrix[2][2])) /
                       rotatedMatrix[3][3];

    return {
        position,
        {
            abs(minX - maxX) * 0.5f,
            abs(minY - maxY) * 0.5f,
            abs(minZ - maxZ) * 0.5f
        }
    };
}

AABB EllipsoidShape::ComputeAABB(const math::Transform& transform) const
{
	return ComputeAABB(transform.GetLocalPosition(), transform.GetLocalScale(), transform.GetLocalRotation());
}

ShapeType EllipsoidShape::GetType() const { return ShapeType::ELLIPSOID; }

std::unique_ptr<IShape> EllipsoidShape::Clone()
{
    return std::make_unique<EllipsoidShape>(*this);
}

math::Vec3 EllipsoidShape::GetCenter() const { return positionOffset_; }

void EllipsoidShape::SetCenter(const math::Vec3 center)
{
    positionOffset_ = center;
}

float EllipsoidShape::GetRadius() const { return radius_; }

void EllipsoidShape::SetRadius(const float radius) { radius_ = radius; }

void EllipsoidShape::SetFromJson(json shapeJson)
{
	positionOffset_.SetFromJson(shapeJson["positionOffset"]);
	radius_ = shapeJson["radius"];
}

json EllipsoidShape::ToJson() const
{
	json shapeJson;
	shapeJson["positionOffset"] = positionOffset_.ToJson();
	shapeJson["radius"] = radius_;
	return shapeJson;
}
} //namespace poke::physics
