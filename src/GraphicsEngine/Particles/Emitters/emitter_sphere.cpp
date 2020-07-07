#include <GraphicsEngine/Particles/Emitters/emitter_sphere.h>
#include <Math/math.h>

namespace poke {
namespace graphics {

bool EmitterSphere::operator==(const EmitterSphere& other) const
{
    return radius == other.radius;
}

bool EmitterSphere::operator!=(const EmitterSphere& other) const
{
    return !(*this == other);
}

EmitterSphere::EmitterSphere(const float radius) : radius(radius) {}

math::Vec3 EmitterSphere::GeneratePosition() const
{
	auto a = math::Random::Get().RandomRange(0.0f, 1.0f);
	auto b = math::Random::Get().RandomRange(0.0f, 1.0f);

    if(a > b) {
		std::swap(a, b);
    }

	const auto randX = b * std::cos(2.0f * math::kPi * (a / b));
	const auto randY = b * std::sin(2.0f * math::kPi * (a / b));
	const auto distance = math::Vec2(randX, randY).GetMagnitude();
	return RandomUnitVector() * distance * radius;
}

math::Vec3 EmitterSphere::GenerateDirection(const math::Vec3& spawnPos) const
{
    if(spawnPos != math::Vec3(0,0,0)) {
		return spawnPos.Normalize();
    }else {
		return RandomUnitVector();
    }
}

void EmitterSphere::SetFromJson(const json& emitterJson)
{
	radius = emitterJson["radius"];
}

json EmitterSphere::ToJson() const
{
	json emitterJson;

	emitterJson["radius"] = radius;

	return emitterJson;
}
} //namespace graphics
} //namespace poke
