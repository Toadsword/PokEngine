#include <PhysicsEngine/collider.h>

namespace poke {
namespace physics {
bool Collider::operator==(const Collider& other) const
{
    if (shapeType != other.shapeType) { return false; }
    switch (shapeType) {
    case ShapeType::BOX:
        if (boxShape != other.boxShape) { return false; }
        break;
    case ShapeType::SPHERE:
        if (sphereShape != other.sphereShape) { return false; }
        break;
    case ShapeType::ELLIPSOID:
        if (ellipsoidShape != other.ellipsoidShape) { return false; }
        break;
    case ShapeType::MESH:
        if (meshShape != other.meshShape) { return false; }
        break;
    case ShapeType::RECT:
        return false;
        break;
    default: ;
    }

    return isTrigger == other.isTrigger;
}

bool Collider::operator!=(const Collider& other) const
{
    return !(*this == other);
}

Collider::Collider(const Collider& other)
    : shapeType(other.shapeType),
      isTrigger(other.isTrigger)
{
    switch (shapeType) {
    case ShapeType::BOX:
        boxShape = other.boxShape;
        break;
    case ShapeType::SPHERE:
        ellipsoidShape = other.ellipsoidShape;
        break;
    case ShapeType::ELLIPSOID:
        meshShape = other.meshShape;
        break;
    case ShapeType::MESH:
        sphereShape = other.sphereShape;
        break;
    case ShapeType::RECT:
        break;
    default: ;
    }
}

Collider::Collider(Collider&& other) noexcept
    : shapeType(other.shapeType),
      isTrigger(other.isTrigger)
{
    switch (shapeType) {
    case ShapeType::BOX:
        boxShape = std::move(other.boxShape);
        break;
    case ShapeType::SPHERE:
        ellipsoidShape = std::move(other.ellipsoidShape);
        break;
    case ShapeType::ELLIPSOID:
        meshShape = std::move(other.meshShape);
        break;
    case ShapeType::MESH:
        sphereShape = std::move(other.sphereShape);
        break;
    case ShapeType::RECT:
        break;
    default: ;
    }
}

Collider& Collider::operator=(const Collider& other)
{
    if (this == &other)
        return *this;
    shapeType = other.shapeType;
    isTrigger = other.isTrigger;

    switch (shapeType) {
    case ShapeType::BOX:
        boxShape = other.boxShape;
        break;
    case ShapeType::SPHERE:
        ellipsoidShape = other.ellipsoidShape;
        break;
    case ShapeType::ELLIPSOID:
        meshShape = other.meshShape;
        break;
    case ShapeType::MESH:
        sphereShape = other.sphereShape;
        break;
    case ShapeType::RECT:
        break;
    default: ;
    }
    return *this;
}

Collider& Collider::operator=(Collider&& other) noexcept
{
    if (this == &other)
        return *this;
    shapeType = other.shapeType;
    isTrigger = other.isTrigger;

    switch (shapeType) {
    case ShapeType::BOX:
        boxShape = other.boxShape;
        break;
    case ShapeType::SPHERE:
        ellipsoidShape = other.ellipsoidShape;
        break;
    case ShapeType::ELLIPSOID:
        meshShape = other.meshShape;
        break;
    case ShapeType::MESH:
        sphereShape = other.sphereShape;
        break;
    case ShapeType::RECT:
        break;
    default: ;
    }
    return *this;
}

Collider::Collider()
    : boxShape(BoxShape()),
      shapeType(ShapeType::BOX),
      isTrigger(false) { }

Collider::~Collider() {}

void Collider::SetShape(const BoxShape boxShape)
{
    this->boxShape = boxShape;
    shapeType = ShapeType::BOX;
}

void Collider::SetShape(const SphereShape sphereShape)
{
    this->sphereShape = sphereShape;
    shapeType = ShapeType::SPHERE;
}

void Collider::SetShape(const MeshShape meshShape)
{
    this->meshShape = meshShape;
    shapeType = ShapeType::MESH;
}

void Collider::SetShape(const EllipsoidShape ellipsoidShape)
{
    this->ellipsoidShape = ellipsoidShape;
    shapeType = ShapeType::ELLIPSOID;
}

AABB Collider::ComputeAABB(
    const math::Vec3 center,
    const math::Vec3 extent,
    const math::Vec3 rotation) const
{
    switch (shapeType) {
    case ShapeType::BOX:
        return boxShape.ComputeAABB(center, extent, rotation);
    case ShapeType::SPHERE:
        return sphereShape.ComputeAABB(center, extent, rotation);
    case ShapeType::ELLIPSOID:
        return ellipsoidShape.ComputeAABB(center, extent, rotation);
    case ShapeType::MESH:
        return meshShape.ComputeAABB(center, extent, rotation);
    default: ;
    }
    return boxShape.ComputeAABB(center, extent, rotation);
}

AABB Collider::ComputeAABB(const math::Transform& transform) const
{
	switch (shapeType) {
	case ShapeType::BOX:
		return boxShape.ComputeAABB(transform);
	case ShapeType::SPHERE:
		return sphereShape.ComputeAABB(transform);
	case ShapeType::ELLIPSOID:
		return ellipsoidShape.ComputeAABB(transform);
	case ShapeType::MESH:
		return meshShape.ComputeAABB(transform);
	default:;
	}
	return boxShape.ComputeAABB(transform);
}

json Collider::ToJson() const
{
    json newJson;

	newJson["isTrigger"] = isTrigger;
	newJson["shapeType"] = static_cast<int>(shapeType);
    switch (shapeType) {
        case ShapeType::BOX:
			newJson["shape"] = boxShape.ToJson();
        break;
        case ShapeType::SPHERE:
			newJson["shape"] = sphereShape.ToJson();
        break;
        case ShapeType::ELLIPSOID:
			newJson["shape"] = ellipsoidShape.ToJson();
        break;
        case ShapeType::MESH:
			newJson["shape"] = meshShape.ToJson();
        break;
    default: ; }
    return newJson;
}

void Collider::SetFromJson(const json& json)
{
	isTrigger = json["isTrigger"];
	shapeType = static_cast<ShapeType>(json["shapeType"]);
	switch (shapeType) {
	case ShapeType::BOX:
		boxShape.SetFromJson(json["shape"]);
		break;
	case ShapeType::SPHERE:
		sphereShape.SetFromJson(json["shape"]);
		break;
	case ShapeType::ELLIPSOID:
		ellipsoidShape.SetFromJson(json["shape"]);
		break;
	case ShapeType::MESH:
		meshShape.SetFromJson(json["shape"]);
		break;
	default:;
	}
}
} //namespace physics
} //namespace poke
