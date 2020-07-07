#include <Ecs/ComponentManagers/audio_sources_manager.h>

namespace poke {
namespace ecs {

void AudioSourcesManager::ClearEntity(const EntityIndex entityIndex)
{
	audioSources_[entityIndex].playOnLoadScene = false;
}

void AudioSourcesManager::ResizeEntities(const size_t size)
{
    audioSources_.resize(size);
}

void AudioSourcesManager::SetWithArchetype(
    const EntityPool entityPool,
    const Archetype& archetype)
{
    for (auto entityIndex = entityPool.firstEntity;
         entityIndex < entityPool.lastEntity; entityIndex++) {
        audioSources_[entityIndex] = archetype.audioSource;
    }
}

const audio::AudioSource& AudioSourcesManager::GetComponent(
    const EntityIndex entityIndex) const { return audioSources_[entityIndex]; }

void AudioSourcesManager::SetComponent(
    const EntityIndex entityIndex,
    const audio::AudioSource& audioSource)
{
    audioSources_[entityIndex] = audioSource;
}

void AudioSourcesManager::InsertArchetype(
    const EntityIndex entity,
    const Archetype& archetype)
{
    audioSources_.insert(audioSources_.begin() + entity, archetype.audioSource);
}

void AudioSourcesManager::EraseEntities(
    const EntityPool pool,
    const size_t nbObjectToErase)
{
    audioSources_.erase(
        audioSources_.begin() + pool.firstEntity,
        audioSources_.begin() + pool.firstEntity + nbObjectToErase);
}

void AudioSourcesManager::SetComponentFromJson(
    const EntityIndex entityIndex,
    const json& componentJson)
{
    audioSources_[entityIndex].SetFromJson(componentJson);
}

json AudioSourcesManager::GetJsonFromComponent(const EntityIndex entityIndex)
{
    return audioSources_[entityIndex].ToJson();
}
} //namespace ecs
} //namespace poke
