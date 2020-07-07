#include <CoreEngine/CoreSystems/draw_system.h>

#include <CoreEngine/engine.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>
#include <Utility/profiler.h>

namespace poke {
DrawSystem::DrawSystem(Engine& engine)
    : System(engine),
      modelCommandBuffer_(GraphicsEngineLocator::Get().GetModelCommandBuffer()),
      entities_(10000),
      newEntities_(10000),
      forcedDrawEntities_(100),
      newForcedEntities_(10),
      modelsManager_(EcsManagerLocator::Get().GetComponentsManager<ecs::ModelsManager>()),
      transformsManager_(EcsManagerLocator::Get().GetComponentsManager<ecs::TransformsManager>()),
      instancingIndexes_(10000),
      forwardIndexes_(10000)
{
    engine.AddObserver(observer::MainLoopSubject::DRAW, [this]() { OnDraw(); });
    engine.AddObserver(observer::MainLoopSubject::CULLING, [this]() { OnCulling(); });
    engine.AddObserver(observer::MainLoopSubject::RENDER, [this]() { OnRender(); });
    engine.AddObserver(observer::MainLoopSubject::END_FRAME, [this]() { OnEndOfFrame(); });

    ecsManager_.RegisterObserverAddComponent(
        [this](const ecs::EntityIndex entityIndex, const ecs::ComponentMask component) {
            OnAddComponent(entityIndex, component);
        });

    ecsManager_.RegisterObserverRemoveComponent(
        [this](const ecs::EntityIndex entityIndex, const ecs::ComponentMask component) {
            OnRemoveComponent(entityIndex, component);
        });

    ecsManager_.RegisterObserverUpdateComponent(
        [this](const ecs::EntityIndex entityIndex, const ecs::ComponentMask component) {
            OnUpdateComponent(entityIndex, component);
        });

    SceneManagerLocator::Get().AddOnUnloadObserver([this]() { OnUnloadScene(); });
}

void DrawSystem::OnDraw()
{
    //Create drawing info to sent to gpu
    pok_BeginProfiling(Draw_System, 0);

	pok_BeginProfiling(Entities, 0);
	entitiesToDraw_.reserve(entities_.size());
    for (const ecs::EntityIndex entityIndex : entities_) {
        if (ecsManager_.IsEntityVisible(entityIndex)) {
            //Compute aabb
			pok_BeginProfiling(Compute_aabb, 0);
            const auto aabb = meshShapes_[entityIndex].ComputeAABB(
                transformsManager_.GetWorldPosition(entityIndex),
                transformsManager_.GetWorldScale(entityIndex),
                transformsManager_.GetWorldRotation(entityIndex)
            );
			pok_EndProfiling(Compute_aabb);

            //Add Drawing command
			pok_BeginProfiling(Add_drawing_cmd, 0);
            instanceDrawInfos1_[instancingIndexes_[entityIndex]].instances.push_back(
                {
                    transformsManager_.GetLocalToWorldMatrix(entityIndex),
                    aabb,
                    transformsManager_.GetWorldPosition(entityIndex),
                    entityIndex
                });

            entitiesToDraw_.emplace_back(entityIndex);
			pok_EndProfiling(Add_drawing_cmd);
        }
    }
	pok_EndProfiling(Entities);

	pok_BeginProfiling(Forced_drawn_entities, 0);
    for (ecs::EntityIndex entityIndex : forcedDrawEntities_) {
        forwardDrawInfo1_.emplace_back(
            BlockDrawForwardInfo{
                transformsManager_.GetLocalToWorldMatrix(entityIndex),
                modelsManager_.GetComponent(entityIndex),
                forwardIndexes_[entityIndex],
                entityIndex
            });
    }
    pok_EndProfiling(Forced_drawn_entities);

    pok_EndProfiling(Draw_System);
}

void DrawSystem::OnEndOfFrame()
{
    pok_BeginProfiling(Draw_System, 0);

    //TODO(@Nico) create field.
    auto& meshManagerLocator = MeshManagerLocator::Get();

    //Get InstancesIndex for new entities
    if (newEntities_.size() > 0) {
        if (instancingIndexes_.size() < newEntities_.back() + 1) {
            instancingIndexes_.resize(newEntities_.back() + 1);
        }

        for (const auto newEntity : newEntities_) {
            if (entities_.exist(newEntity))
                continue;

            const auto model = modelsManager_.GetComponent(newEntity);

            const auto& mat = MaterialsManagerLocator::Get().GetMaterial(model.materialID);

            const auto& mesh = meshManagerLocator.GetMesh(model.meshID);
            const auto index = modelCommandBuffer_.GetModelInstanceIndex(mat, mesh);
            if (instanceDrawInfos1_.size() < index + 1) { instanceDrawInfos1_.resize(index + 1); }

            instancingIndexes_[newEntity] = index;

            entities_.insert(newEntity);

			if (meshShapes_.size() < newEntity + 1) meshShapes_.resize(newEntity + 1);
			meshShapes_[newEntity] = physics::MeshShape(meshManagerLocator.GetMesh(modelsManager_.GetComponent(newEntity).meshID));
        }
        newEntities_.clear();
    }

    for (auto newForcedEntity : newForcedEntities_) {
        if (forcedDrawEntities_.exist(newForcedEntity))
            continue;
        ecsManager_.SetEntityVisible(newForcedEntity, ecs::EntityStatus::ACTIVE);
        if(forwardIndexes_.size() < newForcedEntity + 1) {
			forwardIndexes_.resize(newForcedEntity + 1);
        }
        forwardIndexes_[newForcedEntity] = modelCommandBuffer_.GetForwardIndex();

        forcedDrawEntities_.insert(newForcedEntity);
    }
    newForcedEntities_.clear();

    auto& camera = CameraLocator::Get();
    const graphics::CameraData cameraData{
        camera.GetProjectionMatrix(),
        camera.GetViewMatrix(),
        camera.GetPosition()
    };
    GraphicsEngineLocator::Get().SetCameraData(cameraData);
    ecsManager_.SetVisibleEntities(drawnEntities_);
    drawnEntities_ = entitiesToDraw_;
    entitiesToDraw_.clear();

    instanceDrawInfos2_ = instanceDrawInfos1_;
    for (auto& infos1 : instanceDrawInfos1_) { infos1.instances.clear(); }

    forwardDrawInfo2_ = forwardDrawInfo1_;
    forwardDrawInfo1_.clear();

    pok_EndProfiling(Draw_System);
}

void DrawSystem::OnCulling()
{
    //Check only object that are visible
    pok_BeginProfiling(Draw_System, 0);
    std::vector<ecs::EntityIndex> visibleEntities;
    visibleEntities.reserve(instanceDrawInfos2_.size());

    const auto frustumPlanes = CameraLocator::Get().GetFrustumPlanes();
    //Forced entities
    for (auto& model : forwardDrawInfo2_) {
        visibleEntities.emplace_back(model.entityIndex);
        modelCommandBuffer_.Draw(
            model.worldMatrix,
            model.model,
            model.forwardIndex);
    }

    const auto cameraPos = CameraLocator::Get().GetPosition();

    //Opaque entities
    for (auto i = 0; i < instanceDrawInfos2_.size(); i++) {
        if (instanceDrawInfos2_[i].frontToBackSorting) {
            std::sort(
                instanceDrawInfos2_[i].instances.begin(),
                instanceDrawInfos2_[i].instances.end(),
                [cameraPos](const BlockDrawInstanceInfo& a, const BlockDrawInstanceInfo& b) {
                    return math::Vec3::GetDistanceManhattan(a.worldPosition, cameraPos) < math::Vec3
                           ::GetDistanceManhattan(b.worldPosition, cameraPos);
                });
        } else {
            std::sort(
                instanceDrawInfos2_[i].instances.begin(),
                instanceDrawInfos2_[i].instances.end(),
                [cameraPos](const BlockDrawInstanceInfo& a, const BlockDrawInstanceInfo& b) {
                    return math::Vec3::GetDistanceManhattan(a.worldPosition, cameraPos) > math::Vec3
                           ::GetDistanceManhattan(b.worldPosition, cameraPos);
                });
        }

        for (const auto& info : instanceDrawInfos2_[i].instances) {
            if (CullAABB(info.aabb, frustumPlanes)) {
                visibleEntities.emplace_back(info.entityIndex);

                modelCommandBuffer_.Draw(info.worldMatrix, i);
            }
        }
    }

    drawnEntities_ = visibleEntities;

    pok_EndProfiling(Draw_System);
}

void DrawSystem::OnRender() { GraphicsEngineLocator::Get().Render(); }

void DrawSystem::OnUnloadScene()
{
    forwardDrawInfo1_.clear();
    forwardDrawInfo2_.clear();
    instanceDrawInfos1_.clear();
    instanceDrawInfos2_.clear();
    instancingIndexes_.clear();
    forwardIndexes_.clear();
}

void DrawSystem::OnAddComponent(
    const ecs::EntityIndex entityIndex,
    const ecs::ComponentMask component)
{
    if ((component & ecs::ComponentType::MODEL) == ecs::ComponentType::MODEL) {
        const auto model = modelsManager_.GetComponent(entityIndex);
        const auto& mat = MaterialsManagerLocator::Get().GetMaterial(model.materialID);

        if (mat.GetType() == graphics::MaterialType::SKYBOX) {
            if (newForcedEntities_.exist(entityIndex))
                return;
            GraphicsEngineLocator::Get().UpdateSkybox(
                *reinterpret_cast<const MaterialSkybox&>(mat).GetTexture());
            newForcedEntities_.insert(entityIndex);
        } else if (mat.GetType() == graphics::MaterialType::DIFFUSE) {
            if (newEntities_.exist(entityIndex))
                return;
            newEntities_.insert(entityIndex);
        }
    }
}

void DrawSystem::OnRemoveComponent(
    const ecs::EntityIndex entityIndex,
    const ecs::ComponentMask component)
{
    if ((component & ecs::ComponentType::MODEL) == ecs::ComponentType::MODEL) {
        if (entities_.exist(entityIndex)) { entities_.erase(entities_.find(entityIndex)); } else if
        (forcedDrawEntities_.exist(entityIndex)) {
            modelCommandBuffer_.FreeForwardIndex(forwardIndexes_[entityIndex]);
            forcedDrawEntities_.erase(forcedDrawEntities_.find(entityIndex));
        }
    }
}

void DrawSystem::OnUpdateComponent(
    const ecs::EntityIndex entityIndex,
    const ecs::ComponentMask component)
{
    if ((component & ecs::ComponentType::MODEL) == ecs::ComponentType::MODEL) {
        const auto model = modelsManager_.GetComponent(entityIndex);
        const auto& mat = MaterialsManagerLocator::Get().GetMaterial(model.materialID);

        switch (mat.GetType()) {
        case graphics::MaterialType::DIFFUSE: {
            if (entities_.exist(entityIndex)) {
                const auto& mesh = MeshManagerLocator::Get().GetMesh(model.meshID);
                instancingIndexes_[entityIndex] = GraphicsEngineLocator::Get()
                                                  .GetModelCommandBuffer().GetModelInstanceIndex(
                                                      mat,
                                                      mesh);

                if (instanceDrawInfos1_.size() < instancingIndexes_[entityIndex] + 1) {
                    instanceDrawInfos1_.resize(instancingIndexes_[entityIndex] + 1);
                }

                meshShapes_[entityIndex] = physics::MeshShape(MeshManagerLocator::Get().GetMesh(modelsManager_.GetComponent(entityIndex).meshID));
            }
        }
        break;
        case graphics::MaterialType::SKYBOX: {
            if (forcedDrawEntities_.exist(entityIndex)) {
                if (mat.GetType() == graphics::MaterialType::SKYBOX) {
                    GraphicsEngineLocator::Get().UpdateSkybox(*reinterpret_cast<const MaterialSkybox&>(mat).GetTexture());
                }
            }
        }
        break;
        case graphics::MaterialType::LENGTH:
            break;
        default: ;
        }
    }
}

bool DrawSystem::CullAABB(
    const physics::AABB aabb,
    const FrustumPlanes& frustumPlanes)
{
    const float minX = aabb.worldPosition.x - aabb.worldExtent.x;
    const float maxX = aabb.worldPosition.x + aabb.worldExtent.x;

    const float minY = aabb.worldPosition.y - aabb.worldExtent.y;
    const float maxY = aabb.worldPosition.y + aabb.worldExtent.y;

    const float minZ = aabb.worldPosition.z - aabb.worldExtent.z;
    const float maxZ = aabb.worldPosition.z + aabb.worldExtent.z;

    // check box outside/inside of frustum
    for (int i = 0; i < 6; i++) {
        if (frustumPlanes[i] * math::Vec4(minX, minY, minZ, 1.0f) <= 0.0f &&
            frustumPlanes[i] * math::Vec4(maxX, minY, minZ, 1.0f) <= 0.0f &&
            frustumPlanes[i] * math::Vec4(minX, maxY, minZ, 1.0f) <= 0.0f &&
            frustumPlanes[i] * math::Vec4(maxX, maxY, minZ, 1.0f) <= 0.0f &&
            frustumPlanes[i] * math::Vec4(minX, minY, maxZ, 1.0f) <= 0.0f &&
            frustumPlanes[i] * math::Vec4(maxX, minY, maxZ, 1.0f) <= 0.0f &&
            frustumPlanes[i] * math::Vec4(minX, maxY, maxZ, 1.0f) <= 0.0f &&
            frustumPlanes[i] * math::Vec4(maxX, maxY, maxZ, 1.0f) <= 0.0f) { return false; }
    }
    return true;
}
} //namespace poke
