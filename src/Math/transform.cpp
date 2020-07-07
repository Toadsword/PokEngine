#include <Math/tranform.h>

namespace poke {
namespace math {
Transform::Transform(
    const Vec3 position,
    const Vec3 rotation,
    const Vec3 scale)
    : localPosition_(position),
      localRotation_(rotation),
      localScale_(scale) {}

bool Transform::operator==(const Transform& other) const
{
    return localPosition_ == other.localPosition_
           && localRotation_ == other.localRotation_
           && localScale_ == other.localScale_;
}

bool Transform::operator!=(const Transform& other) const
{
    return !(*this == other);
}

Vec3 Transform::GetLocalPosition() const { return localPosition_; }

void Transform::SetLocalPosition(const Vec3 value) { localPosition_ = value; }

Vec3 Transform::GetLocalRotation() const { return localRotation_; }

void Transform::SetLocalRotation(const Vec3 value) { localRotation_ = value; }

Vec3 Transform::GetLocalScale() const { return localScale_; }

void Transform::SetLocalScale(const Vec3 value) { localScale_ = value; }

const json Transform::ToJson() const
{
    json transformJson = json();
    transformJson["position"] = localPosition_.ToJson();
    transformJson["scale"] = localScale_.ToJson();
    transformJson["rotation"] = localRotation_.ToJson();
    return transformJson;
}

void Transform::SetFromJson(const json& transformJson)
{
    localPosition_.SetFromJson(transformJson["position"]);
    localRotation_.SetFromJson(transformJson["rotation"]);
    localScale_.SetFromJson(transformJson["scale"]);
}

Matrix4 Transform::GetWorldMatrix() const
{
    auto parentMatrix = Matrix4::Identity();
    parentMatrix = Matrix4::Translate(parentMatrix, localPosition_);

    parentMatrix = Matrix4::Rotate(
        parentMatrix,
        localRotation_.x,
        Vec3(1, 0, 0));
    parentMatrix = Matrix4::Rotate(
        parentMatrix,
        localRotation_.y,
        Vec3(0, 1, 0));
    parentMatrix = Matrix4::Rotate(
        parentMatrix,
        localRotation_.z,
        Vec3(0, 0, 1));

    parentMatrix = Matrix4::Scale(parentMatrix, localScale_);

    return parentMatrix;
}
} //namespace math
} //namespace poke
