#include <Editor/ResourcesManagers/editor_materials_manager.h>
#include <Utility/log.h>

namespace poke {
namespace editor {

EditorMaterialsManager::EditorMaterialsManager()
    : CoreMaterialsManager() { /*SetBaseMaterialNames();*/ }

void EditorMaterialsManager::AddMaterial(const std::string& materialName)
{
    //Check if already exist
    auto it = std::find(
        skyboxMaterialNames_.begin(),
        skyboxMaterialNames_.end(),
        materialName);
    if (it != skyboxMaterialNames_.end()) { return; }

    it = std::find(
        diffuseMaterialNames_.begin(),
        diffuseMaterialNames_.end(),
        materialName);
    if (it != diffuseMaterialNames_.end()) { return; }

	it = std::find(
		particleMaterialNames_.begin(),
		particleMaterialNames_.end(),
		materialName);
	if (it != particleMaterialNames_.end()) { return; }

	it = std::find(
		trailMaterialNames_.begin(),
		trailMaterialNames_.end(),
		materialName);
	if (it != trailMaterialNames_.end()) { return; }

    //If the materials is not loaded yet, then load it
    const auto materialJson = LoadJson(materialName, FileType::MATERIAL);

    if (materialJson.is_null()) { return; }

    const graphics::MaterialType materialType = materialJson["materialType"];

    switch (materialType) {
    case graphics::MaterialType::SKYBOX:
        skyboxMaterialNames_.push_back(materialName);
        break;
    case graphics::MaterialType::DIFFUSE:
        diffuseMaterialNames_.push_back(materialName);
        break;
	case graphics::MaterialType::TRAIL:
		trailMaterialNames_.push_back(materialName);
		break;
	case graphics::MaterialType::PARTICLE:
		particleMaterialNames_.push_back(materialName);
		break;
    default: ;
    }

    CoreMaterialsManager::AddMaterial(materialName);
}


void EditorMaterialsManager::Clear()
{
    CoreMaterialsManager::Clear();

    skyboxMaterialNames_.clear();
    diffuseMaterialNames_.clear();
	particleMaterialNames_.clear();
	trailMaterialNames_.clear();
}

void EditorMaterialsManager::SetFromJson(const json& materialsJson)
{
    for (const auto& materialJson : materialsJson) {
        std::string materialsName = materialJson;

        AddMaterial(materialsName);
    }
}

json EditorMaterialsManager::ToJson()
{
    json materialsJson;

    for (const auto& materialName : skyboxMaterialNames_) {
        if (materialName != "default_skybox_material_name") {
            materialsJson.push_back(materialName);
        }
    }

    for (const auto& materialName : diffuseMaterialNames_) {
        if (materialName != "default_material_name") { materialsJson.push_back(materialName); }
    }

	for (const auto& materialName : trailMaterialNames_) {
		materialsJson.push_back(materialName);
	}

	for (const auto& materialName : particleMaterialNames_) {
		materialsJson.push_back(materialName); 
	}

    return materialsJson;
}

graphics::MaterialType EditorMaterialsManager::GetMaterialType(
    const std::string& materialName) { return GetMaterialType(math::HashString(materialName)); }

graphics::MaterialType EditorMaterialsManager::GetMaterialType(
    const ResourceID resourceID)
{
    auto it = std::find(
        skyboxMaterialIDs_.begin(),
        skyboxMaterialIDs_.end(),
        resourceID);
    if (it != skyboxMaterialIDs_.end()) { return graphics::MaterialType::SKYBOX; }

    it = std::find(
        diffuseMaterialIDs_.begin(),
        diffuseMaterialIDs_.end(),
        resourceID);
    if (it != diffuseMaterialIDs_.end()) { return graphics::MaterialType::DIFFUSE; }

    cassert(false, "the material doesn't exist");
}

MaterialSkybox& EditorMaterialsManager::GetMaterialSkybox(
    const std::string& materialName) { return GetMaterialSkybox(math::HashString(materialName)); }

MaterialSkybox& EditorMaterialsManager::GetMaterialSkybox(
    const ResourceID resourceID)
{
    const auto it = std::find(skyboxMaterialIDs_.begin(), skyboxMaterialIDs_.end(), resourceID);

    auto index = std::distance(skyboxMaterialIDs_.begin(), it);

    if (it == skyboxMaterialIDs_.end()) {
        LogWarning(
            "You're trying to get a skybox material that is not in the manager : " + std::to_string(
                resourceID));
        index = 0;
    }

    return skyboxMaterials_[index];
}

MaterialDiffuse& EditorMaterialsManager::GetMaterialDiffuse(const std::string& materialName)
{
    return GetMaterialDiffuse(math::HashString(materialName));
}

MaterialDiffuse& EditorMaterialsManager::GetMaterialDiffuse(const ResourceID resourceID)
{
    const auto it = std::find(diffuseMaterialIDs_.begin(), diffuseMaterialIDs_.end(), resourceID);

    auto index = std::distance(diffuseMaterialIDs_.begin(), it);

    if (it == diffuseMaterialIDs_.end()) {
        LogWarning(
            "You're trying to get a diffuse material that is not in the manager : " + std::
            to_string(resourceID));
        index = 0;
    }

    return diffuseMaterials_[index];
}

MaterialTrail& EditorMaterialsManager::GetMaterialTrail(const std::string& materialName)
{
    return GetMaterialTrail(math::HashString(materialName));
}

MaterialTrail& EditorMaterialsManager::GetMaterialTrail(const ResourceID resourceID)
{
    const auto it = std::find(trailMaterialIDs_.begin(), trailMaterialIDs_.end(), resourceID);

    auto index = std::distance(trailMaterialIDs_.begin(), it);

    if (it == trailMaterialIDs_.end()) {
        LogWarning(
            "You're trying to get a diffuse material that is not in the manager : " + std::
            to_string(resourceID));
        index = 0;
    }

    return trailMaterials_[index];
}

MaterialParticle& EditorMaterialsManager::GetMaterialParticle(const std::string& materialName)
{
	return GetMaterialParticle(math::HashString(materialName));
}

MaterialParticle& EditorMaterialsManager::GetMaterialParticle(const ResourceID resourceID)
{
	const auto it = std::find(particleMaterialIDs_.begin(), particleMaterialIDs_.end(), resourceID);

	auto index = std::distance(particleMaterialIDs_.begin(), it);

	if (it == particleMaterialIDs_.end()) {
		LogWarning(
			"You're trying to get a diffuse material that is not in the manager : " + std::
			to_string(resourceID));
		index = 0;
	}

	return particleMaterials_[index];
}

const std::vector<std::string>& EditorMaterialsManager::
GetMaterialSkyboxNames() const { return skyboxMaterialNames_; }

const std::vector<std::string>& EditorMaterialsManager::GetMaterialDiffuseNames() const
{
    return diffuseMaterialNames_;
}

const std::vector<std::string>& EditorMaterialsManager::GetMaterialTrailNames() const
{
	return trailMaterialNames_;
}

const std::vector<std::string>& EditorMaterialsManager::GetMaterialParticlesNames() const
{
	return particleMaterialNames_;
}

const std::string& EditorMaterialsManager::GetMaterialName(
    const ResourceID resourceID)
{
    for (size_t i = 0; i < diffuseMaterialIDs_.size(); i++) {
        if (diffuseMaterialIDs_[i] == resourceID) { return diffuseMaterialNames_[i]; }
    }

    for (size_t i = 0; i < skyboxMaterialIDs_.size(); i++) {
        if (skyboxMaterialIDs_[i] == resourceID) { return skyboxMaterialNames_[i]; }
    }

	for (size_t i = 0; i < particleMaterialIDs_.size(); i++) {
		if (particleMaterialIDs_[i] == resourceID) { return particleMaterialNames_[i]; }
	}

	for (size_t i = 0; i < trailMaterialIDs_.size(); i++) {
		if (trailMaterialIDs_[i] == resourceID) { return trailMaterialNames_[i]; }
	}

    LogWarning("You're trying to get the name of a non existing material : " + resourceID);
    return "";
}

void EditorMaterialsManager::SetMaterial(
    const MaterialSkybox& material,
    const std::string& materialName) { SetMaterial(material, math::HashString(materialName)); }

void EditorMaterialsManager::SetMaterial(
    const MaterialSkybox& material,
    const ResourceID resourceID)
{
    for (size_t i = 0; i < skyboxMaterialIDs_.size(); i++) {
        if (skyboxMaterialIDs_[i] == resourceID) {
            skyboxMaterials_[i] = material;

            PokFileSystem::WriteFile(
                skyboxMaterialNames_[i],
                skyboxMaterials_[i].ToJson(),
                FileType::MATERIAL);
            break;
        }
    }
}

void EditorMaterialsManager::SetMaterial(
    const MaterialDiffuse& material,
    const std::string& materialName) { SetMaterial(material, math::HashString(materialName)); }

void EditorMaterialsManager::SetMaterial(const MaterialDiffuse& material, ResourceID resourceID)
{
    for (size_t i = 0; i < diffuseMaterialIDs_.size(); i++) {
        if (diffuseMaterialIDs_[i] == resourceID) {
            diffuseMaterials_[i] = material;

            PokFileSystem::WriteFile(
                diffuseMaterialNames_[i],
                diffuseMaterials_[i].ToJson(),
                FileType::MATERIAL);
            break;
        }
    }
}

void EditorMaterialsManager::SetMaterial(
    const MaterialTrail& material,
    const std::string& materialName) { SetMaterial(material, math::HashString(materialName)); }

void EditorMaterialsManager::SetMaterial(const MaterialTrail& material, const ResourceID resourceID)
{
    for (size_t i = 0; i < trailMaterialIDs_.size(); i++) {
        if (trailMaterialIDs_[i] == resourceID) {
            trailMaterials_[i] = material;

            PokFileSystem::WriteFile(
                trailMaterialNames_[i],
                trailMaterials_[i].ToJson(),
                FileType::MATERIAL);
            break;
        }
    }
}

void EditorMaterialsManager::SetMaterial(
    const MaterialParticle& material,
    const std::string& materialName)
{
	SetMaterial(material, math::HashString(materialName));
}

void EditorMaterialsManager::SetMaterial(const MaterialParticle& material, ResourceID resourceID)
{
	for (size_t i = 0; i < particleMaterialIDs_.size(); i++) {
		if (particleMaterialIDs_[i] == resourceID) {
			particleMaterials_[i] = material;

			PokFileSystem::WriteFile(
				particleMaterialNames_[i],
				particleMaterials_[i].ToJson(),
				FileType::MATERIAL);
			break;
		}
	}
}

void EditorMaterialsManager::SetBaseMaterialNames()
{
    std::vector<std::string> materialFiles = PokFileSystem::GetAllFilesInDirectories(
        PokFileSystem::GetPath(FileType::MATERIAL, FolderType::ROM),
        true,
        true);

    for (const std::string& materialFile : materialFiles) {
        AddMaterial(
            PokFileSystem::GetSubPathFromFullPath(
                materialFile,
                FileType::MATERIAL,
                FolderType::ROM, false));
    }

	diffuseMaterialNames_.emplace_back("default_material_name");
	skyboxMaterialNames_.emplace_back("default_skybox_material_name");
}
} //namespace editor
} //namespace poke
