#include <GraphicsEngine/Particles/Emitters/emitter_cone.h>

#include <Math/math.h>

namespace poke {
namespace graphics {

EmitterCone::EmitterCone(const float angle, const math::Vec3 direction)
    : angle(angle),
      direction(direction) { }

bool EmitterCone::operator==(const EmitterCone& other) const
{
    return angle == other.angle &&
           direction == other.direction;
}

bool EmitterCone::operator!=(const EmitterCone& other) const { return !(*this == other); }

math::Vec3 EmitterCone::GeneratePosition() const
{
    const math::Vec3 pos = direction * math::Random::Get().RandomRange(0.0001f, 1.0f);

    const math::Vec3 dir = math::Vec3::Cross(RandomUnitVector(), direction) * math::Random::Get().
                           RandomRange(
                               0.0f,
                               std::tan(angle * 0.5f)) * pos.GetMagnitude();

    return pos + dir;
}

math::Vec3 EmitterCone::GenerateDirection(const math::Vec3& spawnPos) const { return spawnPos; }

void EmitterCone::SetFromJson(const json& emitterJson)
{
    angle = emitterJson["angle"];
    direction.SetFromJson(emitterJson["direction"]);
}

json EmitterCone::ToJson() const
{
    json emitterJson;

    emitterJson["angle"] = angle;
    emitterJson["direction"] = direction.ToJson();

    return emitterJson;
}
} //namespace graphics
} //namespace poke
