#include <Editor/Tools/resources_viewer.h>

#include <string>
#include <imgui.h>

#include <Utility/log.h>
#include <Utility/time_custom.h>
#include <Editor/Tools/resource_accessor_tool.h>
#include <Editor/editor.h>

namespace poke {
namespace editor {
ResourcesViewerTool::ResourcesViewerTool(Editor& editor, const bool defaultActive)
    : Tool(defaultActive),
      editor_(editor)
{
    name_ = "ResourcesViewerTool";
	toolFlag_ = TOOL_FLAG_RESOURCES_VIEWER;
}

std::string ResourcesViewerTool::DisplayListStrings(const std::vector<std::string>& list, const char* label, FileType fileType) const
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

void ResourcesViewerTool::OnDrawImgui() {
	ImGui::Begin(name_.c_str(), &isActive_);

	auto& materialManager = editor_.GetResourcesManagerContainer().editorMaterialsManager;
	auto& meshesManager = editor_.GetResourcesManagerContainer().editorMeshesManager;
	auto& texturesManager = editor_.GetResourcesManagerContainer().editorTexturesManager;

	const std::vector<std::string>& defaultTextures = texturesManager.GetImage2dNames();
	const std::vector<std::string>& defaultImageCubes = texturesManager.GetImageCubeNames();
	const std::vector<std::string>& diffuseMaterials = materialManager.GetMaterialDiffuseNames();
	const std::vector<std::string>& skyboxMaterials = materialManager.GetMaterialSkyboxNames();
	const std::vector<std::string>& trailMaterials = materialManager.GetMaterialTrailNames();
	const std::vector<std::string>& meshes = meshesManager.GetMeshNames();

	std::string outString = DisplayListStrings(defaultTextures, "Loaded Textures", FileType::TEXTURE);
	if (!outString.empty()) texturesManager.AddTexture2D(outString);

	outString = DisplayListStrings(defaultImageCubes, "Loaded Image Cubes", FileType::IMAGE_CUBE);
	if (!outString.empty()) texturesManager.AddTextureCube(outString);

	outString = DisplayListStrings(diffuseMaterials, "Loaded Default materials", FileType::MATERIAL);
	if (!outString.empty()) materialManager.AddMaterial(outString);

	outString = DisplayListStrings(skyboxMaterials, "Loaded Skybox materials", FileType::MATERIAL);
	if (!outString.empty()) materialManager.AddMaterial(outString);

	outString = DisplayListStrings(trailMaterials, "Loaded Trail materials", FileType::MATERIAL);
	if (!outString.empty()) materialManager.AddMaterial(outString);

    outString = DisplayListStrings(meshes, "Loaded Meshes", FileType::MESH);
	if (!outString.empty()) meshesManager.AddMesh(outString);

    ImGui::End();
}

void ResourcesViewerTool::OnSetInactive() { }
void ResourcesViewerTool::OnSetActive() { }
} //namespace editor
} //namespace poke
