#include <GraphicsEngine/Particles/Emitters/emitter_circle.h>

#include <Math/math.h>

namespace poke {
namespace graphics {

bool EmitterCircle::operator==(const EmitterCircle& other) const
{
    return radius == other.radius
           && direction == other.direction;
}

bool EmitterCircle::operator!=(const EmitterCircle& other) const
{
    return !(*this == other);
}

EmitterCircle::EmitterCircle(const float radius, const math::Vec3 direction)
    : radius(radius),
      direction(direction) {}

math::Vec3 EmitterCircle::GeneratePosition() const
{
	math::Vec3 dir;
    do {
		const auto randomVector = RandomUnitVector();
		dir = math::Vec3::Cross(randomVector, direction);
	} while (dir.GetMagnitude() == 0.0f);

	dir = dir.Normalize() * radius;

	auto a = math::Random::Get().RandomRange(0.0f, 1.0f);
	auto b = math::Random::Get().RandomRange(0.0f, 1.0f);

    if(a > b) {
		std::swap(a, b);
    }

	const auto randX = b * std::cos(2.0f * math::kPi * (a / b));
	const auto randY = b * std::sin(2.0f * math::kPi * (a / b));
	const auto distance = math::Vec3(randX, randY, 0.0f).GetMagnitude();
	return dir * distance;
}

math::Vec3 EmitterCircle::GenerateDirection(const math::Vec3& spawnPos) const
{
    if(spawnPos != math::Vec3(0,0,0)) {
        return spawnPos;
    }

	return math::Vec3::Multiply(RandomUnitVector(), direction).Normalize();
}

void EmitterCircle::SetFromJson(const json& emitterJson)
{
	radius = emitterJson["radius"];
	direction.SetFromJson(emitterJson["direction"]);
}

json EmitterCircle::ToJson() const
{
	json emitterJson;

	emitterJson["radius"] = radius;
	emitterJson["direction"] = direction.ToJson();

	return emitterJson;
}

} //namespace graphics
} //namespace poke
