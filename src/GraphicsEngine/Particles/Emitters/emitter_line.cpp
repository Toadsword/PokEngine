#include <GraphicsEngine/Particles/Emitters/emitter_line.h>
#include <Math/math.h>

namespace poke {
namespace graphics {

bool EmitterLine::operator==(const EmitterLine& other) const
{
    return length == other.length
           && axis == other.axis;
}

bool EmitterLine::operator!=(const EmitterLine& other) const
{
    return !(*this == other);
}

EmitterLine::EmitterLine(const float length, const math::Vec3 axis)
    : length(length),
      axis(axis) {}

math::Vec3 EmitterLine::GeneratePosition() const
{
    return axis * length * math::Random::Get().RandomRange(-0.5f, 0.5f);
}

const math::Vec3& EmitterLine::GenerateDirection(math::Vec3 spawnPosition) const
{
	return math::Vec3::GetOrthogonal(axis);
}

void EmitterLine::SetFromJson(const json& emitterJson)
{
	length = emitterJson["length"];
	axis.SetFromJson(emitterJson["axis"]);
}

json EmitterLine::ToJson() const
{
	json emitterJson;
	emitterJson["length"] = length;
	emitterJson["axis"] = axis.ToJson();

	return emitterJson;
}
} //namespace graphics
} //namespace poke
