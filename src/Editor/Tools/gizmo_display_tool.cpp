#include <Editor/Tools/gizmo_display_tool.h>

#include <imgui.h>

#include <Editor/gizmo_utility.h>

namespace poke {
namespace editor {

GizmoDisplayTool::GizmoDisplayTool(Editor& editor, bool defaultActive)
    : Tool(defaultActive),
	  editor_(editor)
{
	name_ = "Gizmo tool";
}

void GizmoDisplayTool::OnDrawImgui() {

	ImGui::Begin(name_.c_str(), &isActive_);

	if (ImGui::BeginMenuBar()) {
		GizmoUtility::DisplayGizmoMenu("Guizmo display");

		ImGui::EndMenuBar();
	}

	ImGui::Text("Guizmos options");

    GizmoFlags selectedEntityGizmoFlag = GizmoUtility::GetGizmoFlags();

	if (ImGui::CollapsingHeader(GizmoUtility::GetGizmoName(GIZMO_FLAG_AABB))) {
		selectedEntityGizmoFlag = GizmoUtility::DisplayCheckboxEntity(selectedEntityGizmoFlag, GIZMO_FLAG_AABB);
	}
	if (ImGui::CollapsingHeader(GizmoUtility::GetGizmoName(GIZMO_FLAG_COLLIDER))) {
		selectedEntityGizmoFlag = GizmoUtility::DisplayCheckboxEntity(selectedEntityGizmoFlag, GIZMO_FLAG_COLLIDER);
	}
	if (ImGui::CollapsingHeader(GizmoUtility::GetGizmoName(GIZMO_FLAG_DIRECTION))) {
		selectedEntityGizmoFlag = GizmoUtility::DisplayCheckboxEntity(selectedEntityGizmoFlag, GIZMO_FLAG_DIRECTION);

		float directionLineLength = GizmoUtility::GetDirectionLineLength();
		if (ImGui::DragFloat("Direction line length", &directionLineLength)) {
			GizmoUtility::SetDirectionLineLength(directionLineLength);
		}

		float endArrowSphereRadius = GizmoUtility::GetEndArrowSphereRadius();
		if (ImGui::DragFloat("End arrow sphere radius", &endArrowSphereRadius)) {
			GizmoUtility::SetEndArrowSphereRadius(endArrowSphereRadius);
		}
	}
	if (ImGui::CollapsingHeader(GizmoUtility::GetGizmoName(GIZMO_FLAG_SPLINE))) {
		selectedEntityGizmoFlag = GizmoUtility::DisplayCheckboxEntity(selectedEntityGizmoFlag, GIZMO_FLAG_SPLINE);
	}
	if (ImGui::CollapsingHeader(GizmoUtility::GetGizmoName(GIZMO_FLAG_TRANSFORM))) {
		selectedEntityGizmoFlag = GizmoUtility::DisplayCheckboxEntity(selectedEntityGizmoFlag, GIZMO_FLAG_TRANSFORM);

		float transformPointRadius = GizmoUtility::GetTransformPointRadius();
		if (ImGui::DragFloat("Transform point radius", &transformPointRadius)) {
			GizmoUtility::SetTransformPointRadius(transformPointRadius);
		}
	}

	GizmoUtility::SetGizmoFlags(selectedEntityGizmoFlag);

	ImGui::Separator();
	ImGui::Text("Chunk options : ");
	if (ImGui::CollapsingHeader(GizmoUtility::GetGizmoName(GIZMO_FLAG_CHUNK))) {
		if (ImGui::CollapsingHeader("Active chunks options")) {
			Color gizmoColor = GizmoUtility::GetChunkGizmoColor(chunk::ChunkStatus::ACTIVE);
			if (ImGui::ColorPicker3("Active", &gizmoColor[0])) {
				GizmoUtility::SetChunkGizmoColor(chunk::ChunkStatus::ACTIVE, gizmoColor);
			}
		}
		if (ImGui::CollapsingHeader("Hidden chunks options")) {
			Color gizmoColor = GizmoUtility::GetChunkGizmoColor(chunk::ChunkStatus::HIDDEN);
			if (ImGui::ColorPicker3("Hidden", &gizmoColor[0])) {
				GizmoUtility::SetChunkGizmoColor(chunk::ChunkStatus::HIDDEN, gizmoColor);
			}
		}
		if (ImGui::CollapsingHeader("Visible chunks options")) {
			Color gizmoColor = GizmoUtility::GetChunkGizmoColor(chunk::ChunkStatus::VISIBLE);
			if (ImGui::ColorPicker3("Visible", &gizmoColor[0])) {
				GizmoUtility::SetChunkGizmoColor(chunk::ChunkStatus::VISIBLE, gizmoColor);
			}
		}
		if (ImGui::CollapsingHeader("Destroyed chunks options")) {
			Color gizmoColor = GizmoUtility::GetChunkGizmoColor(chunk::ChunkStatus::DESTROYED);
			if (ImGui::ColorPicker3("Destroyed", &gizmoColor[0])) {
				GizmoUtility::SetChunkGizmoColor(chunk::ChunkStatus::DESTROYED, gizmoColor);
			}
		}

		const uint8_t nbChunks = ChunksManagerLocator::Get().GetChunkNb();
        if(nbChunks) {
	        ImGui::Text("Chunks to display : ");
		    std::vector<int> newNbChunks;
		    std::vector<int> chunkNoList = GizmoUtility::GetChunkIndexsNoDisplayGizmo();
            for(int i = 0; i < nbChunks; i++) {
			    bool doHaveOne = std::find(chunkNoList.begin(), chunkNoList.end(), i) == chunkNoList.end();
			    ImGui::Checkbox(
				    ("Display chunk " + std::to_string(i)).c_str(),
				    &doHaveOne);

                if(!doHaveOne) {
				    newNbChunks.push_back(i);
                }	
            }
		    GizmoUtility::SetChunkIndexsNoDisplayGizmo(newNbChunks);
		}else {
			ImGui::Text("No chunks yet...");
		}
	}

	ImGui::End();
}

void GizmoDisplayTool::OnSetInactive() {}
void GizmoDisplayTool::OnSetActive() {}

} //namespace editor
} //namespace poke
