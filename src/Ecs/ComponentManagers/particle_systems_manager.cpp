#include <Ecs/ComponentManagers/particle_systems_manager.h>
#include <GraphicsEngine/Particles/particle_command_buffer.h>

namespace poke {
namespace graphics {} //namespace graphics
ecs::ParticleSystemsManager::ParticleSystemsManager()
{
    
}

void ecs::ParticleSystemsManager::ClearEntity(const EntityIndex entityIndex)
{
	particleSystems_[entityIndex].lifetime = 0;
}

void ecs::ParticleSystemsManager::Clear()
{
    particleSystems_.clear();
}

void ecs::ParticleSystemsManager::ResizeEntities(const size_t size)
{
    particleSystems_.resize(size);
}

void ecs::ParticleSystemsManager::SetWithArchetype(
    const EntityPool entityPool,
    const Archetype& archetype)
{
	for (EntityIndex entityIndex = entityPool.firstEntity; entityIndex < entityPool.lastEntity;
		entityIndex++) {
		particleSystems_[entityIndex] = archetype.particleSystem;
	}
}

void ecs::ParticleSystemsManager::InsertArchetype(
    const EntityIndex entity,
    const Archetype& archetype)
{
	particleSystems_.insert(particleSystems_.begin() + entity, archetype.particleSystem);
}

void ecs::ParticleSystemsManager::EraseEntities(
    const EntityPool pool,
    const size_t nbObjectToErase)
{
	particleSystems_.erase(
		particleSystems_.begin() + pool.firstEntity,
		particleSystems_.begin() + pool.firstEntity + nbObjectToErase);
}


void ecs::ParticleSystemsManager::SetComponentFromJson(
    const EntityIndex entityIndex,
    const json& componentJson)
{
	particleSystems_[entityIndex].SetFromJson(componentJson);
}

json ecs::ParticleSystemsManager::
GetJsonFromComponent(const EntityIndex entityIndex)
{
	return particleSystems_[entityIndex].ToJson();
}

const graphics::ParticleSystem& ecs::ParticleSystemsManager::GetComponent(
    const EntityIndex entityIndex) const
{
    return particleSystems_[entityIndex];
}

void ecs::ParticleSystemsManager::SetComponent(
    const EntityIndex entityIndex,
    const graphics::ParticleSystem& particleSystem)
{
    particleSystems_[entityIndex] = particleSystem;
}
} //namespace poke
