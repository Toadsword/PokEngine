#include <Editor/ResourcesManagers/editor_textures_manager.h>

namespace poke {
namespace editor {

EditorTexturesManager::EditorTexturesManager() : CoreTexturesManager() {


}

void EditorTexturesManager::AddTexture2D(const std::string& texturePath)
{
    const auto it = std::find(
        image2dNames_.begin(),
        image2dNames_.end(),
        texturePath);

    if (it != image2dNames_.end()) { return; }

    image2dNames_.push_back(texturePath);

    CoreTexturesManager::AddTexture2D(texturePath);
}

void EditorTexturesManager::AddTextureCube(const std::string& texturePath)
{
    const auto it = std::find(
        imageCubeNames_.begin(),
        imageCubeNames_.end(),
        texturePath);

    if (it != imageCubeNames_.end()) { return; }

    imageCubeNames_.push_back(texturePath);

    CoreTexturesManager::AddTextureCube(texturePath);
}

void EditorTexturesManager::Clear()
{
    CoreTexturesManager::Clear();

    image2dNames_.clear();
    imageCubeNames_.clear();
}

void EditorTexturesManager::SetFromJson(const json& texturesJson)
{
	for (const auto& textureJson : texturesJson) {
		std::string textureName = textureJson;

		//TODO(@Nico) Refactor the scene data to add a separation between 2D and Cube.
		if (textureName.find('.') != std::string::npos) {
			AddTexture2D(textureName);
		}
		else {
			AddTextureCube(textureName);
		}
	}
}

json EditorTexturesManager::ToJson()
{
	json texturesJson;
	//TODO(@Nico) When the separation in the scene is done, make a separation in two different sub-object.
    for (const auto& name : image2dNames_) {
		texturesJson.push_back(name);
    }

	for (const auto& name : imageCubeNames_) {
		texturesJson.push_back(name);
	}

	return texturesJson;
}

const std::vector<std::string>& EditorTexturesManager::GetImage2dNames() const
{
    return image2dNames_;
}

const std::vector<std::string>& EditorTexturesManager::GetImageCubeNames() const
{
    return imageCubeNames_;
}

const std::string EditorTexturesManager::GetImageCubeName(
    const ResourceID resourceID) const
{
    for (size_t i = 0; i < imageCubeIDs_.size(); i++) {
        if (imageCubeIDs_[i] == resourceID) { return imageCubeNames_[i]; }
    }

    return "";
}

const std::string EditorTexturesManager::GetImage2dName(
    const ResourceID resourceID) const
{
    for (size_t i = 0; i < image2dIDs_.size(); i++) {
        if (image2dIDs_[i] == resourceID) { return image2dNames_[i]; }
    }

    return "";
}
} //namespace editor
} //namespace poke
