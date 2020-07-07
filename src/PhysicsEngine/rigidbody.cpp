#include <PhysicsEngine/rigidbody.h>

namespace poke {
namespace physics {

bool Rigidbody::operator==(const Rigidbody& other) const
{
    return linearDrag == other.linearDrag &&
           linearVelocity == other.linearVelocity &&
           angularVelocity == other.angularVelocity &&
           angularDrag == other.angularDrag &&
           type == other.type;
}

bool Rigidbody::operator!=(const Rigidbody& other) const
{
    return !(*this == other);
}

const json Rigidbody::ToJson() const
{
	json transformJson = json();
	transformJson["linearVelocity"] = linearVelocity.ToJson();
	transformJson["angularVelocity"] = angularVelocity.ToJson();
	transformJson["linearDrag"] = linearDrag;
	transformJson["angularDrag"] = angularDrag;
	transformJson["type"] = static_cast<int>(type);
	return transformJson;
}

void Rigidbody::SetFromJson(const json& transformJson)
{
	linearVelocity.SetFromJson(transformJson["linearVelocity"]);
	angularVelocity.SetFromJson(transformJson["angularVelocity"]);
	linearDrag = transformJson["linearDrag"];
	angularDrag = transformJson["angularDrag"];
	type = static_cast<RigidbodyType>(transformJson["type"]);
}
} //namespace physics
} //namespace poke