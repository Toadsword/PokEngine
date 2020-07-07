#include <Editor/Showroom/showroom.h>

#include <vector>
#include <string>

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include <CoreEngine/engine.h>
#include <Editor/editor.h>
#include <Utility/file_system.h>
#include <Editor/component_utility.h>

namespace poke
{
namespace editor
{

Showroom::Showroom(Editor& editor, bool defaultActive) :
	Tool(defaultActive),
	editor_(editor),
	modelsManager_(editor.GetEditorEcsManager().GetComponentsManager<ecs::ModelsManager>(
    ))
{	
	isActive_ = defaultActive;
	name_ = "Showroom";
}

void Showroom::OnDrawImgui()
{
	DisplayEntities();
	DisplayComponentWindow();

	if (ImGui::Begin("Showroom", &isActive_)) {

		ImGui::Columns(2);
        if(ImGui::Button("Refresh Lists") || meshFilePaths_.empty()) {          
		    RefreshFilePaths();
        }
		ImGui::NextColumn();

        if(ImGui::Button("Create material")) {
			ImGui::OpenPopup("Name new material");
			newName_ = "new_material";
        }

		bool confirmCreate = false;
		if (ImGui::BeginPopup("Name new material")) {
			ImGui::InputTextWithHint(
				"New material Name",
				"Enter material Name",
				&newName_);
			ImGui::Checkbox(
				"Set to current object",
				&setWithNew_);
			if (ImGui::Button("Create Material")) {
                if(PokFileSystem::CheckFileExists(
					PokFileSystem::GetFullPath(
						newName_, 
						FileType::MATERIAL, 
						FolderType::ROM)
			        ))
				{
                    ImGui::OpenPopup("Confirm Replace Material");
                } else {
					confirmCreate = true;
				    ImGui::CloseCurrentPopup();
				}
			}

			if (ImGui::BeginPopupModal("Confirm Replace Material")) {
				ImGui::Text("A material already exists with that name, want to replace ?");
				
                if(ImGui::Button("Yes")) {
					confirmCreate = true;
					ImGui::CloseCurrentPopup(); // Close "Confirm Replace Material" popup
					ImGui::CloseCurrentPopup(); // Close "Name new material"
                }
				ImGui::SameLine();
				if (ImGui::Button("No")) {
					ImGui::CloseCurrentPopup();
                }
			    ImGui::EndPopup();
			}

			ImGui::EndPopup();
		}
        if(confirmCreate) {
			CreateDefaultMaterial(newName_, setWithNew_);
			isDirty_ = true;
			RefreshFilePaths();
        }
			
		ImGui::NextColumn();

	    ImGui::Columns(3);

	    const graphics::Model model = modelsManager_.GetComponent(selectedEntityIndex_);

        // Mesh Display
	    size_t newIndex = DisplayList(
		    meshFilePaths_, 
		    FileType::MESH,
		    "Mesh",
		    meshIndex_, 
		    model.meshID);
	    if (newIndex != meshIndex_ && newIndex > 0) {
		    meshIndex_ = newIndex;
		    LoadMesh(meshFilePaths_[meshIndex_]);
		    SetMesh(meshFilePaths_[meshIndex_]);
	    }

	    // Materials Display
	    ImGui::NextColumn();
	    newIndex = DisplayList(
		    materialDefaultFilePaths_, 
		    FileType::MATERIAL, 
		    "Materials", 
		    materialDefaultIndex_, 
		    model.materialID);
        if(newIndex != materialDefaultIndex_ && newIndex >= 0) {

		    materialDefaultIndex_ = newIndex;
		    SetMaterial(materialDefaultFilePaths_[materialDefaultIndex_]);
        }

		// Skybox Display
	    ImGui::NextColumn();
	    newIndex = DisplayList(
		    skyboxFilePaths_,
		    FileType::MATERIAL,
		    "Skybox",
			skyboxIndex_,
			currentSkyboxHash_);
	    if (newIndex != skyboxIndex_ && newIndex >= 0) {
			skyboxIndex_ = newIndex;
		    SetSkybox(skyboxFilePaths_[skyboxIndex_]);
	    }
	}
	ImGui::End(); //End of Showroom window
}

void Showroom::OnSetActive() {
	const auto name = SceneManagerLocator::Get().GetActiveScene().GetSceneName();
	cassert(
		name == "ShowroomScene",
		"ShowroomProject file is not right ! Should load base Showroom Scene as a 0 index.");
    
    entitiesNames_.resize(kEntityNameBaseSize_);

	//Load all base materials, meshes, etc...
	RefreshFilePaths();

	//Create entity and assign it by default
	selectedEntityIndex_ = AddModelEntity();
}

void Showroom::OnSetInactive() {
	SceneManagerLocator::Get().LoadScene(fromSceneIndex_);
}

void Showroom::DisplayEntities() {

	ImGui::Begin("Showroom hierarchy");

	//Check if selected entity exist
	if (std::find(
		    showroomEntities_.begin(),
		showroomEntities_.end(),
		    selectedEntityIndex_) == showroomEntities_.end())
	{
		selectedEntityIndex_ = showroomEntities_[0];
	}

	for (ecs::EntityIndex entity : showroomEntities_) {
		DisplayEntity(entity);
	}

	if (ImGui::Button("Create New Entity")) {
		selectedEntityIndex_ = AddModelEntity();
	}
	ImGui::End();
}

void Showroom::DisplayEntity(const ecs::EntityIndex entityIndex) {

	if (entitiesNames_.size() <= entityIndex) {
		entitiesNames_.resize(entityIndex * ecs::kEntityResizeFactor);
	}
	if (entitiesNames_[entityIndex].empty()) {
		entitiesNames_[entityIndex] = "Entity " + std::to_string(entityIndex);
	}

	const std::string entityName = entitiesNames_[entityIndex];

	ecs::EntityIndex currentIndex = selectedEntityIndex_;
    if(ImGui::Selectable(
		entitiesNames_[entityIndex].c_str(),
		selectedEntityIndex_ == entityIndex)) 
	{
		selectedEntityIndex_ = entityIndex;
    }
    // If we selected another entity, load its mesh and material
    if(selectedEntityIndex_ != currentIndex || isDirty_) {
		const graphics::Model model = modelsManager_.GetComponent(
			selectedEntityIndex_);
		
		meshIndex_ = UpdateIndex(meshFilePaths_, model.meshID);
		materialDefaultIndex_ = UpdateIndex(materialDefaultFilePaths_, model.materialID);
    }

	if (ImGui::BeginPopupContextItem(entityName.c_str())) {
		if (ImGui::Button("Rename Entity")) {
			ImGui::OpenPopup("Rename");
			newName_ = entitiesNames_[entityIndex];
		}
		if (ImGui::BeginPopup("Rename")) {
			if (ImGui::InputTextWithHint(
				"Entity Name",
				"Enter entity name",
				&newName_)) {
			}
			if (ImGui::Button("Rename")) {
				entitiesNames_[entityIndex] = newName_;
			}
			ImGui::EndPopup();
		}
		if (ImGui::Button("Delete Entity")) {
			editor_.GetEditorEcsManager().DestroyEntity(entityIndex);
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void Showroom::DisplayComponentWindow() const
{
	// Display Component list window
	if (!editor_.GetEditorEcsManager().HasComponent(
		selectedEntityIndex_,
		ecs::ComponentType::MODEL)
		)
	{
		editor_.GetEditorEcsManager().AddComponent(
			selectedEntityIndex_,
			ecs::ComponentType::MODEL);
	}

	ImGui::Begin("Model options");
	{
		// Display transform
		auto& transformManager = editor_.GetEditorEcsManager()
			.GetComponentsManager<ecs::TransformsManager>(
            );
		transformManager.SetComponent(
            selectedEntityIndex_,
            ComponentUtility::DisplayAndModifyTransform(
                transformManager.GetComponent(selectedEntityIndex_)
            )
        );

		// Display model
		const graphics::Model model = modelsManager_.GetComponent(selectedEntityIndex_);
		const graphics::Model newModel = ComponentUtility::DisplayAndModifyModel(model);
		if (newModel != model) {
			modelsManager_.SetComponent(selectedEntityIndex_, model);
		}
	}
	ImGui::End();
}

int Showroom::DisplayList(
	std::vector<std::string>& filepathList,
	const FileType fileType,
	const char* label,
	int index,
	XXH64_hash_t baseHash) const
{
	ImGui::Text(label);
	if (ImGui::BeginTabBar(label)) {
		//This selectable list is for a representation of what the UI will look like.
		const auto listSize = filepathList.size();
		for (int i = 0; i < listSize; i++) {
			if (ImGui::Selectable(
				filepathList[i].c_str(),
				index == i))
			{
				index = i;
			}
		}
		ImGui::EndTabBar(); //End of Mesh tab bar
	}
	return index;
}

void Showroom::LoadMesh(const std::string& meshName) const {
	editor_
        .GetResourcesManagerContainer()
        .editorMeshesManager
        .AddMesh(meshName);
}

void Showroom::SetMesh(const std::string& meshName) const {
	// Assign loaded mesh to current model
	if (!editor_.GetEditorEcsManager().HasComponent(
		selectedEntityIndex_,
		ecs::ComponentType::MODEL)
		)
	{
		editor_.GetEditorEcsManager().AddComponent(
			selectedEntityIndex_,
			ecs::ComponentType::MODEL);
	}

	graphics::Model model = modelsManager_.GetComponent(selectedEntityIndex_);
	XXH64_hash_t hash = XXH64(
		meshName.c_str(),
		meshName.size(),
		math::kHashSeed);
	model.meshID = hash;
	modelsManager_.SetComponent(selectedEntityIndex_, model);
}

void Showroom::LoadMaterial(const std::string& materialName) const {
	editor_.GetResourcesManagerContainer()
        .editorMaterialsManager
        .AddMaterial(materialName);
}

void Showroom::SetMaterial(const std::string& materialName) const {
	// Assign loaded mesh to current model
	if (!editor_.GetEditorEcsManager().HasComponent(
		selectedEntityIndex_,
		ecs::ComponentType::MODEL)
		)
	{
		editor_.GetEditorEcsManager().AddComponent(
			selectedEntityIndex_,
			ecs::ComponentType::MODEL);
	}

	graphics::Model model = modelsManager_.GetComponent(selectedEntityIndex_);
	XXH64_hash_t hash = XXH64(
		materialName.c_str(),
		materialName.size(),
		math::kHashSeed);
	model.materialID = hash;
	modelsManager_.SetComponent(selectedEntityIndex_, model);
}

void Showroom::SetSkybox(const std::string & skyboxName)
{
	currentSkyboxHash_ = XXH64(
		skyboxName.c_str(),
		skyboxName.size(), 
		math::kHashSeed);

	graphics::Model skyboxModel = modelsManager_.GetComponent(0);
	skyboxModel.materialID = currentSkyboxHash_;
	modelsManager_.SetComponent(0, skyboxModel);
}

void Showroom::CreateDefaultMaterial(
	const std::string& newMaterialName, 
	bool setToActualObject) const
{
	json newMaterial = json();
	newMaterial["blend"] = 1.0f;
	newMaterial["baseDiffuse"]["a"] = 1.0f;
	newMaterial["baseDiffuse"]["r"] = 97.0f / 255.0f;
	newMaterial["baseDiffuse"]["g"] = 95.0f / 255.0f;
	newMaterial["baseDiffuse"]["b"] = 87.0f / 255.0f;
	newMaterial["materialType"] = 0.0f;
	newMaterial["metallic"] = 0.0f;
	newMaterial["roughness"] = 0.0f;
	newMaterial["textureDiffuse"] = "default_diffuse.jpg";
	newMaterial["ignoreLighting"] = false;
	newMaterial["ignoreFog"] = false;

	PokFileSystem::WriteFile(
		newMaterialName, 
		newMaterial, 
		FileType::MATERIAL, 
		FolderType::SAVE_IN_ROM);

	editor_.GetResourcesManagerContainer()
        .editorMaterialsManager
        .AddMaterial(newMaterialName);

	XXH64_hash_t hash = XXH64(
		newMaterialName.c_str(),
		newMaterialName.size(),
		math::kHashSeed);

    // Set new material to current object
    if(setToActualObject) {
	    graphics::Model currentModel = modelsManager_
            .GetComponent(selectedEntityIndex_);
		currentModel.materialID = hash;
	    modelsManager_.SetComponent(selectedEntityIndex_, currentModel);
    }
}

ecs::EntityIndex Showroom::AddModelEntity()
{
	const ecs::EntityIndex entityIndex =
		editor_.GetEditorEcsManager().AddEntity();

	showroomEntities_.push_back(entityIndex);

	editor_.GetEditorEcsManager().AddComponent(
		entityIndex,
		ecs::ComponentType::TRANSFORM);

	editor_.GetEditorEcsManager().AddComponent(
		entityIndex,
		ecs::ComponentType::MODEL);

	isDirty_ = true;

	return entityIndex;
}

int Showroom::UpdateIndex(
	const std::vector<std::string>& list, 
	XXH64_hash_t baseHash)
{
	for (int i = 0; i < list.size(); i++) {
		const std::string& name = list[i];
		const XXH64_hash_t hash = XXH64(
			name.c_str(),
			name.size(),
			math::kHashSeed);

		if (hash == baseHash) {
			return i;
		}
	}
	return 0;
}

void Showroom::RefreshFilePaths() {
    // Loading Mesh. We take them from the path since we don't want
    // to load them all at the beginning and will load them only when needed.
	const std::string resourcePath = PokFileSystem::GetPath(
		FileType::MESH,
		FolderType::ROM);
	const int extensionSize = PokFileSystem::GetExtension(FileType::MESH).size();
	std::vector<std::string> unfilteredFilePaths = 
		PokFileSystem::GetAllFilesInDirectories(
		    resourcePath,
		    true,
		    true);

	meshFilePaths_.clear();
	for (auto& filepath : unfilteredFilePaths) {
		filepath = filepath.substr(
			resourcePath.size(),
			filepath.size() - resourcePath.size() - extensionSize);

        if(filepath.substr(filepath.size() - 4, 4) == ".obj") {
			meshFilePaths_.push_back(filepath);
        }
	}

    // Load all materials
	const std::vector<std::string> allMaterials = 
		PokFileSystem::GetAllFilesInDirectories(
			FileType::MATERIAL, 
			FolderType::ROM
		);
	for (const std::string& materialName : allMaterials) {
		size_t namePos = materialName.find_last_of('/') + 1;
		size_t extensionPos = materialName.find_last_of('.');
		size_t nameSize = materialName.size() - 
			namePos - 
			(materialName.size() - extensionPos);
		LoadMaterial(materialName.substr(namePos, nameSize));
	}

	EditorMaterialsManager& materialsManager = 
		editor_.GetResourcesManagerContainer().editorMaterialsManager;

	skyboxFilePaths_ = materialsManager.GetMaterialSkyboxNames();

	meshIndex_ = -1;
	skyboxIndex_ = .1;
	materialDefaultIndex_ = -1;

	isDirty_ = true;
}

}// namespace editor
}// namespace poke
