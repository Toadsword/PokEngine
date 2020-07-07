#include <Editor/Tools/chunk_editor_tool.h>
#include <Ecs/ecs_utility.h>

namespace poke {
namespace editor {

ChunkEditorTool::ChunkEditorTool(Editor& editor, bool defaultActive) :
	Tool(defaultActive),
	chunkManager_(editor.GetEngine().GetModuleManager().chunksManager),
	editor_(editor)

{
	isActive_ = false;
	name_ = "Chunk Editor Tool";
	toolFlag_ = TOOL_FLAG_CHUNK_EDITOR;

	editor_.GetEngine().AddObserver(observer::MainLoopSubject::DRAW, [this]() {OnDraw(); });
}


void ChunkEditorTool::OnSetActive()
{
	isActive = true;
	SetValuesAtStart();
}

void ChunkEditorTool::OnSetInactive()
{
	isActive = false;
	chunkManager_.SetChunks(chunkArray_, chunkManager_.GetChunkNb());
}

std::vector<chunk::ChunkIndex> ChunkEditorTool::GetChunksToDisplay()
{
	if (isActive) {
		return chunkToDisplayInGuizmo_;
	}
}

void ChunkEditorTool::OnDraw()
{
    for (auto chunkIndex : debugVisibleChunkIndexes_) {
		chunkManager_.DebugDisplayChunk(chunkIndex);
    }

	for (auto chunkIndex : debugHiddenChunkIndexes_) {
		chunkManager_.DebugHideChunk(chunkIndex);
	}
}

void ChunkEditorTool::OnDrawImgui()
{
	ImGui::Begin(kChunkWindow_, &isActive_);
	
	//for change status of chunk
	if (chunkSelected_) {

		//Main fonction for modify chuks values
		SetChunkValues(chunkArray_[selectedIndex_]);

		//Change status of all chunks for the selected chunk
		if (ImGui::Button("Set all")) {

			for (int i = 0; i < chunkManager_.GetChunkNb(); i++) {
				chunkArray_[i].SetStatus(globalStatus_);
			}
		}
	}

	//for add new chunk
	if (ImGui::Button("Add Chunk")) {
		auto chunkIndex = chunkManager_.CreateChunk();
		chunkManager_.SetChunks(chunkArray_, chunkManager_.GetChunkNb());
		SetChunkNames();		
	}

	//for display of chunk names in ImGui
	for (size_t i = 0; i < chunkNames_.size(); i++) {

		if (ImGui::MenuItem(chunkNames_[i].c_str())) {
			chunkEntities_ = chunkArray_[i].GetLinkEntities();
			selectedIndex_ = i;
			chunkSelected_ = true;
		}
	}
	ImGui::End();

	if (sceneIndex != SceneManagerLocator::Get().GetActiveSceneIndex()) {
		SetValuesAtStart();
	}
	chunkManager_.SetChunks(chunkArray_, chunkManager_.GetChunkNb());
}

void ChunkEditorTool::SetChunkValues(chunk::Chunk chunk)
{
	auto status = chunk.GetStatus();

	ImGui::Begin("set chunk values");

	const char* statusStates[] = { "Active", "Visible", "Hidden", "Destroyed" };
	int currentStatus = static_cast<int>(status);
	ImGui::Text(chunkNames_[selectedIndex_].c_str());
	//Set status
	if (ImGui::BeginCombo("Set chunk status", statusStates[static_cast<int>(status)])) {

		for (int i = 0; i < IM_ARRAYSIZE(statusStates); i++) {
			const bool isSelected = currentStatus == i;

			if (ImGui::Selectable(statusStates[i], isSelected)) {
				currentStatus = i;
			}
			if (isSelected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		status = static_cast<chunk::ChunkStatus>(currentStatus);
		chunkArray_[selectedIndex_].SetStatus(status);
		globalStatus_ = status;
		ImGui::EndCombo();
	}

	//set size
	math::Vec3 extent = chunk.GetExtent();	
	ImGui::DragFloat3("size", &extent[0], 0.1f);
	chunkArray_[selectedIndex_].SetExtent(extent);

	//set position
	math::Vec3 position = chunk.GetPosition();
	ImGui::DragFloat3("position", &position[0], 0.1f);
	chunkArray_[selectedIndex_].SetPosition(position);

	if (ImGui::CollapsingHeader("Add entity")) {
		//Add entity in chunk
		if (ImGui::Button("add entity to chunk")) {
			ImGui::OpenPopup("add entity to chunk");
		}
		if (ImGui::BeginPopup("add entity to chunk")) {
			for (auto entity : entityInScene_) {
				if (ImGui::MenuItem(editor_.GetEditorEcsManager().GetEntityName(entity).c_str())) {
					chunkArray_[selectedIndex_].AddLinkEntity(entityInScene_[entity]);
				}
			}
			ImGui::EndPopup();
		}

		//Remove entity in chunk
		ImGui::SameLine();
		if (ImGui::Button("remove entity")) {
			ImGui::OpenPopup("remove entity");
		}
		if (ImGui::BeginPopup("remove entity")) {
			for (auto entity : chunkArray_[selectedIndex_].GetLinkEntities()) {
				if (ImGui::MenuItem(editor_.GetEditorEcsManager().GetEntityName(entity).c_str())) {
					chunkArray_[selectedIndex_].RemoveLinkEntity(entity);
				}
			}
			ImGui::EndPopup();
		}
		ImGui::SameLine();
		if (ImGui::BeginMenu("Entities")) {
			for (auto entity : chunkArray_[selectedIndex_].GetLinkEntities()) {
				ImGui::Text(editor_.GetEditorEcsManager().GetEntityName(entity).c_str());	
			}
			ImGui::EndMenu();
		}
		ImGui::NewLine();
		//Set entities to activate
		if (ImGui::Button("add entity to activate")) {

			ImGui::OpenPopup("add entity to activate");
		}
		if (ImGui::BeginPopup("add entity to activate")) {
			for (auto entity : entityInScene_) {

				if (ImGui::MenuItem(editor_.GetEditorEcsManager().GetEntityName(entity).c_str())) {
					chunkArray_[selectedIndex_].AddEntityToActivate(entityInScene_[entity]);
				}
			}
			ImGui::EndPopup();
		}

		ImGui::SameLine();
		if (ImGui::Button("remove entity to activate")) {
			ImGui::OpenPopup("remove entity to activate");
		}
		if (ImGui::BeginPopup("remove entity to activate")) {
			for (auto entity : chunkArray_[selectedIndex_].GetEntitiesToActivate()) {
				if (ImGui::MenuItem(editor_.GetEditorEcsManager().GetEntityName(entity).c_str())) {
					chunkArray_[selectedIndex_].RemoveEntityToActivate(entity);
				}
			}
			ImGui::EndPopup();
		}
		ImGui::SameLine();
		if(ImGui::BeginMenu("Entities to activate")) {
			for (auto entity : chunkArray_[selectedIndex_].GetEntitiesToActivate()) {
				ImGui::Text(editor_.GetEditorEcsManager().GetEntityName(entity).c_str());
			}
			ImGui::EndMenu();
		}
		ImGui::NewLine();
	}
	//add chunks to destroy/ to hide / to show
	if (ImGui::CollapsingHeader("Add Chunks")) {
		//Set chunksToDestroy
		if (ImGui::Button("add chunk to destroy")) {

			ImGui::OpenPopup("add chunk to destroy");
		}
		if (ImGui::BeginPopup("add chunk to destroy")) {
			for (int i = 0; i < chunkIndexes_.size(); i++) {

				if (ImGui::MenuItem(chunkNames_[i].c_str())) {
					chunkArray_[selectedIndex_].AddChunkToDestroy(chunkIndexes_[i]);
				}
			}
			ImGui::EndPopup();
		}

		ImGui::SameLine();
		if (ImGui::Button("remove destroy")) {
			ImGui::OpenPopup("remove chunk to destroy");
		}
		if (ImGui::BeginPopup("remove chunk to destroy")) {
			for (auto i = 0; i < chunkManager_.GetChunkNb(); i++) {

				if (chunkArray_[selectedIndex_].GetChunksToDestroy()[i]) {
					if (ImGui::MenuItem(chunkNames_[i].c_str())) {
						chunkArray_[selectedIndex_].RemoveChunkToDestroy(i);
					}
				}
		
			}
			ImGui::EndPopup();
		}
		if (ImGui::BeginMenu("Chunk to destroy")) {
			for (int i = 0; i < chunkManager_.GetChunkNb(); i++) {
				if (chunkArray_[selectedIndex_].GetChunksToDestroy()[i]) {
					ImGui::Text(chunkNames_[i].c_str());
				}
			}
			ImGui::EndMenu();
		}
		ImGui::NewLine();
		//Set chunkToHide
		if (ImGui::Button("add chunk to hide")) {

			ImGui::OpenPopup("add chunk to hide");
		}
		if (ImGui::BeginPopup("add chunk to hide")) {
			for (int i = 0; i < chunkIndexes_.size(); i++) {

				if (ImGui::MenuItem(chunkNames_[i].c_str())) {
					chunkArray_[selectedIndex_].AddChunkToHide(chunkIndexes_[i]);
				}
			}
			ImGui::EndPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("remove hide")) {
			ImGui::OpenPopup("remove chunk to hide");
		}
		if (ImGui::BeginPopup("remove chunk to hide")) {
			for (auto i = 0; i < chunkManager_.GetChunkNb(); i++) {

				if (chunkArray_[selectedIndex_].GetChunksToHide()[i]) {
					if (ImGui::MenuItem(chunkNames_[i].c_str())) {
						chunkArray_[selectedIndex_].RemoveChunkToHide(i);
					}
				}
			}
			ImGui::EndPopup();
		}

		if (ImGui::BeginMenu("Chunk to hide")) {
			for (int i = 0; i < chunkManager_.GetChunkNb(); i++) {
				if (chunkArray_[selectedIndex_].GetChunksToHide()[i]) {
					ImGui::Text(chunkNames_[i].c_str());
				}
			}
			ImGui::EndMenu();
		}
		ImGui::NewLine();
		//Set chunkToShow
		if (ImGui::Button("add chunk to show")) {

			ImGui::OpenPopup("add chunk to show");
		}
		if (ImGui::BeginPopup("add chunk to show")) {
			for (int i = 0; i < chunkIndexes_.size(); i++) {

				if (ImGui::MenuItem(chunkNames_[i].c_str())) {
					chunkArray_[selectedIndex_].AddChunkToShow(chunkIndexes_[i]);
				}
			}
			ImGui::EndPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("remove show")) {
			ImGui::OpenPopup("remove chunk to show");
		}
		if (ImGui::BeginPopup("remove chunk to show")) {
			for (auto i = 0; i < chunkManager_.GetChunkNb(); i++) {

				if (chunkArray_[selectedIndex_].GetChunksToShow()[i]) {
					if (ImGui::MenuItem(chunkNames_[i].c_str())) {
						chunkArray_[selectedIndex_].RemoveChunkToShow(i);
					}
				}
			}
			ImGui::EndPopup();
		}
		if (ImGui::BeginMenu("Chunk to show")) {
			for (int i = 0; i < chunkManager_.GetChunkNb(); i++) {
				if (chunkArray_[selectedIndex_].GetChunksToShow()[i]) {
					ImGui::Text(chunkNames_[i].c_str());
				}
			}
			ImGui::EndMenu();
		}
		ImGui::NewLine();
	}

	
	if (ImGui::CollapsingHeader("Set display")) {
		if (ImGui::Button("add to display")) {
			bool isDisplay = false;
			if (chunkToDisplayInGuizmo_.size() < chunkManager_.GetChunkNb()) {
				for (auto index : chunkToDisplayInGuizmo_) {
					if (index == chunkIndexes_[selectedIndex_]) {
						isDisplay = true;
					}
				}
				if (!isDisplay) {
					chunkToDisplayInGuizmo_.push_back(chunkIndexes_[selectedIndex_]);
				}
			}
		}
		if (ImGui::Button("remove from display")) {
			for (auto index : chunkToDisplayInGuizmo_) {
				if (index == chunkIndexes_[selectedIndex_]) {
					chunkToDisplayInGuizmo_.erase(chunkToDisplayInGuizmo_.begin() + index);
				}
			}
		}
	}

	ImGui::Text("Debug visibility : ");
	ImGui::SameLine();
	auto it = std::find(debugVisibleChunkIndexes_.begin(), debugVisibleChunkIndexes_.end(), selectedIndex_);
    if(it != debugVisibleChunkIndexes_.end()) {
		ImGui::Text("Visible");

		if (ImGui::Button("Hide")) {
			debugHiddenChunkIndexes_.push_back(selectedIndex_);
			debugVisibleChunkIndexes_.erase(it);
		}

		if (ImGui::Button("Reset")) {
			debugVisibleChunkIndexes_.erase(it);

			if (chunkManager_.GetChunkStatus(selectedIndex_) == chunk::ChunkStatus::HIDDEN ||
				chunkManager_.GetChunkStatus(selectedIndex_) == chunk::ChunkStatus::DESTROYED) {
				chunkManager_.DebugHideChunk(selectedIndex_);
			}
			else {
				chunkManager_.DebugDisplayChunk(selectedIndex_);
			}
		}
    }else {
		it = std::find(debugHiddenChunkIndexes_.begin(), debugHiddenChunkIndexes_.end(), selectedIndex_);

		if (it != debugHiddenChunkIndexes_.end()) {
			ImGui::Text("Hidden");

			if (ImGui::Button("Show")) {
				debugVisibleChunkIndexes_.push_back(selectedIndex_);
				debugHiddenChunkIndexes_.erase(it);
			}

			if (ImGui::Button("Reset")) {
				debugHiddenChunkIndexes_.erase(it);

                if(chunkManager_.GetChunkStatus(selectedIndex_) == chunk::ChunkStatus::HIDDEN ||
				   chunkManager_.GetChunkStatus(selectedIndex_) == chunk::ChunkStatus::DESTROYED) {
					chunkManager_.DebugHideChunk(selectedIndex_);
                }else {
					chunkManager_.DebugDisplayChunk(selectedIndex_);
                }
			}

		}else {
			ImGui::Text(" ");

			if (ImGui::Button("Show")) {
				debugVisibleChunkIndexes_.push_back(selectedIndex_);
			}

            if(ImGui::Button("Hide")) {
				debugHiddenChunkIndexes_.push_back(selectedIndex_);
            }
		}
    }

	//set all chunks
	chunkManager_.SetChunks(chunkArray_, chunkManager_.GetChunkNb());
	ImGui::End();
}

void ChunkEditorTool::SetChunkNames()
{
	chunkNames_.clear();
	chunkIndexes_.resize(chunkManager_.GetChunkNb());
	for (int i = 0; i < chunkManager_.GetChunkNb(); i++) {
		std::string chunkName = "chunk";

		chunkName += std::to_string(i);
		chunkNames_.emplace_back(chunkName);

		chunkIndexes_[i] = i;
	}
    if(chunkManager_.GetChunkNb() > 0) {
	    chunkToDisplayInGuizmo_.push_back(chunkIndexes_.back());
    }
}

void ChunkEditorTool::SetValuesAtStart()
{
	chunkArray_ = chunkManager_.GetChunks();
	SetChunkNames();
	entityInScene_ = editor_.GetEditorEcsManager().GetNotEmptyEntities();
	chunkToDisplayInGuizmo_ = chunkIndexes_;
	sceneIndex = SceneManagerLocator::Get().GetActiveSceneIndex();
}
}//namespace editor
}//namespace poke
