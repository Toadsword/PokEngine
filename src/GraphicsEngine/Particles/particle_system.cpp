#include <GraphicsEngine/Particles/particle_system.h>

#include <GraphicsEngine/Particles/particle.h>
#include <math/tranform.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>

namespace poke {
namespace graphics {
ParticleSystem::ParticleSystem() : emitter() {}

bool ParticleSystem::operator==(const ParticleSystem& other) const
{
    return rateOverTime == other.rateOverTime
           && minSpeed == other.minSpeed
           && gravityModifier == other.gravityModifier
           && startColor == other.startColor
           && randomizeDirection == other.randomizeDirection
           && maxSpeed == other.maxSpeed
           && minLifetime == other.minLifetime
           && maxLifetime == other.maxLifetime
           && minSize == other.minSize
           && maxSize == other.maxSize
           && timeEmit == other.timeEmit
           && duration == other.duration
           && looping == other.looping
           && startDelay == other.startDelay
           && lifetime == other.lifetime
           && minSize == other.minSize
           && maxSize == other.maxSize
           && startRotation == other.startRotation
           && maxParticles == other.maxParticles
           && rateOverDistance == other.rateOverDistance
           && randomizePosition == other.randomizePosition
           && emitter == other.emitter
           && colorOverLifetime == other.colorOverLifetime
           && materialID == other.materialID;
}

bool ParticleSystem::operator!=(const ParticleSystem& other) const { return !(*this == other); }

math::Vec3 ParticleSystem::RandomUnitVectorWithinCone(
    const math::Vec3 coneDirection,
    const float angle) const
{
    const auto cosAngle = std::cos(angle);
    const auto theta = math::Random::Get().RandomRange(0.0f, 1.0f) * 2.0f * math::kPi;
    const auto z = (cosAngle + math::Random::Get().RandomRange(0.0f, 1.0f)) * (1.0f - cosAngle);
    const auto rootOneMinusZSquared = std::sqrt(1.0f - z * z);
    const auto x = rootOneMinusZSquared * std::cos(theta);
    const auto y = rootOneMinusZSquared * std::sin(theta);

    math::Vec4 direction(x, y, z, 1.0f);

    if (coneDirection.x != 0.0f || coneDirection.y != 0.0f || coneDirection.z != 1.0f &&
        coneDirection.z != -1.0f) {
        const auto rotateAxis = math::Vec3::Cross(coneDirection, {0, 0, 1}).
            Normalize();
        const auto rotateAngle = std::acos(coneDirection * math::Vec3(0, 0, 1));

        const math::Matrix4 rotationMatrix = math::Matrix4::Rotate(
            math::Matrix4::Identity(),
            -rotateAngle,
            rotateAxis);

        direction = math::Matrix4::Transform(rotationMatrix, direction);
    } else
        if (coneDirection.z == -1.0f) { direction.z *= -1.0f; }

    return direction.To3();
}

Particle ParticleSystem::EmitParticle(const math::Vec3 worldPos) const
{
    math::Vec3 spawnPos = emitter.GetSpawnPos();
    math::Vec3 velocity = emitter.GetDirection(spawnPos);

    spawnPos += worldPos + GenerateRandomUnitVector() * randomizePosition;

    velocity = velocity.Normalize();
    velocity *= math::Random::Get().RandomRange(minSpeed, maxSpeed);

    const auto scale = math::Random::Get().RandomRange(minSize, maxSize);
    const auto lifeLength = math::Random::Get().RandomRange(minLifetime, maxLifetime);

    return Particle{
        spawnPos,
        velocity,
        lifeLength,
        scale,
        gravityModifier,
        startColor.GetColorAt(math::Random::Get().RandomRange(0.0f, 1.0f)),
        colorOverLifetime
    };
}

bool ParticleSystem::UpdateLifetime(const float dt)
{
    lifetime += dt;
    if (!looping && duration + maxLifetime <= lifetime) {
        lifetime = 0;
        return false;
    }
    return true;
}

float ParticleSystem::GenerateValue(
    const float average,
    const float errorPercent)
{
    const auto error = math::Random::Get().RandomRange(-1.0f, 1.0f) * errorPercent;
    return average + (average * error);
}

math::Vec3 ParticleSystem::GenerateRandomUnitVector() const
{
    const auto theta = math::Random::Get().RandomRange(0.0f, 1.0f) * 2.0f * math::kPi;
    const auto z = math::Random::Get().RandomRange(0.0f, 1.0f) * 2.0f - 1.0f;
    const auto rootOneMinusZSquared = std::sqrt(1.0f - z * z);
    const auto x = rootOneMinusZSquared * std::cos(theta);
    const auto y = rootOneMinusZSquared * std::sin(theta);
    return {x, y, z};
}

void ParticleSystem::SetFromJson(const json& particleSystemJson)
{
    //Particle System
    duration = particleSystemJson["duration"];
    looping = particleSystemJson["looping"];
    startDelay = particleSystemJson["startDelay"];
    minLifetime = particleSystemJson["minLifetime"];
    maxLifetime = particleSystemJson["maxLifetime"];
    minSpeed = particleSystemJson["minSpeed"];
    maxSpeed = particleSystemJson["maxSpeed"];
    minSize = particleSystemJson["minSize"];
    maxSize = particleSystemJson["maxSize"];
    startRotation = particleSystemJson["startRotation"];
    startColor.SetFromJson(particleSystemJson["startColor"]);
    gravityModifier = particleSystemJson["gravityModifier"];
    maxParticles = particleSystemJson["maxParticles"];

    //Emission
    rateOverTime = particleSystemJson["rateOverTime"];
    rateOverDistance = particleSystemJson["rataeOverDistance"];

    //Shape
    emitter.SetFromJson(particleSystemJson["emitter"]);
    randomizeDirection = particleSystemJson["randomizeDirection"];
    randomizePosition = particleSystemJson["randomizePosition"];

    //Color over lifetime
    colorOverLifetime.SetFromJson(particleSystemJson["colorOverLifetime"]);

    //Material
    materialID = particleSystemJson["materialID"];
}

json ParticleSystem::ToJson() const
{
    json particleSystemJson;
    //Particle System
    particleSystemJson["duration"] = duration;
    particleSystemJson["looping"] = looping;
    particleSystemJson["startDelay"] = startDelay;
    particleSystemJson["minLifetime"] = minLifetime;
    particleSystemJson["maxLifetime"] = maxLifetime;
    particleSystemJson["minSpeed"] = minSpeed;
    particleSystemJson["maxSpeed"] = maxSpeed;
    particleSystemJson["minSize"] = minSize;
    particleSystemJson["maxSize"] = maxSize;
    particleSystemJson["startRotation"] = startRotation;
    particleSystemJson["startColor"] = startColor.ToJson();
    particleSystemJson["gravityModifier"] = gravityModifier;
    particleSystemJson["maxParticles"] = maxParticles;

    //Emission
    particleSystemJson["rateOverTime"] = rateOverTime;
    particleSystemJson["rataeOverDistance"] = rateOverDistance;

    //Shape
    particleSystemJson["emitter"] = emitter.ToJson();
    particleSystemJson["randomizeDirection"] = randomizeDirection;
    particleSystemJson["randomizePosition"] = randomizePosition;

    //Color over lifetime
    particleSystemJson["colorOverLifetime"] = colorOverLifetime.ToJson();

    //Material
    particleSystemJson["materialID"] = materialID;

    return particleSystemJson;
}
} //namespace graphics
} //namespace poke
