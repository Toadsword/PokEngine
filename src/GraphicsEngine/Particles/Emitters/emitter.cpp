#include <GraphicsEngine/Particles/Emitters/emitter.h>

#include <Math/math.h>

namespace poke {
namespace graphics {

math::Vec3 Emitter::RandomUnitVector()
{
    const auto theta = math::Random::Get().RandomRange(0.0f, 1.0f) * 2.0f * math
                       ::kPi;
    const auto z = math::Random::Get().RandomRange(0.0f, 1.0f) * 2.0f - 1.0f;
    const auto rootOneMinusZSquared = std::sqrt(1.0f - z * z);
    const auto x = rootOneMinusZSquared * std::cos(theta);
    const auto y = rootOneMinusZSquared * std::sin(theta);
    return {x, y, z};
}
} //namespace graphics
} //namespace poke