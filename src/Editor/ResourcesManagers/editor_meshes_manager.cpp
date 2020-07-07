#include <Editor/ResourcesManagers/editor_meshes_manager.h>

namespace poke {
namespace editor {

EditorMeshesManager::EditorMeshesManager() : CoreMeshManager() {
	AddBaseMeshNames();
}

void EditorMeshesManager::AddMesh(const std::string& name)
{
    const auto it = std::find(meshNames_.begin(), meshNames_.end(), name);

    if (it != meshNames_.end()) { return; }

    meshNames_.push_back(name);

    CoreMeshManager::AddMesh(name);
}

void EditorMeshesManager::Clear()
{
    CoreMeshManager::Clear();

    meshNames_.clear();

	AddBaseMeshNames();
}

void EditorMeshesManager::SetFromJson(const json& meshesJson)
{
    for (const auto& meshJson : meshesJson) {
        std::string meshName = meshJson;

        AddMesh(meshName);
    }
}

json EditorMeshesManager::ToJson()
{
    json meshesJson;

    for (const auto& meshName : meshNames_) {
        if(meshName != "Cube" &&
		   meshName != "Sphere" &&
		   meshName != "Plane" &&
		   meshName != "Quad" &&
		   meshName != "LineGizmo" &&
		   meshName != "CubeGizmo" &&
		   meshName != "SphereGizmo")
        meshesJson.push_back(meshName);
    }

    return meshesJson;
}

const std::string EditorMeshesManager::GetMeshName(
    const ResourceID resourceID) const
{
    for (size_t i = 0; i < meshIDs_.size(); i++) {
        if (meshIDs_[i] == resourceID) { return meshNames_[i + 7]; }
    }

	if (resourceID == math::HashString("Cube")) {
		return "Cube";
	}
	if (resourceID == math::HashString("CubeGizmo")) {
		return "CubeGizmo";
	}
	if (resourceID == math::HashString("LineGizmo")) {
		return "LineGizmo";
	}
	if (resourceID == math::HashString("Plane")) {
		return "Plane";
	}
	if (resourceID == math::HashString("Quad")) {
		return "Quad";
	}
	if (resourceID == math::HashString("Sphere")) {
		return "Sphere";
	}
	if (resourceID == math::HashString("SphereGizmo")) {
		return "SphereGizmo";
	}
    return "";
}

void EditorMeshesManager::AddBaseMeshNames() {
	meshNames_.emplace_back("Cube"),
	meshNames_.emplace_back("Sphere"),
	meshNames_.emplace_back("Plane"),
	meshNames_.emplace_back("Quad"),
	meshNames_.emplace_back("LineGizmo"),
	meshNames_.emplace_back("CubeGizmo"),
	meshNames_.emplace_back("SphereGizmo");

    
	std::vector<std::string> meshFiles = PokFileSystem::GetAllFilesInDirectories(
		PokFileSystem::GetPath(FileType::MESH, FolderType::ROM),
		true,
		true);

	for (const std::string& meshFile : meshFiles) {
        if(meshFile.substr(meshFile.size() - 4, 4) == ".obj") {
		    AddMesh(
			    PokFileSystem::GetSubPathFromFullPath(
				    meshFile, 
				    FileType::MESH, 
				    FolderType::ROM)
		    );
        }
	}
}
} //namespace editor
} //namespace poke
