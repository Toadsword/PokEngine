#include <CoreEngine/CoreSystems/audio_system.h>

#include <CoreEngine/engine.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>

namespace poke {
AudioSystem::AudioSystem(Engine& engine)
    : System(engine),
      audioEngine_(AudioEngineLocator::Get()),
      audioSourcesManager_(ecsManager_.GetComponentsManager<ecs::AudioSourcesManager>()),
      transformsManager_(ecsManager_.GetComponentsManager<ecs::TransformsManager>()),
      rigidbodyManager_(ecsManager_.GetComponentsManager<ecs::RigidbodyManager>()),
      playOnStartEntities_(50),
      audioSources_(4000),
      playingSounds_(100)
{
    engine.AddObserver(observer::MainLoopSubject::UPDATE, [this]() { OnUpdate(); });

    ObserveEntityDestroy();

    ecsManager_.RegisterObserverAddComponent(
        [this](
        const ecs::EntityIndex entityIndex,
        const ecs::ComponentMask component) {
            OnAddComponent(entityIndex, component);
        });

    ecsManager_.RegisterObserverRemoveComponent(
        [this](
        const ecs::EntityIndex entityIndex,
        const ecs::ComponentMask component) {
            OnRemoveComponent(entityIndex, component);
        });

    audioEngine_.Init();

    //Force load banks
    //TODO(@Nico) Move loading of bank in loading of scene
    const auto masterPath = PokFileSystem::GetAbsoluteFullPath(
        "Master",
        FileType::FMOD_BANK,
        FolderType::ROM);
    const auto level1Path = PokFileSystem::GetAbsoluteFullPath(
        "Level1",
        FileType::FMOD_BANK,
        FolderType::ROM);
    const auto masterStringPath = PokFileSystem::GetAbsoluteFullPath(
        "Master.strings",
        FileType::FMOD_BANK,
        FolderType::ROM);
    audioEngine_.LoadBank(masterPath, FMOD_STUDIO_LOAD_BANK_NORMAL);
    audioEngine_.LoadBank(level1Path, FMOD_STUDIO_LOAD_BANK_NORMAL);
    audioEngine_.LoadBank(masterStringPath, FMOD_STUDIO_LOAD_BANK_NORMAL);
    audioEngine_.LoadEventNames();

    lastPositionCamera_ = CameraLocator::Get().GetPosition();
}

AudioSystem::~AudioSystem() { }

void AudioSystem::OnEngineDestroy() { audioEngine_.Destroy(); }

void AudioSystem::OnUpdate()
{
    const auto& camera = CameraLocator::Get();

    const auto position = camera.GetPosition();

    const math::Vec3 cameraVelocity = math::Vec3::GetDistance(position, lastPositionCamera_) / (
                                    Time::Get().deltaTime.count() / 1000.0f);

    lastPositionCamera_ = position;

    audioEngine_.SetListener(position, cameraVelocity, camera.GetFront(), camera.GetUp());

    for (const auto entityIndex : playOnStartEntities_) {
        math::Vec3 pos(0, 0, 0);

        if (ecsManager_.HasComponent(
            entityIndex,
            ecs::ComponentType::TRANSFORM)) {
            pos = transformsManager_.GetWorldPosition(entityIndex);
        }

        auto audioSource = audioSourcesManager_.GetComponent(entityIndex);
        audioSource.Play(pos);
        audioSourcesManager_.SetComponent(entityIndex, audioSource);

        playingSounds_.insert(entityIndex);
    }

    playOnStartEntities_.clear();

    for (auto entityIndex : audioSources_) {
		auto audioSource = audioSourcesManager_.GetComponent(entityIndex);

        if(audioSource.IsPlaying()) {
            if(!playingSounds_.exist(entityIndex)) {
				playingSounds_.insert(entityIndex);
            }
        }
    }

    std::vector<ecs::EntityIndex> entitiesToRemove;
    entitiesToRemove.reserve(playingSounds_.size());
    for (auto entityIndex : playingSounds_) {
        auto audioSource = audioSourcesManager_.GetComponent(entityIndex);

        if (!audioSource.IsPlaying()) {
            entitiesToRemove.emplace_back(entityIndex);
        } else {
            const math::Vec3 pos = transformsManager_.GetComponent(entityIndex).GetLocalPosition();
            math::Vec3 velocity = {0, 0, 0};
            math::Vec3 forward = camera.GetFront() * -1;
            if (ecsManager_.HasComponent(entityIndex, ecs::ComponentType::RIGIDBODY)) {
                velocity = rigidbodyManager_.GetComponent(entityIndex).linearVelocity;
                forward = velocity;
            }
            const math::Vec3 up = {0, 1, 0};

            audioSource.Update3DAttributes(pos, velocity, forward, up);
        }

        audioSourcesManager_.SetComponent(entityIndex, audioSource);
    }

    for (auto toRemove : entitiesToRemove) { playingSounds_.erase(playingSounds_.find(toRemove)); }

    audioEngine_.Update();
}

void AudioSystem::OnAddComponent(
    const ecs::EntityIndex entityIndex,
    const ecs::ComponentMask component)
{
    if (ecs::ComponentType::AUDIO_SOURCE == (component & ecs::ComponentType::AUDIO_SOURCE)) {
        const auto audioSource = audioSourcesManager_.GetComponent(entityIndex);
        if (audioSource.playOnLoadScene) { playOnStartEntities_.insert(entityIndex); }

        audioSources_.insert(entityIndex);
    }
}

void AudioSystem::
OnRemoveComponent(const ecs::EntityIndex entityIndex, const ecs::ComponentMask component)
{
    if (ecs::ComponentType::AUDIO_SOURCE == (component & ecs::ComponentType::AUDIO_SOURCE)) {
        if (audioSources_.exist(entityIndex)) {
            audioSources_.erase(audioSources_.find(entityIndex));

            //If the sounds is still playing then remove it
            if (playingSounds_.exist(entityIndex)) {
                auto audioSource = audioSourcesManager_.GetComponent(entityIndex);
                audioSource.Stop();
                audioSourcesManager_.SetComponent(entityIndex, audioSource);
                playingSounds_.erase(playingSounds_.find(entityIndex));
            }
        }
    }
}
} //namespace poke
