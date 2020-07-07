#include <Editor/gizmo_utility.h>

#include <CoreEngine/CoreSystems/spline_follower_system.h>
#include <Game/Systems/spline_state_system.h>
#include <Editor/Ecs/editor_ecs_manager.h>
#include <Editor/imgui_gradient.h>
#include <Ecs/ecs_utility.h>

#include <Editor/Tools/chunk_editor_tool.h>

namespace poke {
namespace editor {
const void GizmoUtility::DisplayAabbGizmo(
	const physics::AABB& aabb, 
	Color color) {

	if (gizmoFlags_ & GIZMO_FLAG_AABB) {
		graphics::GizmoCommandBuffer& gizmoCommandBuffer =
			GraphicsEngineLocator::Get().GetGizmoCommandBuffer();

	    gizmoCommandBuffer.SetColor(color);
		gizmoCommandBuffer.DrawWireCube(aabb.worldPosition, aabb.worldExtent);
	}
}

const void GizmoUtility::DisplayAllChunksGizmo() {

	std::vector<chunk::ChunkIndex> chunkIndexes = ChunkEditorTool::GetChunksToDisplay();
	const uint8_t nbChunks = ChunksManagerLocator::Get().GetChunkNb();

	if (chunkIndexes.size() <= 0) {
		for (int i = 0; i < nbChunks; i++) {
			DisplayChunkGizmo(i);
		}
	}

	if (chunkIndexes.size() > 0) {
		for (int i = 0; i < chunkIndexes.size(); i++) {
			DisplayChunkGizmo(chunkIndexes[i]);
		}
	}
}

const void poke::editor::GizmoUtility::DisplayChunkGizmo(uint8_t chunkIndex) {

    const uint8_t nbChunks = ChunksManagerLocator::Get().GetChunkNb();
	if (gizmoFlags_ & GIZMO_FLAG_CHUNK &&
		chunkIndex < nbChunks) 
	{
		graphics::GizmoCommandBuffer& gizmoCommandBuffer =
			GraphicsEngineLocator::Get().GetGizmoCommandBuffer();
        const auto& chunks = ChunksManagerLocator::Get().GetChunks();

		switch (chunks[chunkIndex].GetStatus()) {
		case chunk::ChunkStatus::VISIBLE:
			gizmoCommandBuffer.SetColor(gizmoColorChunkVisible_);
			break;
		case chunk::ChunkStatus::HIDDEN:
			gizmoCommandBuffer.SetColor(gizmoColorChunkHidden_);
			break;
		case chunk::ChunkStatus::ACTIVE:
			gizmoCommandBuffer.SetColor(gizmoColorChunkActive_);
			break;
		case chunk::ChunkStatus::DESTROYED:
			gizmoCommandBuffer.SetColor(gizmoColorChunkDestroyed_);
			break;
		case chunk::ChunkStatus::LENGTH:
			break;
		default:;
		}

		gizmoCommandBuffer.DrawWireCube(
			chunks[chunkIndex].GetPosition(),
			chunks[chunkIndex].GetExtent()
		);
	    
	    gizmoCommandBuffer.SetColor(Color::white);
	}
}

const void GizmoUtility::DisplayTransformGizmo(
	const math::Transform& transform,
    Color color)
{
	if (gizmoFlags_ & GIZMO_FLAG_TRANSFORM) {

		graphics::GizmoCommandBuffer& gizmoCommandBuffer =
			GraphicsEngineLocator::Get().GetGizmoCommandBuffer();
		gizmoCommandBuffer.SetColor(color);

		gizmoCommandBuffer.DrawWireSphere(transform.GetLocalPosition(), transformPointRadius_);
	}
}

const void GizmoUtility::DisplayColliderGizmo(
	const physics::Collider& collider, 
	const math::Transform& transform, 
	Color color) {

	if (gizmoFlags_ & GIZMO_FLAG_COLLIDER) {
		graphics::GizmoCommandBuffer& gizmoCommandBuffer =
			GraphicsEngineLocator::Get().GetGizmoCommandBuffer();
		gizmoCommandBuffer.SetColor(color);

		switch (collider.shapeType) {
		case physics::ShapeType::BOX: {		    
			gizmoCommandBuffer.DrawWireCube(
				transform.GetLocalPosition() + collider.boxShape.GetCenter(),
				{
					transform.GetLocalScale().x * collider.boxShape.GetExtent().x,
					transform.GetLocalScale().y * collider.boxShape.GetExtent().y,
					transform.GetLocalScale().z * collider.boxShape.GetExtent().z,
				},
				transform.GetLocalRotation());
			break;
		}
		case physics::ShapeType::SPHERE: {   
			gizmoCommandBuffer.DrawWireSphere(
				transform.GetLocalPosition() + collider.boxShape.GetCenter(),
				std::max(transform.GetLocalScale().x, transform.GetLocalScale().y) *collider.sphereShape.GetRadius());
			break;
		}
		case physics::ShapeType::ELLIPSOID: {
			break;
		}
		case physics::ShapeType::MESH: {		    
			gizmoCommandBuffer.DrawWireCube(
				transform.GetLocalPosition() + collider.meshShape.GetOffset(),
				{
					transform.GetLocalScale().x * collider.meshShape.GetMeshExtent().x,
					transform.GetLocalScale().y * collider.meshShape.GetMeshExtent().y,
					transform.GetLocalScale().z * collider.meshShape.GetMeshExtent().z,
				},
				transform.GetLocalRotation());
			break;
		}
		case physics::ShapeType::RECT: break;
		default:;
		}
	}
}

const void GizmoUtility::DisplayDirectionGizmo(
    const math::Vec3& origin,
	const math::Vec3& direction,
	Color color) {

    if(gizmoFlags_ & GIZMO_FLAG_DIRECTION) {
		graphics::GizmoCommandBuffer& gizmoCommandBuffer = 
			GraphicsEngineLocator::Get().GetGizmoCommandBuffer();

		math::Vec3 end = origin + (direction.Normalize() * directionLineLength_);

        gizmoCommandBuffer.SetColor(color);
        gizmoCommandBuffer.DrawLine(origin, end);

        gizmoCommandBuffer.DrawWireSphere(end, endArrowSphereRadius_);

    }
}

const void GizmoUtility::DisplaySplineGizmo(
	const math::CubicHermiteSpline& spline,
	Color color)
{
	if (gizmoFlags_ & GIZMO_FLAG_SPLINE) {
		graphics::GizmoCommandBuffer& gizmoCommandBuffer = 
			GraphicsEngineLocator::Get().GetGizmoCommandBuffer();
		std::vector<math::Vec3> splinePoints = spline.GetPoints();
		for (int i = 0; i < splinePoints.size(); i++) {
			gizmoCommandBuffer.SetColor(
				Color(
					static_cast<float>(i) * (1.0f / splinePoints.size()),
					static_cast<float>(i) * (1.0f / splinePoints.size()),
					static_cast<float>(i) * (1.0f / splinePoints.size())));

			gizmoCommandBuffer.DrawWireSphere(splinePoints[i], 0.1f);
		}
		gizmoCommandBuffer.SetColor(Color::red);

		for (int i = 1; i + 2 < splinePoints.size(); i++) {
			for (float j = 0; j < 1; j += iterationSpace_) {
				gizmoCommandBuffer.DrawLine(spline.Lerp(i, j), spline.Lerp(i, j + iterationSpace_));
				gizmoCommandBuffer.
					DrawWireSphere(
						spline.Lerp(i, j),
						0.01f);
			}
		}

		gizmoCommandBuffer.SetColor(Color::blue);
	}
}

const void GizmoUtility::DisplaySplineGizmo(
	const ecs::SplineFollower& splineFollower,
	Color splineColor,
	Color followerColor) {

	if (gizmoFlags_ & GIZMO_FLAG_SPLINE) {

		DisplaySplineGizmo(splineFollower.spline, splineColor);

		graphics::GizmoCommandBuffer& gizmoCommandBuffer = 
			GraphicsEngineLocator::Get().GetGizmoCommandBuffer();

		gizmoCommandBuffer.SetColor(followerColor);
		gizmoCommandBuffer.DrawWireSphere(
			splineFollower.spline.Lerp(
				splineFollower.lastPoint,
				splineFollower.segmentPercentage),
			iterationSpace_);
	}
}

const void GizmoUtility::DisplaySplinePointsGizmo(
	const game::SplineStates & splineStates, 
	const ecs::SplineFollower& splineFollower, 
	Color splineStatesColor,
	Color directionColor)
{
	if (gizmoFlags_ & GIZMO_FLAG_SPLINE_POINTS) {
        
		graphics::GizmoCommandBuffer& gizmoCommandBuffer =
			GraphicsEngineLocator::Get().GetGizmoCommandBuffer();

		const std::vector<math::Vec3> spline = splineFollower.spline.GetPoints();
		size_t maxSize = std::min(spline.size(), splineStates.pointStates.size());
		maxSize = std::min(maxSize - 2, maxSize);
        for(size_t i = 1; i < maxSize; i++) {

            //Display front 
			gizmoCommandBuffer.SetColor(directionColor);

            // Origin =  spline[i]
			const math::Vec3 front = spline[i + 1] - spline[i];
			const math::Vec3 endFront = spline[i] + (front.Normalize() * directionLineLength_);
			gizmoCommandBuffer.DrawLine(spline[i], endFront);
			gizmoCommandBuffer.DrawWireSphere(endFront, endArrowSphereRadius_);
			
            //Display up
			gizmoCommandBuffer.SetColor(splineStatesColor);
			
			const math::Vec3 up = splineStates.pointStates[i].rotation;
			const math::Vec3 endUp = spline[i] + (up.Normalize() * directionLineLength_* 1.5f);
			gizmoCommandBuffer.DrawLine(spline[i], endUp);
			gizmoCommandBuffer.DrawWireSphere(endUp, endArrowSphereRadius_);
			
            //Display right
			gizmoCommandBuffer.SetColor(splineStatesColor);
			math::Vec3 right = front;
			right = right.Cross(up);
			const math::Vec3 endRight = spline[i] + (right.Normalize() * directionLineLength_);
			gizmoCommandBuffer.DrawLine(spline[i], endRight);
			gizmoCommandBuffer.DrawWireSphere(endRight, endArrowSphereRadius_);
			
        }
	}
}

void GizmoUtility::DisplayGizmos() {
	EditorEcsManager& editorEcsManager = editor_->GetEditorEcsManager();
	std::vector<ecs::EntityIndex> entitiesToDisplay = editorEcsManager.GetActiveEntities();
	for (ecs::EntityIndex entityIndex : entitiesToDisplay) {

	    //Transform
		math::Transform entityTransform;
        if(editorEcsManager.HasComponent(entityIndex, ecs::ComponentType::TRANSFORM)) {
			entityTransform = editorEcsManager.GetComponentsManager<ecs::TransformsManager>().GetWorldTransform(entityIndex);
			DisplayTransformGizmo(entityTransform);
        }

        //Collider
		if (editorEcsManager.HasComponent(entityIndex, ecs::ComponentType::COLLIDER)) {
			DisplayColliderGizmo(
				editorEcsManager.GetComponentsManager<ecs::CollidersManager>().GetComponent(entityIndex),
				entityTransform
			);
		}

        //Rigidbody
        if(editorEcsManager.HasComponent(entityIndex, ecs::ComponentType::RIGIDBODY)) {
			DisplayDirectionGizmo(
				entityTransform.GetLocalPosition(),
				editorEcsManager.GetComponentsManager<ecs::RigidbodyManager>().GetComponent(entityIndex).linearVelocity
			);
        }

        //Spline follower
		if (editorEcsManager.HasComponent(entityIndex, ecs::ComponentType::SPLINE_FOLLOWER)) {
			DisplaySplineGizmo(
				editorEcsManager.GetComponentsManager<ecs::SplineFollowersManager>().GetComponent(entityIndex)
			);
		}

        //Spline points
		if (editorEcsManager.HasComponent(entityIndex, ecs::ComponentType::SPLINE_FOLLOWER) && 
			editorEcsManager.HasComponent(entityIndex, ecs::ComponentType::SPLINE_STATES)) {
			DisplaySplinePointsGizmo(
				editorEcsManager.GetComponentsManager<game::SplineStateManager>().GetComponent(entityIndex),
				editorEcsManager.GetComponentsManager<ecs::SplineFollowersManager>().GetComponent(entityIndex)
			);
		}
	}

	DisplayAllChunksGizmo();
}

void GizmoUtility::DisplayGizmoMenu(const char* label) {
	if (ImGui::BeginMenu(label)) {
		bool allDisplayed = true;
		allDisplayed = DisplaySubMenuGizmoItem(GIZMO_FLAG_AABB) && allDisplayed;
		allDisplayed = DisplaySubMenuGizmoItem(GIZMO_FLAG_CHUNK) && allDisplayed;
		allDisplayed = DisplaySubMenuGizmoItem(GIZMO_FLAG_COLLIDER) && allDisplayed;
		allDisplayed = DisplaySubMenuGizmoItem(GIZMO_FLAG_DIRECTION) && allDisplayed;
		allDisplayed = DisplaySubMenuGizmoItem(GIZMO_FLAG_SPLINE) && allDisplayed;
		allDisplayed = DisplaySubMenuGizmoItem(GIZMO_FLAG_SPLINE_POINTS) && allDisplayed;
		allDisplayed = DisplaySubMenuGizmoItem(GIZMO_FLAG_TRANSFORM) && allDisplayed;
		if (ImGui::MenuItem(
			GetGizmoName(GIZMO_FLAG_ALL),
			"",
			allDisplayed)) {
			if (allDisplayed) {
				gizmoFlags_ = 0;
			}
			else {
				gizmoFlags_ = static_cast<uint8_t>(GIZMO_FLAG_ALL);
			}
		}
		ImGui::EndMenu();
	}
}

GizmoFlags GizmoUtility::DisplayCheckboxEntity(
	GizmoFlags flags,
    GizmoFlagEnum flagToModify) {

	bool doDisplay = flags & flagToModify;
	ImGui::Columns(2);
	if (ImGui::Checkbox(GetGizmoName(flagToModify), &doDisplay)) {
		if (doDisplay) {
			flags -= static_cast<uint8_t>(flagToModify);
		}
		else {
			flags |= static_cast<uint8_t>(flagToModify);
		}
	}
	ImGui::NextColumn();

	Color gizmoColor = GizmoUtility::GetGizmoColor(flagToModify);
	if(ImGui::ColorPicker3(GetGizmoName(flagToModify), &gizmoColor[0])) {
		GizmoUtility::SetGizmoColor(flagToModify, gizmoColor);
	}
         	
	ImGui::Columns(1);
	return flags;
}

const char * GizmoUtility::GetGizmoName(GizmoFlagEnum gizmoFlag)
{
	switch (gizmoFlag) {
	case GIZMO_FLAG_CHUNK: return "Chunk";
	case GIZMO_FLAG_SPLINE: return "Spline";
	case GIZMO_FLAG_SPLINE_POINTS: return "Spline points";
	case GIZMO_FLAG_COLLIDER: return "Collider";
	case GIZMO_FLAG_AABB: return "AABB";
	case GIZMO_FLAG_DIRECTION: return "Direction";
	case GIZMO_FLAG_TRANSFORM: return "Transform";
	case GIZMO_FLAG_ALL: return "All";
	case GIZMO_FLAG_NONE: return "None";
	}
	return "";
}

Color GizmoUtility::GetChunkGizmoColor(
	chunk::ChunkStatus chunkStatus)
{
	switch (chunkStatus) {
	case chunk::ChunkStatus::VISIBLE:
		return gizmoColorChunkVisible_;
	case chunk::ChunkStatus::HIDDEN:
		return gizmoColorChunkHidden_;
	case chunk::ChunkStatus::ACTIVE:
		return gizmoColorChunkActive_;
	case chunk::ChunkStatus::DESTROYED:
		return gizmoColorChunkDestroyed_;
	case chunk::ChunkStatus::LENGTH:
		return Color();
	}
	return Color();
}

void GizmoUtility::SetChunkGizmoColor(
	chunk::ChunkStatus chunkStatus,
    Color newColor)
{
	switch (chunkStatus) {
	case chunk::ChunkStatus::VISIBLE:
		gizmoColorChunkVisible_ = newColor;
		break;
	case chunk::ChunkStatus::HIDDEN:
		gizmoColorChunkHidden_ = newColor;
		break;
	case chunk::ChunkStatus::ACTIVE:
		gizmoColorChunkActive_ = newColor;
		break;
	case chunk::ChunkStatus::DESTROYED:
		gizmoColorChunkDestroyed_ = newColor;
		break;
	case chunk::ChunkStatus::LENGTH:
		break;
	}
}

Color GizmoUtility::GetGizmoColor(GizmoFlagEnum gizmoFlag)
{
	switch (gizmoFlag) {
	case GIZMO_FLAG_CHUNK:
		cassert(false, "You shouldn't call GetGizmoColor for chunks. Call GetChunkGizmoColor() instead");
	    return Color();
	case GIZMO_FLAG_SPLINE: return gizmoColorSpline_;
	case GIZMO_FLAG_COLLIDER: return gizmoColorCollider_;
	case GIZMO_FLAG_AABB: return gizmoColorAabb_;
	case GIZMO_FLAG_DIRECTION: return gizmoColorDirection_;
	case GIZMO_FLAG_TRANSFORM: return gizmoColorTransform_;
	default: break;
	}
	return Color();
}

void GizmoUtility::SetGizmoColor(GizmoFlagEnum gizmoFlag, Color newColor)
{
	switch (gizmoFlag) {
	case GIZMO_FLAG_CHUNK:
		cassert(false, "You shouldn't call SetGizmoColor for chunks. Call SetChunkGizmoColor() instead");
	case GIZMO_FLAG_SPLINE:
	    gizmoColorSpline_ = newColor;
		break;
	case GIZMO_FLAG_COLLIDER: 
		gizmoColorCollider_ = newColor;
		break;
	case GIZMO_FLAG_AABB:
		gizmoColorAabb_= newColor;
		break;
	case GIZMO_FLAG_DIRECTION:
		gizmoColorDirection_ = newColor;
		break;;
	case GIZMO_FLAG_TRANSFORM:
		gizmoColorTransform_ = newColor;
		break;;
	default: break;
	}
}

bool GizmoUtility::DisplaySubMenuGizmoItem(GizmoFlagEnum gizmoFlag)
{
	const bool displayed = gizmoFlags_ & static_cast<uint16_t>(gizmoFlag);
	if (ImGui::MenuItem(
		GizmoUtility::GetGizmoName(gizmoFlag),
		"",
		displayed)) {
		if (displayed) {
			gizmoFlags_ -= static_cast<uint8_t>(gizmoFlag);
		}
		else {
			gizmoFlags_ |= static_cast<uint8_t>(gizmoFlag);
		}
	}
	return displayed;
}
} // namespace editor
} // namespace poke
