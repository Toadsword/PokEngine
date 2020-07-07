#include <GraphicsEngine/Particles/Emitters/emitter_point.h>

namespace poke {
namespace graphics {
EmitterPoint::EmitterPoint(const math::Vec3 pos) { point = pos; }

bool EmitterPoint::operator==(const EmitterPoint& other) const
{
    return point == other.point;
}

bool EmitterPoint::operator!=(const EmitterPoint& other) const
{
    return !(*this == other);
}

math::Vec3 EmitterPoint::GeneratePosition() const
{
	return point;
}

math::Vec3 EmitterPoint::GenerateDirection(const math::Vec3& spawnPos) const
{
	return RandomUnitVector();
}

void EmitterPoint::SetFromJson(const json& emitterJson)
{
	point.SetFromJson(emitterJson["point"]);
}

json EmitterPoint::ToJson() const
{
	json emitterJson;
	emitterJson["point"] = point.ToJson();
	return emitterJson;
}
} //namespace graphics
} // namespace poke
