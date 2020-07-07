#include <CoreEngine/CoreSystems/segment_renderer_system.h>

#include <CoreEngine/ServiceLocator/service_locator_definition.h>
#include <Utility/profiler.h>
#include <CoreEngine/engine.h>
#include "Math/quaternion.h"

namespace poke {
namespace ecs {

} //namespace ecs
SegmentRendererSystem::SegmentRendererSystem(Engine& engine) :
    System(engine),
    segmentRendererManager_(ecsManager_.GetComponentsManager<ecs::SegmentRendererManager>()),
    transformManager_(ecsManager_.GetComponentsManager<ecs::TransformsManager>()),
    entities_(10000),
    destroyedEntities_(10000),
    newEntities_(10000),
	modelCommandBuffer_(GraphicsEngineLocator::Get().GetModelCommandBuffer())
{
    ecsManager_.RegisterObserverAddComponent(
		[this](const ecs::EntityIndex entity, const ecs::ComponentMask component) {
		OnAddComponent(entity, component);
	});

	ecsManager_.RegisterObserverRemoveComponent(
		[this](const ecs::EntityIndex entity, const ecs::ComponentMask component) {
		OnRemoveComponent(entity, component);
	});

	ecsManager_.AddObserver(
        observer::EntitiesSubjects::SET_ACTIVE,
		[this](const ecs::EntityIndex entity) {
		OnEntitySetActive(entity);
	});

	ecsManager_.AddObserver(
		observer::EntitiesSubjects::SET_INACTIVE,
		[this](const ecs::EntityIndex entity) {
		OnEntitySetInactive(entity);
	});

	engine_.AddObserver(
		observer::MainLoopSubject::UPDATE,
		[this]() {OnUpdate(); });

	engine_.AddObserver(
		observer::MainLoopSubject::CULLING,
		[this]() {OnCulling(); });

	engine_.AddObserver(
		observer::MainLoopSubject::END_FRAME,
		[this]() {OnEndOfFrame(); });
}

void SegmentRendererSystem::OnEntitySetActive(const ecs::EntityIndex entityIndex)
{
	if (ecsManager_.HasComponent(entityIndex, ecs::ComponentType::SEGMENT_RENDERER)) {
		if (!newEntities_.exist(entityIndex) && !entities_.exist(entityIndex))
			newEntities_.insert(entityIndex);
	}
}

void SegmentRendererSystem::OnEntitySetInactive(const ecs::EntityIndex entityIndex)
{
	if (ecsManager_.HasComponent(entityIndex, ecs::ComponentType::SEGMENT_RENDERER)) {
		if (!destroyedEntities_.exist(entityIndex) && entities_.exist(entityIndex))
			destroyedEntities_.insert(entityIndex);
	}
}

void SegmentRendererSystem::OnUpdate()
{
	auto& gizmoCommandBuffer = GraphicsEngineLocator::Get().GetGizmoCommandBuffer();

	for (size_t i = 0; i < entities_.size(); i++) {
		auto segmentRenderer = segmentRendererManager_.GetComponent(entities_[i]);

		auto currentPosition = transformManager_.GetWorldPosition(entities_[i]);

		if (currentPosition == previousPos_[i]) continue;

		auto direction = (currentPosition - previousPos_[i]).Normalize();

		math::Vec3 endPos;
		distances_[i] = math::Vec3::GetDistance(currentPosition, previousPos_[i]);
		LogDebug("Distance = " + std::to_string(distances_[i]));
        if(distances_[i] >= segmentRenderer.lifetime - 0.1f || states_[i] == SegmentState::IDLE) {
			states_[i] = SegmentState::IDLE;
			LogDebug("Continue");
			previousPos_[i] = currentPosition - direction * segmentRenderer.lifetime;
            endPos = currentPosition - direction * segmentRenderer.lifetime;

			centerPos_[i] = (currentPosition + endPos) / 2.0f;

			previousPos_[i] = currentPosition;
        }else {
			LogDebug("Grow");
			endPos = previousPos_[i];

			centerPos_[i] = (currentPosition + endPos) / 2.0f;

			//previousPos_[i] = currentPosition;
        }

		math::Matrix4 mat = math::Matrix4::Identity();

		mat = math::Matrix4::Translate(mat, centerPos_[i]);

		auto p1 = math::Vec3{ 0, 1, 0 };
		p1 = p1.Normalize();
		auto p2 = previousPos_[i] - centerPos_[i];
		p2 = p2.Normalize();

		const math::Quaternion quat = math::Quaternion::FromToRotation(p1, p2);
		math::Vec3 rotation = quat.GetEulerAngles();

		mat = math::Matrix4::Rotate(mat, rotation.x, math::Vec3(1, 0, 0));
		mat = math::Matrix4::Rotate(mat, rotation.y, math::Vec3(0, 1, 0));
		mat = math::Matrix4::Rotate(mat, rotation.z, math::Vec3(0, 0, 1));

		mat = math::Matrix4::Scale(
			mat, 
			math::Vec3(
				segmentRendererManager_.GetComponent(entities_[i]).width,
				math::Vec3::GetDistance(centerPos_[i], currentPosition) * 2,
				segmentRendererManager_.GetComponent(entities_[i]).width
			)
		);

		drawInfosDrawing_.push_back(SegmentDrawInfos{
			mat,
			instanceIndexes_[i]
			});
	}
}

void SegmentRendererSystem::OnCulling()
{
	for (size_t i = 0; i < drawInfosRendering_.size(); i++) {
		modelCommandBuffer_.Draw(drawInfosRendering_[i].worldsMatrix, drawInfosRendering_[i].instanceIndex);
    }
}

void SegmentRendererSystem::OnAddComponent(
    const ecs::EntityIndex entityIndex,
    const ecs::ComponentMask component)
{
	if ((component & ecs::ComponentType::SEGMENT_RENDERER) == ecs::ComponentType::SEGMENT_RENDERER) {
		if (!newEntities_.exist(entityIndex) && !entities_.exist(entityIndex) && ecsManager_.IsEntityActive(entityIndex))
			newEntities_.insert(entityIndex);
	}
}

void SegmentRendererSystem::OnRemoveComponent(
    const ecs::EntityIndex entityIndex,
    const ecs::ComponentMask component)
{
	if ((component & ecs::ComponentType::SEGMENT_RENDERER) == ecs::ComponentType::SEGMENT_RENDERER) {
		if (!destroyedEntities_.exist(entityIndex) && entities_.exist(entityIndex))
			destroyedEntities_.insert(entityIndex);
	}
}

void SegmentRendererSystem::OnEndOfFrame()
{
	pok_BeginProfiling(Segment_renderer_system, 0);

	//Destroy entities
	pok_BeginProfiling(Destroy_entities, 0);

	for (const auto destroyedEntity : destroyedEntities_) {
		const auto it = entities_.find(destroyedEntity);
		const auto index = std::distance(entities_.begin(), it);

		entities_.erase(it);

		instanceIndexes_.erase(instanceIndexes_.begin() + index);
		drawInfosDrawing_.erase(drawInfosDrawing_.begin() + index);
		previousPos_.erase(previousPos_.begin() + index);
		centerPos_.erase(centerPos_.begin() + index);
		states_.erase(states_.begin() + index);
		distances_.erase(distances_.begin() + index);
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

		const auto& segmentRenderer = segmentRendererManager_.GetComponent(newEntity);

		previousPos_.insert(previousPos_.begin() + dist, transformManager_.GetWorldPosition(newEntity));
		centerPos_.insert(centerPos_.begin() + dist, transformManager_.GetWorldPosition(newEntity));
		states_.insert(states_.begin() + dist, SegmentState::GROWING);
		distances_.insert(distances_.begin() + dist, 0);

		instanceIndexes_.insert(
			instanceIndexes_.begin() + dist,
			modelCommandBuffer_.GetModelInstanceIndex(
				MaterialsManagerLocator::Get().GetMaterial(segmentRenderer.materialID),
				MeshManagerLocator::Get().GetMesh("cylinder.obj")));
	}
	newEntities_.clear();
	pok_EndProfiling(Add_entities);
	pok_EndProfiling(Segment_renderer_system);
}
} //namespace poke