#include <CoreEngine/CoreSystems/particles_system.h>

#include <CoreEngine/engine.h>
#include <Utility/time_custom.h>
#include <GraphicsEngine/Particles/particle.h>
#include <Utility/profiler.h>

namespace poke {
ParticlesSystem::ParticlesSystem(Engine& engine)
    : System(engine),
      newEntities_(500),
      destroyedEntities_(500),
      particleSystems_(10000),
      particleSystemsManager_(ecsManager_.GetComponentsManager<ecs::ParticleSystemsManager>()),
      transformsManager_(ecsManager_.GetComponentsManager<ecs::TransformsManager>()),
      materialManager_(MaterialsManagerLocator::Get()),
      camera_(CameraLocator::Get()),
      particleCommandBuffer_(GraphicsEngineLocator::Get().GetParticleCommandBuffer())
{
    engine_.AddObserver(
        observer::MainLoopSubject::UPDATE,
        [this]() { OnUpdate(); });
    engine_.AddObserver(
        observer::MainLoopSubject::DRAW,
        [this]() { OnDraw(); });
    engine_.AddObserver(
        observer::MainLoopSubject::CULLING,
        [this]() { OnCulling(); });
    engine_.AddObserver(
        observer::MainLoopSubject::END_FRAME,
        [this]() { OnEndOfFrame(); });
    SceneManagerLocator::Get().AddOnUnloadObserver(
        [this]() { OnUnloadScene(); });

    ecsManager_.RegisterObserverAddComponent(
        [this](
        const ecs::EntityIndex entityIndex,
        const ecs::ComponentMask component) {
            OnEntityAddComponent(entityIndex, component);
        });

    ecsManager_.RegisterObserverRemoveComponent(
        [this](
        const ecs::EntityIndex entityIndex,
        const ecs::ComponentMask component) {
            OnEntityRemoveComponent(entityIndex, component);
        });

    ecsManager_.RegisterObserverUpdateComponent(
        [this](
        const ecs::EntityIndex entityIndex,
        const ecs::ComponentMask component) {
            OnEntityUpdateComponent(entityIndex, component);
        });

    ObserveEntitySetActive();
    ObserveEntitySetInactive();
}

void ParticlesSystem::OnEntitySetActive(const ecs::EntityIndex entityIndex)
{
    if (ecsManager_.HasComponent(entityIndex, ecs::ComponentType::PARTICLE_SYSTEM)) {
        newEntities_.insert(entityIndex);
    }
}

void ParticlesSystem::OnEntitySetInactive(const ecs::EntityIndex entityIndex)
{
    if (ecsManager_.HasComponent(entityIndex, ecs::ComponentType::PARTICLE_SYSTEM)) {
        destroyedEntities_.insert(entityIndex);
    }
}

void ParticlesSystem::OnUpdate() {}

inline math::Vec2 CalculateImageOffset(const int32_t index)
{
    const auto column = index % 1;
    const auto row = index / 1;
    return math::Vec2(static_cast<float>(column), static_cast<float>(row)) / 1.0f;
}

void ParticlesSystem::OnDraw()
{
    pok_BeginProfiling(Particle_System, 0);

    const float dt = Time::Get().deltaTime.count() / 1000.0f;

    const auto camPos = camera_.GetPosition();

    //Check life time and emit time
    for (size_t i = 0; i < particleSystems_.size(); i++) {
        const auto entityIndex = particleSystems_[i];
#pragma region EMIT
        pok_BeginProfiling(Emit, 0);
        auto particleSystem = particleSystemsManager_.GetComponent(entityIndex);

        //Check lifetime
        pok_BeginProfiling(Check_lifetime, 0);
        if (!particleSystem.UpdateLifetime(dt)) {
			auto& fillingVector = particlesDrawing_[i];
			fillingVector.clear();
            pok_EndProfiling(Check_lifetime);
            pok_EndProfiling(Emit);
            continue;
        }
        pok_EndProfiling(Check_lifetime);
        auto& numberOfRows = particles_[i].numberOfRows;
        auto& originalColor = particles_[i].originalColor;
        auto& colorOffset = particles_[i].colorOffset;
        auto& colorOverLifetime = particles_[i].colorOverLifetime;
        auto& position = particles_[i].position;
        auto& velocity = particles_[i].velocity;
        auto& imageOffset1 = particles_[i].imageOffset1;
        auto& imageOffset2 = particles_[i].imageOffset2;
        auto& lifetime = particles_[i].lifetime;
        auto& scale = particles_[i].scale;
        auto& elapsedTime = particles_[i].elapsedTime;
        auto& transparency = particles_[i].transparency;
        auto& imageBlendFactor = particles_[i].imageBlendFactor;

        //Check Rate over Time Emit
        pok_BeginProfiling(Check_rate_over_time, 0);
        if (particleSystem.lifetime >= particleSystem.startDelay && (
                particleSystem.lifetime <= particleSystem.duration || particleSystem.looping)) {
            particleSystem.timeEmit += dt;

            const float rate = 1.0f / particleSystem.rateOverTime;

            if (particleSystem.timeEmit > rate) {
                const auto worldPos = transformsManager_.GetWorldPosition(entityIndex);
                while (particleSystem.timeEmit > rate) {
                    auto particle = particleSystem.EmitParticle(worldPos);
                    particleSystem.timeEmit -= rate;

                    if (particles_[i].nbParticles >= particleSystem.maxParticles)
                        continue;
                    const size_t index = particles_[i].nbParticles++;
                    /*if (particles_[particleSystemIndex].nbParticles % 500 == 0) {
                        LogDebug(std::to_string(particles_[particleSystemIndex].nbParticles) + " -> dt = " + std::to_string(dt));
                    }*/

                    if (numberOfRows.size() <= index) {
                        const size_t newSize = std::min(
                            index * 2 + 1,
                            static_cast<size_t>(particleSystem.maxParticles));

                        numberOfRows.resize(newSize);
                        originalColor.resize(newSize);
                        colorOffset.resize(newSize);
                        colorOverLifetime.resize(newSize);
                        position.resize(newSize);
                        velocity.resize(newSize);
                        imageOffset1.resize(newSize);
                        imageOffset2.resize(newSize);
                        lifetime.resize(newSize);
                        scale.resize(newSize);
                        elapsedTime.resize(newSize);
                        transparency.resize(newSize);
                        imageBlendFactor.resize(newSize);
                    }

                    numberOfRows[index] = particle.numberOfRows;
                    originalColor[index] = particle.originalColor;
                    colorOffset[index] = particle.colorOffset;
                    colorOverLifetime[index] = particle.colorOverLifetime;
                    position[index] = particle.position;
                    velocity[index] = particle.velocity;
                    imageOffset1[index] = particle.imageOffset1;
                    imageOffset2[index] = particle.imageOffset2;
                    lifetime[index] = particle.lifetime;
                    scale[index] = particle.scale;
                    elapsedTime[index] = 0;
                    transparency[index] = particle.transparency;
                    imageBlendFactor[index] = particle.imageBlendFactor;
                }
            }
        }
        pok_EndProfiling(Check_rate_over_time);

        //Check Rate Over Distance Emit
        pok_BeginProfiling(Check_rate_over_distance, 0);
        if (particleSystem.rateOverDistance > 0.0f) {
            const auto worldPos = transformsManager_.GetWorldPosition(entityIndex);
            particleSystem.distanceEmit += math::Vec3::GetDistance(
                worldPos,
                particleSystem.previousPos);
            particleSystem.previousPos = transformsManager_
                                         .GetComponent(entityIndex).GetLocalPosition();

            if (particleSystem.distanceEmit > particleSystem.rateOverDistance) {
                while (particleSystem.distanceEmit > particleSystem.rateOverDistance) {
                    auto particle = particleSystem.EmitParticle(worldPos);
                    particleSystem.distanceEmit -= 1.0f / particleSystem.rateOverDistance;

                    if (particles_[i].nbParticles >= particleSystem.maxParticles)
                        continue;
                    const size_t index = particles_[i].nbParticles++;

                    if (numberOfRows.size() <= index) {
                        const size_t newSize = std::min(
                            index * 2 + 1,
                            static_cast<size_t>(particleSystem.maxParticles));

                        numberOfRows.resize(newSize);
                        originalColor.resize(newSize);
                        colorOffset.resize(newSize);
                        colorOverLifetime.resize(newSize);
                        position.resize(newSize);
                        velocity.resize(newSize);
                        imageOffset1.resize(newSize);
                        imageOffset2.resize(newSize);
                        lifetime.resize(newSize);
                        scale.resize(newSize);
                        elapsedTime.resize(newSize);
                        transparency.resize(newSize);
                        imageBlendFactor.resize(newSize);
                    }

                    numberOfRows[index] = particle.numberOfRows;
                    originalColor[index] = particle.originalColor;
                    colorOffset[index] = particle.colorOffset;
                    colorOverLifetime[index] = particle.colorOverLifetime;
                    position[index] = particle.position;
                    velocity[index] = particle.velocity;
                    imageOffset1[index] = particle.imageOffset1;
                    imageOffset2[index] = particle.imageOffset2;
                    lifetime[index] = particle.lifetime;
                    scale[index] = particle.scale;
                    elapsedTime[index] = 0;
                    transparency[index] = particle.transparency;
                    imageBlendFactor[index] = particle.imageBlendFactor;
                }
            }
        }
        particleSystemsManager_.SetComponent(entityIndex, particleSystem);
        pok_EndProfiling(Check_rate_over_distance);
        pok_EndProfiling(Emit);
#pragma endregion
        if (particles_[i].nbParticles <= 0)
            continue;
        //Update particle system
        pok_BeginProfiling(Update_particle_system, 0);
        //Check alive
        pok_BeginProfiling(Age, 0);
        const int size = particles_[i].nbParticles;
        for (size_t i = 0; i < size; ++i) { elapsedTime[i] += dt; }
        pok_EndProfiling(Age);
        pok_BeginProfiling(Check_alive, 0);

        auto upperBound = particles_[i].nbParticles;
        for (auto j = size - 1; j >= 0; --j) {
            if (elapsedTime[j] < lifetime[j]) {
                upperBound = j + 1;
                break;
            }
        }

        for (auto i = 0; i < upperBound; i++) {
            if (elapsedTime[i] >= lifetime[i]) {
                //Swap dead and living particles
                const auto index = upperBound - 1;
                std::swap(numberOfRows[i], numberOfRows[index]);
                std::swap(originalColor[i], originalColor[index]);
                std::swap(colorOffset[i], colorOffset[index]);
                std::swap(colorOverLifetime[i], colorOverLifetime[index]);
                std::swap(position[i], position[index]);
                std::swap(velocity[i], velocity[index]);
                std::swap(imageOffset1[i], imageOffset1[index]);
                std::swap(imageOffset2[i], imageOffset2[index]);
                std::swap(lifetime[i], lifetime[index]);
                std::swap(scale[i], scale[index]);
                std::swap(elapsedTime[i], elapsedTime[index]);
                std::swap(transparency[i], transparency[index]);
                std::swap(imageBlendFactor[i], imageBlendFactor[index]);

                for (auto j = upperBound - 1; j >= 0; --j) {
                    if (elapsedTime[j] < lifetime[j]) {
                        upperBound = j + 1;
                        break;
                    }
                }
            }
        }
        particles_[i].nbParticles = upperBound;
        pok_EndProfiling(Check_alive);
        if (upperBound <= 0) {
            pok_BeginProfiling(Update_particle_system, 0);
            continue;
        }
        //Update velocity
        pok_BeginProfiling(Update_velocity, 0);
        const auto gravityFactor = -10.0f * particleSystem.gravityModifier * dt;

        for (int j = 0; j < upperBound; j++) { velocity[j].y += gravityFactor; }
        pok_EndProfiling(Update_velocity);
        //Update position
        pok_BeginProfiling(Update_position, 0);
        for (int j = 0; j < upperBound; j++) { position[j] += velocity[j] * dt; }
        pok_EndProfiling(Update_position);
        //Update
        pok_BeginProfiling(Update, 0);
        for (int j = 0; j < upperBound; j++) {
            //Color
            const auto lifeFactor = elapsedTime[j] / lifetime[j];
            colorOffset[j] = originalColor[j] * particleSystem.colorOverLifetime.GetColorAt(
                                 lifeFactor);

            transparency[j] = colorOffset[j].a;

            const auto stageCount = static_cast<int32_t>(pow(1, 2));
            const auto atlasProgression = 0 * lifeFactor * stageCount;
            //TODO(@Nico) replace 0 with stage cycles
            const auto index1 = static_cast<int32_t>(std::floor(atlasProgression));
            const auto index2 = index1 < stageCount - 1 ? index1 + 1 : index1;

            imageBlendFactor[j] = std::fmod(atlasProgression, 1.0f);
            imageOffset1[j] = CalculateImageOffset(index1);
            imageOffset2[j] = CalculateImageOffset(index2);
        }
        pok_EndProfiling(Update);
        pok_BeginProfiling(Compute_distance, 0);
        //Sort particles
        std::vector<std::pair<float, size_t>> sortedIndex(upperBound);
        for (int j = 0; j < upperBound; j++) {
            const auto distance = math::Vec3::GetDistanceManhattan(camPos, position[j]);
            sortedIndex[j] = std::pair<float, size_t>(distance, j);
        }
        pok_EndProfiling(Compute_distance);
        pok_BeginProfiling(Sort_particles, 0);
        std::sort(
            sortedIndex.begin(),
            sortedIndex.end(),
            [](const std::pair<float, size_t>& d1, const std::pair<float, size_t>& d2) {
                return d1 < d2;
            });
        pok_EndProfiling(Sort_particles);
        //Prepare vector for receiving drawing data
        pok_BeginProfiling(Draw_particles, 0);
        auto& fillingVector = particlesDrawing_[i];
        fillingVector.clear();
        fillingVector.resize(upperBound);

        //Create drawing info
        for (size_t particleIndex = 0; particleIndex < upperBound; particleIndex++) {
            auto index = sortedIndex[particleIndex].second;
            fillingVector[upperBound - index - 1] =
                graphics::ParticleDrawInfo{
                    position[index],
                    colorOffset[index],
                    imageOffset1[index],
                    imageBlendFactor[index],
                    imageOffset2[index],
                    transparency[index],
                    scale[index]
                };
        }
        pok_EndProfiling(Draw_particles);
        pok_EndProfiling(Update_particle_system);
    }

    pok_EndProfiling(Particle_System);
}

void ParticlesSystem::OnCulling()
{
    pok_BeginProfiling(Particle_System, 0);

    for (size_t i = 0; i < particlesRendering_.size(); i++) {
        particleCommandBuffer_.DrawParticles(
            particleInstanceIndexRendering_[i],
            particlesRendering_[i]);
    }
    pok_EndProfiling(Particle_System);
}

void ParticlesSystem::OnEndOfFrame()
{
	pok_BeginProfiling(Particle_System, 0);
    for (auto newEntity : newEntities_) {
        if (particleSystems_.exist(newEntity))
            continue;
        const auto it = particleSystems_.insert(newEntity);
        const auto index = std::distance(particleSystems_.begin(), it);

        auto particleSystems = particleSystemsManager_.GetComponent(newEntity);
        particleSystems.previousPos = transformsManager_.GetWorldPosition(newEntity);
        particleSystemsManager_.SetComponent(newEntity, particleSystems);

        particlesDrawing_.insert(
            particlesDrawing_.begin() + index,
            {graphics::ParticleDrawInfo()});
        particles_.insert(particles_.begin() + index, {Particle()});

        auto& mat = materialManager_.GetMaterial(particleSystems.materialID);
        particleInstanceIndexDrawing_.insert(
            particleInstanceIndexDrawing_.begin() + index,
            particleCommandBuffer_.AddParticleInstance(mat));
    }
    newEntities_.clear();

    for (auto destroyedEntity : destroyedEntities_) {
        if (!particleSystems_.exist(destroyedEntity))
            continue;
        auto particleSystem = particleSystemsManager_.GetComponent(destroyedEntity);
        particleSystem.lifetime = 0;
        particleSystemsManager_.SetComponent(destroyedEntity, particleSystem);

        const auto it = particleSystems_.find(destroyedEntity);
        const auto index = std::distance(particleSystems_.begin(), it);

        particleSystems_.erase(it);
        particleInstanceIndexDrawing_.erase(particleInstanceIndexDrawing_.begin() + index);
        particles_.erase(particles_.begin() + index);
        particlesDrawing_.erase(particlesDrawing_.begin() + index);
    }
    destroyedEntities_.clear();

    particleInstanceIndexRendering_ = particleInstanceIndexDrawing_;

    particlesRendering_ = particlesDrawing_;

	pok_EndProfiling(Particle_System);
}

void ParticlesSystem::OnEntityAddComponent(
    const ecs::EntityIndex entityIndex,
    const ecs::ComponentMask component)
{
    if ((component & ecs::ComponentType::PARTICLE_SYSTEM) == ecs::ComponentType::PARTICLE_SYSTEM &&
        ecsManager_.IsEntityActive(entityIndex)) { newEntities_.insert(entityIndex); }
}

void ParticlesSystem::OnEntityRemoveComponent(
    const ecs::EntityIndex entityIndex,
    const ecs::ComponentMask component)
{
    if ((component & ecs::ComponentType::PARTICLE_SYSTEM) == ecs::ComponentType::PARTICLE_SYSTEM &&
        ecsManager_.IsEntityActive(entityIndex)) { destroyedEntities_.insert(entityIndex); }
}

void ParticlesSystem::OnEntityUpdateComponent(
    const ecs::EntityIndex entityIndex,
    const ecs::ComponentMask component)
{
    if ((component & ecs::ComponentType::PARTICLE_SYSTEM) == ecs::ComponentType::PARTICLE_SYSTEM) {
        const auto particleSystems = particleSystemsManager_.GetComponent(entityIndex);
        const auto& mat = materialManager_.GetMaterial(particleSystems.materialID);

        const auto particleIndex = std::distance(
            particleSystems_.begin(),
            std::find(particleSystems_.begin(), particleSystems_.end(), entityIndex));

        particleInstanceIndexDrawing_[particleIndex] = particleCommandBuffer_.AddParticleInstance(
            mat);
    }
}

void ParticlesSystem::OnUnloadScene()
{
    particleSystems_.clear();

    particleInstanceIndexDrawing_.clear();
    particleInstanceIndexRendering_.clear();

    particlesRendering_.clear();
    particlesDrawing_.clear();
    particles_.clear();
}
} // namespace poke
