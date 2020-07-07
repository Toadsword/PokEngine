#include <CoreEngine/CoreSystems/phyics_system.h>

#include <Ecs/ComponentManagers/rigidbody_manager.h>
#include <Ecs/ComponentManagers/colliders_manager.h>
#include <CoreEngine/engine.h>
#include <Utility/profiler.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>

namespace poke {
PhysicsSystem::PhysicsSystem(Engine& engine)
    : System(engine),
      transformsManager_(EcsManagerLocator::Get().GetComponentsManager<ecs::TransformsManager>()),
      rigidbodyManager_(EcsManagerLocator::Get().GetComponentsManager<ecs::RigidbodyManager>()),
      collidersManager_(EcsManagerLocator::Get().GetComponentsManager<ecs::CollidersManager>())
{
    engine_.AddObserver(observer::MainLoopSubject::PHYSICS_UPDATE, [this]() { OnPhysicUpdate(); });
    ObserveUnloadScene();
    ObserveEntityAddComponent();
    ObserveEntityDestroy();
    ObserveEntitySetActive();
    ObserveEntitySetInactive();

    auto& physicsEngine = PhysicsEngineLocator::Get();

    physicsEngine.SetCallbackNotifyOnTriggerEnter(
        [this](const ecs::EntityIndex entityIndex, const physics::Collision collision) {
            this->ecsManager_.NotifyTriggerEnter(
                entityIndex,
                collision);
        });

    physicsEngine.SetCallbackNotifyOnTriggerExit(
        [this](const ecs::EntityIndex entityIndex, const physics::Collision collision) {
            this->ecsManager_.NotifyTriggerExit(
                entityIndex,
                collision);
        });

    physicsEngine.SetCallbackNotifyOnColliderEnter(
        [this](const ecs::EntityIndex entityIndex, const physics::Collision collision) {
            this->ecsManager_.NotifyColliderEnter(
                entityIndex,
                collision);
        });

    physicsEngine.SetCallbackNotifyOnColliderExit(
        [this](const ecs::EntityIndex entityIndex, const physics::Collision collision) {
            this->ecsManager_.NotifyColliderExit(
                entityIndex,
                collision);
        });
}

void PhysicsSystem::OnPhysicUpdate()
{
    pok_BeginProfiling(Physics_System, 0);

    pok_BeginProfiling(Pre_batch, 0);
    auto& physicsEngine = PhysicsEngineLocator::Get();

    physicsEngineData_.entities.resize(entities_.size());
    physicsEngineData_.worldTransforms.resize(entities_.size());
    physicsEngineData_.rigidbodies.resize(entities_.size());
    physicsEngineData_.colliders.resize(entities_.size());

    //Updates collision
    int i = 0;
    for (const int entity : entities_) {
        if (ecsManager_.IsEntityActive(entity)) {
            physicsEngineData_.entities[i] = entity;
            physicsEngineData_.worldTransforms[i] = transformsManager_.GetWorldTransform(entity);
            physicsEngineData_.rigidbodies[i] = rigidbodyManager_.GetComponent(entity);
            physicsEngineData_.colliders[i] = collidersManager_.GetComponent(entity);
            i++;
        }
    }

    physicsEngineData_.entities.resize(i);
    physicsEngineData_.worldTransforms.resize(i);
    physicsEngineData_.rigidbodies.resize(i);
    physicsEngineData_.colliders.resize(i);

    //Set new physicsData
    physicsEngine.SetPhysicsEngineData(physicsEngineData_);

    //Destroyed old 
    physicsEngine.ClearEntities(destroyedEntities_);
    destroyedEntities_.clear();
    destroyedEntities_.resize(0);
    pok_EndProfiling(Pre_batch);

    //Update physicsEngine
    pok_BeginProfiling(Physics_engine, 0);
    physicsEngine.OnPhysicUpdate();
    pok_EndProfiling(Physics_engine);

    pok_BeginProfiling(Update_Transforms, 0);
    //Update current transform
    auto& physicsEngineData = physicsEngine.GetPhysicsEngineData();

    for (size_t index = 0; index < physicsEngineData.entities.size(); index++) {
        transformsManager_.SetComponentFromWorldTransform(
            physicsEngineData.entities[index],
            physicsEngineData.worldTransforms[index]);
    }
    pok_EndProfiling(Update_Transforms);

    pok_BeginProfiling(Clear_data, 0);
    physicsEngineData_.entities.clear();
    physicsEngineData_.worldTransforms.clear();
    physicsEngineData_.colliders.clear();
    physicsEngineData_.rigidbodies.clear();

    pok_EndProfiling(Clear_data);
    pok_EndProfiling(Physics_System);
}

void PhysicsSystem::OnUnloadScene() { PhysicsEngineLocator::Get().ClearCollisions(); }

void PhysicsSystem::OnEntityDestroy(const ecs::EntityIndex entityIndex)
{
    auto it = std::find(destroyedEntities_.begin(), destroyedEntities_.end(), entityIndex);
    if (it != destroyedEntities_.end())
        return;

    destroyedEntities_.push_back(entityIndex);

    it = std::find(entities_.begin(), entities_.end(), entityIndex);

    if (it != entities_.end()) { entities_.erase(it); }
}

void PhysicsSystem::OnEntityAddComponent(
    ecs::EntityIndex entityIndex,
    const ecs::ComponentMask component)
{
    if ((component & ecs::ComponentType::RIGIDBODY) == ecs::ComponentType::RIGIDBODY &&
        ecsManager_.IsEntityActive(entityIndex)) {
        if (physicsEngineData_.entities.capacity() == physicsEngineData_.entities.size()) {
            physicsEngineData_.entities.reserve((physicsEngineData_.entities.size() + 1) * 2);
            physicsEngineData_.worldTransforms.reserve(physicsEngineData_.entities.capacity());
            physicsEngineData_.rigidbodies.reserve(physicsEngineData_.entities.capacity());
            physicsEngineData_.colliders.reserve(physicsEngineData_.entities.capacity());
            entities_.reserve(physicsEngineData_.entities.capacity());
        }

        const auto it = std::find(entities_.begin(), entities_.end(), entityIndex);
        if (it != entities_.end())
            return;
        entities_.emplace_back(entityIndex);
    }
}

void PhysicsSystem::OnEntitySetActive(ecs::EntityIndex entityIndex)
{
    if (ecsManager_.HasComponent(entityIndex, ecs::ComponentType::RIGIDBODY)) {
        if (physicsEngineData_.entities.capacity() == physicsEngineData_.entities.size()) {
            physicsEngineData_.entities.reserve((physicsEngineData_.entities.size() + 1) * 2);
            physicsEngineData_.worldTransforms.reserve(physicsEngineData_.entities.capacity());
            physicsEngineData_.rigidbodies.reserve(physicsEngineData_.entities.capacity());
            physicsEngineData_.colliders.reserve(physicsEngineData_.entities.capacity());
            entities_.reserve(physicsEngineData_.entities.capacity());
        }

        const auto it = std::find(entities_.begin(), entities_.end(), entityIndex);
        if (it != entities_.end())
            return;
        entities_.emplace_back(entityIndex);
    }
}

void PhysicsSystem::OnEntitySetInactive(const ecs::EntityIndex entityIndex)
{
    auto it = std::find(destroyedEntities_.begin(), destroyedEntities_.end(), entityIndex);
    if (it != destroyedEntities_.end())
        return;

    destroyedEntities_.push_back(entityIndex);

    it = std::find(entities_.begin(), entities_.end(), entityIndex);

    if (it != entities_.end()) { entities_.erase(it); }
}
} //namespace poke
