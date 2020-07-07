#include <GraphicsEngine/Particles/particle.h>

#include <CoreEngine/ServiceLocator/service_locator_definition.h>

namespace poke {
namespace graphics {

bool ParticleDrawInfo::operator==(const ParticleDrawInfo& other) const
{
    return position == other.position
           && scale == other.scale
           && colorOffset == other.colorOffset
           && imageOffset1 == other.imageOffset1
           && imageBlendFactor == other.imageBlendFactor
           && imageOffset2 == other.imageOffset2
           && transparency == other.transparency;
}

bool ParticleDrawInfo::operator!=(const ParticleDrawInfo& other) const { return !(*this == other); }

Particle::Particle(
    const math::Vec3 position,
    const math::Vec3 velocity,
    const float lifeLength,
    const float scale,
    const float gravityEffect,
	const Color colorOffset,
	const ColorGradient colorOverLifetime)
    : originalColor(colorOffset),
      colorOverLifetime(colorOverLifetime),
      position(position),
      velocity(velocity),
      lifetime(lifeLength),
      scale(scale),
      gravityEffect(gravityEffect) {}
} //namespace graphics
} //namespace poke
