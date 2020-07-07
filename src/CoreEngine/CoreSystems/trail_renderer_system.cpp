#include <CoreEngine/CoreSystems/trail_renderer_system.h>

#include <CoreEngine/engine.h>
#include <Utility/profiler.h>

namespace poke {

TrailRendererSystem::TrailRendererSystem(Engine& engine)
    : System(engine),
      trailRendererManager_(ecsManager_.GetComponentsManager<ecs::TrailRendererManager>()),
      transformManager_(ecsManager_.GetComponentsManager<ecs::TransformsManager>()),
      entities_(1000),
      destroyedEntities_(500),
      newEntities_(500),
      meshManager_(MeshManagerLocator::Get()),
	  modelCommandBuffer_(GraphicsEngineLocator::Get().GetModelCommandBuffer()),
	  meshIDs_(1000),
	  forwardIndexes_(1000),
      dynamicMeshIndex_(1000)
{
    engine_.AddObserver(observer::MainLoopSubject::UPDATE, [this]() { OnUpdate(); });

    engine_.AddObserver(observer::MainLoopSubject::CULLING, [this]() { OnCulling(); });

    engine_.AddObserver(observer::MainLoopSubject::END_FRAME, [this]() { OnEndOfFrame(); });

    ecsManager_.RegisterObserverAddComponent(
        [this](const ecs::EntityIndex entityIndex, const ecs::ComponentMask component) {
            OnAddComponent(entityIndex, component);
        });

    ecsManager_.RegisterObserverRemoveComponent(
        [this](const ecs::EntityIndex entityIndex, const ecs::ComponentMask component) {
            OnRemoveComponent(entityIndex, component);
        });

	ObserveEntitySetActive();
	ObserveEntitySetInactive();
}

void TrailRendererSystem::OnEntitySetActive(const ecs::EntityIndex entityIndex)
{
	if (ecsManager_.HasComponent(entityIndex, ecs::ComponentType::TRAIL_RENDERER)) {
		if (!newEntities_.exist(entityIndex) && !entities_.exist(entityIndex))
			newEntities_.insert(entityIndex);
	}
}

void TrailRendererSystem::OnEntitySetInactive(const ecs::EntityIndex entityIndex)
{
	if (ecsManager_.HasComponent(entityIndex, ecs::ComponentType::TRAIL_RENDERER)) {
		if (!destroyedEntities_.exist(entityIndex) && entities_.exist(entityIndex))
			destroyedEntities_.insert(entityIndex);
	}
}

void TrailRendererSystem::OnUpdate()
{
    pok_BeginProfiling(Trail_renderer_system, 0);
	drawInfosDrawing_.clear();
	drawInfosDrawing_.reserve(entities_.size());
	meshesToUpdates_.reserve(entities_.size());
	for (size_t i = 0; i < entities_.size(); i++) {
		const auto entity = entities_[i];
        auto trailRenderer = trailRendererManager_.GetComponent(entity);

        if (!trailRenderer.isPaused) {
            const auto worldPosition = transformManager_.GetWorldPosition(entity);
            if (trailRenderer.Update(worldPosition)) {
                const auto meshInfo = trailRenderer.GenerateMesh();
                meshesToUpdates_.emplace_back(
					MeshRebuildInfo{
					    dynamicMeshIndex_[i],
					    meshInfo.first,
					    meshInfo.second});
            }

        }

        trailRendererManager_.SetComponent(entity, trailRenderer);

		drawInfosDrawing_.emplace_back(TrailDrawInfos{
			trailRenderer.materialID,
			meshIDs_[i],
			forwardIndexes_[i]
		});
    }
    pok_EndProfiling(Trail_renderer_system);
}

void TrailRendererSystem::OnCulling()
{
	pok_BeginProfiling(Trail_renderer_system, 0);
    for (const auto drawInfos : drawInfosRendering_) {
        modelCommandBuffer_.Draw(
            math::Matrix4::Identity(),
            graphics::Model{
                drawInfos.materialID,
                drawInfos.meshID
            },
            drawInfos.forwardIndex);
    }
	pok_EndProfiling(Trail_renderer_system);
}

void TrailRendererSystem::OnAddComponent(
    const ecs::EntityIndex entityIndex,
    const ecs::ComponentMask component)
{
    if ((component & ecs::ComponentType::TRAIL_RENDERER) == ecs::ComponentType::TRAIL_RENDERER) {
        if(!newEntities_.exist(entityIndex) && !entities_.exist(entityIndex) && ecsManager_.IsEntityActive(entityIndex))
		newEntities_.insert(entityIndex);
    }
}

void TrailRendererSystem::OnRemoveComponent(
    const ecs::EntityIndex entityIndex,
    const ecs::ComponentMask component)
{
    if ((component & ecs::ComponentType::TRAIL_RENDERER) == ecs::ComponentType::TRAIL_RENDERER) {
        if(!destroyedEntities_.exist(entityIndex) && entities_.exist(entityIndex))
		destroyedEntities_.insert(entityIndex);
    }
}

void TrailRendererSystem::OnEndOfFrame()
{
	pok_BeginProfiling(Trail_renderer_system, 0);
	//Update mesh
	pok_BeginProfiling(Update_meshes, 0);
	for (const auto& meshesToUpdate : meshesToUpdates_) {
		meshManager_.UpdateDynamicMesh(
			meshesToUpdate.dynamicMeshIndex,
			meshesToUpdate.vertices,
			meshesToUpdate.indexes);
	}

	meshesToUpdates_.clear();
	pok_EndProfiling(Update_meshes);

    //Destroy entities
	pok_BeginProfiling(Destroy_entities, 0);
    
    for (const auto destroyedEntity : destroyedEntities_) {
		auto trailRenderer = trailRendererManager_.GetComponent(destroyedEntity);
		trailRenderer.Clear();
		trailRendererManager_.SetComponent(destroyedEntity, trailRenderer);

		const auto it = entities_.find(destroyedEntity);
		const auto index = std::distance(entities_.begin(), it);

		modelCommandBuffer_.FreeForwardIndex(forwardIndexes_[index]);
		meshManager_.DestroyDynamicMesh(dynamicMeshIndex_[index]);

		entities_.erase(it);

		dynamicMeshIndex_.erase(dynamicMeshIndex_.begin() + index);
		meshIDs_.erase(meshIDs_.begin() + index);
		forwardIndexes_.erase(forwardIndexes_.begin() + index);
		drawInfosDrawing_.erase(drawInfosDrawing_.begin() + index);
    }
	destroyedEntities_.clear();
	pok_EndProfiling(Destroy_entities);

    //Draw active entities
	pok_BeginProfiling(Transfert_data, 0);
	drawInfosRendering_.clear();
	drawInfosRendering_ = drawInfosDrawing_;
	drawInfosDrawing_.clear();
	pok_EndProfiling(Transfert_data);

    //Add new entities
	pok_BeginProfiling(Add_entities, 0);
	for (const auto newEntity : newEntities_) {
		const auto it = entities_.insert(newEntity);
		const auto dist = std::distance(entities_.begin(), it);

		const auto dynamicMeshId = meshManager_.CreateDynamicMesh();
		dynamicMeshIndex_.insert(dynamicMeshIndex_.begin() + dist, dynamicMeshId);
		meshIDs_.insert(meshIDs_.begin() + dist, meshManager_.GetDynamicMeshResourceID(dynamicMeshId));
		forwardIndexes_.insert(forwardIndexes_.begin() + dist, modelCommandBuffer_.GetForwardIndex());
	}
	newEntities_.clear();
	pok_EndProfiling(Add_entities);
	pok_EndProfiling(Trail_renderer_system);
}
} //namespace poke
