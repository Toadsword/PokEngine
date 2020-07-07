#include <Editor/Tools/example_tool.h>

#include <string>
#include <imgui.h>
#include <iostream>

#include <Utility/log.h>
#include <Utility/time_custom.h>
#include <Editor/Tools/resource_accessor_tool.h>
#include <Editor/editor.h>

namespace poke {
namespace editor {
ExampleTool::ExampleTool(Editor& editor, const bool defaultActive)
    : Tool(defaultActive),
      editor_(editor)
{
    name_ = "Exemple tool";
}

std::string ExampleTool::DisplayListStrings(const std::vector<std::string>& list, const char* label, FileType fileType) const
{
	Resource resource = INVALID_RESOURCE;
    if(ImGui::CollapsingHeader(label)) {
		resource = ResourceAccessorTool::DisplayGetResource(
			"Load",
			ResourceAccessorTool::DisplayType::BUTTON, 
			fileType, 
			FolderType::ROM);

        for(const std::string& element : list) {
			ImGui::Text(element.c_str());
        }
    }
	if (ResourceAccessorTool::IsResourceValid(resource)) {
		return PokFileSystem::GetSubPathFromFullPath(resource.path, fileType, FolderType::ROM, true);
	}
	return "";
}

void ExampleTool::OnDrawImgui() {
    static int frame = 0;
    ImGui::Begin(name_.c_str(), &isActive_);

    float ms = Time::Get().deltaTime.count();
    ImGui::Text(std::to_string(ms).c_str());
    ImGui::Text(std::to_string(frame).c_str());
    ImGui::Text("Hello");

//TEST RESOURCE ACCESSOR
    Resource outResource = ResourceAccessorTool::DisplayGetResource(
		"Boutton de test", 
		ResourceAccessorTool::DisplayType::BUTTON,
		FileType::PREFAB, 
		FolderType::SAVE);

    if(ResourceAccessorTool::IsResourceValid(outResource)) {
		wantedResource_ = outResource;
    }
    if(ResourceAccessorTool::IsResourceValid(wantedResource_)) {
	    ImGui::Text(("Selected resource : " + wantedResource_.name).c_str());
    }
//END TEST RESOURCE ACCESSOR

	auto& materialManager = editor_.GetResourcesManagerContainer().editorMaterialsManager;
	auto& meshesManager = editor_.GetResourcesManagerContainer().editorMeshesManager;

	const std::vector<std::string>& diffuseMaterials = materialManager.GetMaterialDiffuseNames();
	const std::vector<std::string>& skyboxMaterials = materialManager.GetMaterialSkyboxNames();
	const std::vector<std::string>& meshes = meshesManager.GetMeshNames();

	std::string outString;
	outString = DisplayListStrings(diffuseMaterials, "Loaded Default materials", FileType::MATERIAL);
	if (!outString.empty()) materialManager.AddMaterial(outString);

	outString = DisplayListStrings(skyboxMaterials, "Loaded Skybox materials", FileType::SKYBOX);
	if (!outString.empty()) materialManager.AddMaterial(outString);

    outString = DisplayListStrings(meshes, "Loaded Meshes", FileType::MESH);
	if (!outString.empty()) meshesManager.AddMesh(outString);

    ImGui::End();
    frame++;
}

void ExampleTool::OnSetInactive() { std::cout << "Deactive example tool\n"; }
void ExampleTool::OnSetActive() { std::cout << "Active example tool\n"; }
} //namespace editor
} //namespace poke
